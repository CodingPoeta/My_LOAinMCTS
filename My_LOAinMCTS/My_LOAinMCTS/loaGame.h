#pragma once
#include "MyChessBoard.h"
#include <vector>

namespace  loa {
	class loaGame
	{
	private:
		MyChessBoard chessBoard;
		std::vector<POSITION> validPositions;
		void updateValid(int8_t i, int8_t j);
		void getInput(POSITION& pos, POSITION& des);

		void mode1();
		void mode2();
		void mode3();
		void mode4();
		
		void drawBoard();
	public:
		loaGame();
		~loaGame();

		void start();
	};
}