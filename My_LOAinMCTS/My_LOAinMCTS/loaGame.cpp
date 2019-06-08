#include "pch.h"
#include "loaGame.h"
#include <iostream>


namespace loa {
	loaGame::loaGame()
	{
	}

	loaGame::~loaGame()
	{
	}

	void loaGame::start()
	{
		std::cout << "Please choose game type: " << std::endl;
		std::cout << "1 for man vs man;" << std::endl;
		std::cout << "2 for man vs AI ;" << std::endl;
		std::cout << "3 for  AI vs man;" << std::endl;
		std::cout << "4 for  AI vs AI ;" << std::endl;
		int8_t gamemode = 0;
		std::cin >> gamemode;
		while (gamemode < '1' || gamemode > '4')
		{
			std::cout << "\nInvalid input, do it again.  " << gamemode << std::endl;
			std::cin >> gamemode;
		}
		switch (gamemode)
		{
		case '1': mode1(); break;
		case '2': mode2(); break;
		case '3': mode3(); break;
		case '4': mode4(); break;
		default: std::cout << "gamemode error" << std::endl;
		}
	}
	void loaGame::getInput(POSITION& pos, POSITION& des)
	{
		int pos_x, pos_y, des_x, des_y;
		while (1)
		{
			validPositions.clear();
			
			std::cout << "input the piece you want to move(";
			if (chessBoard.getWhoseTurn() == Color::BLACK) std::cout << "Black";
			else std::cout << "White";
			std::cout << "): " << std::endl;
			
			drawBoard();
			std::cout << "rows(1-8): ";
			std::cin >> pos_x;
			std::cout << "cols(1-8): ";
			std::cin >> pos_y;
			if (pos_x < 1 || pos_x > 8 || pos_y < 1 || pos_y > 8)
			{
				if (std::cin.fail())
				{
					std::cin.clear();
					std::cin.get();// std::cin.sync();
				}
				continue;
			}
			if (chessBoard.getColor((pos_x - 1) * 8 + (pos_y - 1)) != chessBoard.getWhoseTurn())
			{
				std::cout << "wrong piece" << std::endl;
				continue;
			}
			updateValid( pos_x - 1, pos_y - 1);
			if (validPositions.empty())
			{
				std::cout << "cannot move this piece" << std::endl;
				continue;
			}
			
			std::cout << "input the destination you want to move(";
			if (chessBoard.getWhoseTurn() == Color::BLACK) std::cout << "Black";
			else std::cout << "White";
			std::cout << "): " << std::endl;

			drawBoard();
			while (1)
			{
				std::cout << "rows(1-8): ";
				std::cin >> des_x;
				std::cout << "cols(1-8): ";
				std::cin >> des_y;
				if (des_x < 1 || des_x > 8 || des_y < 1 || des_y > 8)
				{
					if (std::cin.fail())
					{
						std::cin.clear();
						std::cin.get();// std::cin.sync();
					}
					continue;
				}
				break;
			}
			std::cout << "move " << pos_x << "," << pos_y << " to " << des_x << "," << des_y << std::endl;
			bool isValid = false;
			for (auto p : validPositions)
			{
				// std::cout << "valid " << (int)p.first << "," << (int)p.second << std::endl;
				if (p == std::make_pair(int8_t(des_x-1), int8_t(des_y-1)))
				{
					isValid = true;
					break;
				}
			}
			if (isValid) break;
			std::cout << "Invalid input..." << std::endl;
		}
		pos.first  = pos_x-1;
		pos.second = pos_y-1;
		des.first  = des_x-1;
		des.second = des_y-1;
	}

	void loaGame::mode1()   // man vs man
	{
		POSITION pos, des;
		while (1)
		{
			getInput(pos, des);
			chessBoard.update(pos, des);
			if (chessBoard.checkWinner() != Color::NONE) break;
		}
		if (chessBoard.checkWinner() == Color::BLACK) std::cout << "Black wins" << std::endl;
		else std::cout << "White wins" << std::endl;
	}

	void loaGame::mode2()   // man vs AI
	{
	}

	void loaGame::mode3()   //  AI vs man
	{
	}

	void loaGame::mode4()   //  AI vs AI
	{
	}

	void loaGame::updateValid(int8_t i, int8_t j)
	{
		validPositions = chessBoard.getFeasible(std::make_pair(i,j));
	}


	void loaGame::drawBoard()
	{
		std::cout << "Black: ¡ñ, White: ¡ð, Valid target: ¡Á, Blank: ©à;" << std::endl;
		std::cout << "  1 2 3 4 5 6 7 8 " << std::endl;
		for (int8_t i = 0; i < 8; ++i)
		{
			std::cout << i+1 << " ";
			for (int8_t j = 0; j < 8; ++j)
			{
				bool isValid = false;
				for (auto p: validPositions)
				{
					if (p == std::make_pair(i, j))
					{
						isValid = true;
						break;
					}
				}
				if (isValid)
				{
					std::cout << "¡Á";
					continue;
				}
				switch (chessBoard.getColor(i * 8 + j))
				{
				case Color::NONE:  std::cout << "©à"; break;
				case Color::BLACK: std::cout << "¡ñ"; break;
				case Color::WHITE: std::cout << "¡ð"; break;
				default: std::cout << "?";
				}
			}
			std::cout << std::endl;
		}
	}

}
