#include "pch.h"
#include "MCTS.h"
#include <iostream>

namespace loa {

	void MC_NODE::freeNodes(MC_NODE* root)
	{
		for (auto p : children)
		{
			if (p == root) continue;
			p->freeNodes(nullptr);
			delete p;
			// std::cout << "freenode" << std::endl;
			// p = nullptr;
		}
	}

	bool MC_NODE::isFullExpanded()
	{
		for (auto& iter: children)
		{
			if (iter->simCounts == 0) return false;
		}
		return true;
	}

	// ! TODO
	MC_NODE* MC_NODE::bestChild(bool simornot)
	{
		int cnt = simCounts;
		MC_NODE* res = nullptr;
		double score = 0, max_score = 0;
		for (auto iter: children)
		{
			if (iter->simCounts)
			{
				if (simornot) score = (iter->winCounts*1.0+cnt/5.0) / iter->simCounts;
				else score = (iter->winCounts*1.0) / iter->simCounts;
				if (score > max_score)
				{
					max_score = score;
					res = iter;
				}
			}
		}
		return res;
	}

	void MCT::getAINextMove(MyChessBoard& cb)
	{
		MC_NODE* local_ptr = nullptr;

		cnt_mtx.lock();
		while (cnt > 0);
		local_ptr = lastlastRoot;
		lastlastRoot = lastRoot;
		lastRoot = rootNode;
		nodeptr_mtx.lock();
		if (rootNode->children.empty())
		{
			bornChildren(rootNode);
			if (rootNode->children.empty())
			{
				std::cout << "canNotgetAInextmove" << std::endl;
				exit(0);
			}
		}
		if (rootNode->bestChild(false) == nullptr)
			rootNode = (rootNode->children[0]);
		else 
			rootNode = rootNode->bestChild(false);
		nodeptr_mtx.unlock();
		rootNode->parent = nullptr;

		cb = rootNode->chess_board;
		if (local_ptr)
		{
			std::cout << "into AI freenode" << std::endl;
			local_ptr->freeNodes(lastlastRoot);
			std::cout << "leave AI freenode" << std::endl;
			delete local_ptr;
		}
		cnt_mtx.unlock();
	}

	void MCT::postOpMove(const POSITION& pos, const POSITION& des)
	{
		MC_NODE* local_ptr = nullptr;
		MyChessBoard cb = rootNode->chess_board;

		cb.update(pos, des);

		cnt_mtx.lock();
		while (cnt > 0);
		local_ptr = lastlastRoot;
		lastlastRoot = lastRoot;
		lastRoot = rootNode;
		nodeptr_mtx.lock();
		if (rootNode->children.empty()) bornChildren(rootNode);
		for (auto &iter: rootNode->children)
		{
			if (iter->chess_board == cb)
			{
				rootNode = iter;
				break;
			}
		}
		nodeptr_mtx.unlock();

		rootNode->parent = nullptr;
		if (local_ptr)
		{
			std::cout << "into freenode" << std::endl;
			local_ptr->freeNodes(lastlastRoot);
			std::cout << "leave freenode" << std::endl;
			delete local_ptr;
		}
		cnt_mtx.unlock();
	}

	void MCT::backOneStep()
	{
		
	}

	void MCT::startUCTSearch()
	{
		std::cout << "thread" << std::endl;
		MC_NODE* local_root = nullptr;
		while(1)
		{
			cnt_mtx.lock();
			cnt_mtx2.lock();
			cnt++;
			cnt_mtx2.unlock();
			cnt_mtx.unlock();

			nodeptr_mtx.lock();
			local_root = rootNode;
			nodeptr_mtx.unlock();

			MC_NODE* leaf = nullptr;
			// std::cout << local_root << std::endl;
			leaf= treePolicy(local_root);
			int reward = defaultPolicy(leaf);

			backup_mtx.lock();
			backup(leaf, reward);
			backup_mtx.unlock();
			cnt_mtx2.lock();
			cnt--;
			cnt_mtx2.unlock();
			// std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	MC_NODE* MCT::treePolicy(MC_NODE* p)
	{
		if (!p) return nullptr;
		while (p->chess_board.checkWinner() == Color::NONE)
		{
			if (!p->isFullExpanded()) return expand(p);
			else
			{
				p = p->bestChild(true);
			}
		}
		return nullptr;
	}

	MC_NODE* MCT::expand(MC_NODE* node)
	{
		for (auto iter: node->children)
		{
			if (iter->simCounts == 0)
			{
				bornChildren(iter);
				return iter;
			}
		}
		return nullptr;
	}

	int MCT::defaultPolicy(MC_NODE* p)
	{
		if (p == nullptr) return 1;
		MyChessBoard local_chessboard = p->chess_board;
		int cnt = 0;
		while (local_chessboard.checkWinner() != Color::NONE)
		{
			sim_move(local_chessboard);
			if (cnt++ > 10000) break;
		}
		if (local_chessboard.checkWinner() == Color::NONE) return 1;
		if (local_chessboard.checkWinner() == AI_color) return 2;
		else return 0;

	}

	void MCT::backup(MC_NODE* p, int reward)
	{
		while( p != nullptr )
		{
			p->simCounts += 2;   // 平局一分  胜利两分
			p->winCounts += reward;
			p = p->parent;
			reward = 2 - reward;
		}
	}

	void MCT::bornChildren(MC_NODE* p)
	{
		born_mtx.lock();
		if (!p->children.empty())
		{
			born_mtx.unlock();
			return;
		}
		Color color = p->chess_board.getWhoseTurn();
		MyChessBoard localChessboard;
		MC_NODE *localNode_ptr = nullptr;
		for (uint32_t i = 0; i < 64; ++i)
		{
			if (p->chess_board.getColor(i) == color)
			{
				POSITION pos(i / 8, i % 8);
				std::vector<POSITION> destins = p->chess_board.getFeasible(pos);
				for (auto iter: destins)
				{
					localChessboard = p->chess_board;
					localChessboard.update(pos, iter);
					localNode_ptr = new MC_NODE(localChessboard);
					localNode_ptr->parent = p;
					p->children.push_back(localNode_ptr);
				}
			}
		}

		born_mtx.unlock();
	}

	void MCT::sim_move(MyChessBoard& cb)
	{
		while (cb.checkWinner()!=Color::NONE)
		{
			int cnt = 0;
			// std::bitset<64> visited(0);
			for (uint32_t i = rand()%64;;i = rand() % 64)
			{
				if (cb.getColor(i) == cb.getWhoseTurn())
				{
					if (cnt++ > 100) return;
					POSITION pos(i / 8, i % 8);
					std::vector<POSITION> destins = cb.getFeasible(pos);
					
					if (destins.empty()) continue;
					
					int tmp = rand() % destins.size();
					cb.update(pos, destins[tmp]);
					break;
				}
			}
		}
	}


}
