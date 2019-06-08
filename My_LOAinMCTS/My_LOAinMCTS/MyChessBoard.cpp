#include "pch.h"
#include "MyChessBoard.h"
#include <iostream>
#include <bitset>
#include <bitset>
#include <bitset>


namespace loa {
	std::vector<POSITION > MyChessBoard::getFeasible(POSITION pos)
	{
		std::vector<POSITION> vec;
		POSITION tmp;
		if (getFeasibleDir(pos, std::make_pair(-1, -1), tmp)) vec.push_back(tmp);
		if (getFeasibleDir(pos, std::make_pair(-1,  0), tmp)) vec.push_back(tmp);
		if (getFeasibleDir(pos, std::make_pair(-1,  1), tmp)) vec.push_back(tmp);
		if (getFeasibleDir(pos, std::make_pair( 0, -1), tmp)) vec.push_back(tmp);
		if (getFeasibleDir(pos, std::make_pair( 0,  1), tmp)) vec.push_back(tmp);
		if (getFeasibleDir(pos, std::make_pair( 1, -1), tmp)) vec.push_back(tmp);
		if (getFeasibleDir(pos, std::make_pair( 1,  0), tmp)) vec.push_back(tmp);
		if (getFeasibleDir(pos, std::make_pair( 1,  1), tmp)) vec.push_back(tmp);
		return std::move(vec);
	}

	bool MyChessBoard::getFeasibleDir(const POSITION pos, const POSITION dir, POSITION& des)
	{
		int x = pos.first, y = pos.second;
		uint8_t nums = getNumofPiecesDir(pos, dir);
		for (int i = 1; i < nums; ++i)
		{
			x += dir.first;
			y += dir.second;
			if (x < 0 || x > 7 || y < 0 || y > 7) return false;
			if (getColor(x * 8 + y) != Color::NONE && getColor(x * 8 + y) != WhoseTurn) return false;
		}
		x += dir.first;
		y += dir.second;
		if (x < 0 || x > 7 || y < 0 || y > 7) return false;
		if (getColor(x * 8 + y) == WhoseTurn) return false;
		des = std::make_pair(x, y);
		return true;
	}

	uint8_t MyChessBoard::getNumofPiecesDir(const POSITION pos, const POSITION dir)
	{
		uint32_t cnt = 0;
		int x = pos.first, y = pos.second;
		while (1)
		{
			x -= dir.first;
			y -= dir.second;
			if (x < 0 || x > 7 || y < 0 || y > 7)
			{
				break;
			}
		} // 刚好超出棋盘范围
		while (1)
		{
			x += dir.first;  //  刚好进入棋盘
			y += dir.second;
			if (x < 0 || x > 7 || y < 0 || y > 7)
			{
				break;
			}
			if (getColor(x * 8 + y) != Color::NONE) ++cnt;
		}
		return cnt;
	}

	bool MyChessBoard::update(const POSITION& pos, const POSITION& des)  // return whether a piece got eaten.
	{
		if (WhoseTurn == Color::BLACK)
		{
			WhoseTurn = Color::WHITE;
			black_pieces.reset(pos.first * 8 + pos.second);
			black_pieces.set(des.first * 8 + des.second);
			if (white_pieces.test(des.first*8+des.second))
			{
				white_pieces.reset(des.first * 8 + des.second);
				num_of_pieces.second--;
				
				return true;
			}
		}
		else
		{
			WhoseTurn = Color::BLACK;
			white_pieces.reset(pos.first * 8 + pos.second);
			white_pieces.set(des.first * 8 + des.second);
			if (black_pieces.test(des.first * 8 + des.second))
			{
				black_pieces.reset(des.first * 8 + des.second);
				num_of_pieces.first--;
				
				return true;
			}
		}
		return false;
	}
	Color MyChessBoard::checkWinner()
	{
		if (num_of_pieces.first == 1) return Color::WHITE;
		else if (num_of_pieces.second == 1) return  Color::BLACK;

		std::bitset<64> b_flagboard = black_pieces, w_flag_board = white_pieces;
		for (auto i = 0; i < 64; ++i)
		{
			if(b_flagboard.test(i))
			{
				dfs(b_flagboard, i);
				if (b_flagboard.count() == 0) return Color::BLACK;
				else break;
			}
		}

		for (auto i = 0; i < 64; ++i)
		{
			if (w_flag_board.test(i))
			{
				dfs(w_flag_board, i);
				if (w_flag_board.count() == 0) return Color::WHITE;
				else break;
			}
		}
		return Color::NONE;
	}

	void MyChessBoard::dfs(std::bitset<64>& board, int pos)
	{
		if (board.test(pos))
		{
			board.reset(pos);
			if (pos % 8 != 0)
			{
				if (pos - 9 > 0) dfs(board, pos - 9);
				if (pos - 1 > 0) dfs(board, pos - 1);
				if (pos + 7 < 64) dfs(board, pos + 7);
			}
			if (pos - 8 > 0 ) dfs(board, pos - 8);
			if (pos + 8 < 64) dfs(board, pos + 8);
			if (pos % 8 != 7)
			{
				if (pos - 7 > 0) dfs(board, pos - 7);
				if (pos + 1 < 64) dfs(board, pos + 1);
				if (pos + 9 < 64) dfs(board, pos + 9);
			}
		}
	}

}
