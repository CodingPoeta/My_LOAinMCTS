#pragma once
#include "MyChessBoard.h"
#include <thread>
#include <mutex>

const int threadNum = 4;

namespace loa {
	class MC_NODE
	{
	public:
		MyChessBoard chess_board;
		MC_NODE* parent;
		std::vector<MC_NODE*> children;
		int simCounts, winCounts;

		MC_NODE(const MyChessBoard& cb):chess_board(cb), parent(nullptr), simCounts(0), winCounts(0) {}
		~MC_NODE(){}

		void freeNodes(MC_NODE* root);
		bool isFullExpanded();
		MC_NODE* bestChild(bool simornot);
	private:
		MC_NODE(){}
	};

	class MCT
	{
	public:
		MCT(const MyChessBoard& cb, const Color& c) :lastRoot(nullptr), lastlastRoot(nullptr), cnt(0), AI_color(c)
		{
			rootNode = new MC_NODE(cb);
			bornChildren(rootNode);
			std::thread t;
			for(int i = 0; i < threadNum; ++i)
			{
				t = std::thread(&MCT::startUCTSearch, this);
				t.detach();
			}
		}
		~MCT() {}

		void postOpMove( const POSITION& pos, const POSITION& des );    //提交对手的走法
		void getAINextMove(MyChessBoard& cb);             //时间到了之后获取MCTS中的最佳走法
		void backOneStep();  //悔棋

	private:
		std::mutex backup_mtx, cnt_mtx, cnt_mtx2, nodeptr_mtx, born_mtx;
		Color AI_color;
		int msg_cnt;
		volatile int cnt;    //用于记录正在uctsearch但是还没有backup的线程数目
		MC_NODE *rootNode, *lastRoot, *lastlastRoot;

		MCT(){}

		void startUCTSearch();
		MC_NODE* treePolicy(MC_NODE* p);
		MC_NODE* expand(MC_NODE* p);
		int  defaultPolicy(MC_NODE* p);
		void backup(MC_NODE* p, int reward);
		void bornChildren(MC_NODE* p);

		void sim_move(MyChessBoard& cb);
	};
}
