#pragma once

#include <cstdint>
#include <utility>
#include <bitset>
#include <vector>

namespace loa{
	using POSITION = std::pair<int8_t, int8_t>;
	enum class Color : uint8_t { NONE, BLACK, WHITE };
	class MyChessBoard
	{
	public:
		MyChessBoard() : black_pieces(0x7e0000000000007e),
						white_pieces(0x0081818181818100),
			WhoseTurn(Color::BLACK), num_of_pieces(12, 12) {}
		MyChessBoard(uint64_t& black, uint64_t white, Color& color) :
					black_pieces(black), white_pieces(white), WhoseTurn(color)
		{
			num_of_pieces = std::make_pair(black_pieces.count(), white_pieces.count());
		}
		Color getWhoseTurn()
		{
			return WhoseTurn;
		}
		std::vector<POSITION> getFeasible(POSITION pos);
		bool update(const POSITION& pos, const POSITION& des);  // return whether a piece got eaten.
		Color checkWinner();

		Color getColor(const uint8_t pos)
		{
			if (black_pieces.test(pos))
				return Color::BLACK;
			else if (white_pieces.test(pos))
				return Color::WHITE;
			return Color::NONE;
		}

	private:
		std::bitset<64> black_pieces, white_pieces;
		Color WhoseTurn;
		std::pair<uint8_t, uint8_t> num_of_pieces;   // first = black, second = white
		bool getFeasibleDir(const POSITION pos, const POSITION dir, POSITION& des);
		uint8_t getNumofPiecesDir(const POSITION pos, const POSITION dir);
		void dfs(std::bitset<64>& board, int pos);
	};
}
