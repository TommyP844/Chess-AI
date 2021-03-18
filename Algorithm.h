#pragma once

#include "Move.h"
#include "ChessBoard.h"
#include "ChessDefines.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <map>
#include <iomanip> // for std::setprecision

class Algorithm
{
public:

	Algorithm();
	~Algorithm();

	void init(ChessBoard* board);

	Move move();

private:
	// *****************
	// * AI Speed Data *
	// *****************
	int mPrunedNodes;
	int mSearchedNodes;
	int mZobristMove;
	

	const double millis = 1.0 / CLOCKS_PER_SEC;


	ChessBoard* mChessBoard;

	struct ZobristMove
	{
		int eval;
	};

	u64 mTranspositionNumbers[12][64];
	std::map<u64, ZobristMove> mTranspositionTable;
	std::map<u64, ZobristMove>::iterator iter;

	void applyZobristMove(const Move& m, u64& zobrist);
	void undoZobristMove(const Move& m, u64& zobrist);
	u64 boardToZobristKey(const Board& board);
	u64 random();
	int NegaMax(int depth, int alpha, int beta, Color color, u64 zobrist);
	int evaluate();
	std::vector<Move> getAllValidMoves(Color color);



};

