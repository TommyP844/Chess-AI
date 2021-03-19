#include "ChessBoard.h"

const u64 notAFile = 0xfefefefefefefefe; // ~0x0101010101010101
const u64 notHFile = 0x7f7f7f7f7f7f7f7f; // ~0x8080808080808080

u64 eastOne(u64 b) { return (b << 1) & notAFile; }
u64 noEaOne(u64 b) { return (b << 9) & notAFile; }
u64 soEaOne(u64 b) { return (b >> 7) & notAFile; }
u64 westOne(u64 b) { return (b >> 1) & notHFile; }
u64 soWeOne(u64 b) { return (b >> 9) & notHFile; }
u64 noWeOne(u64 b) { return (b << 7) & notHFile; }

ChessBoard::ChessBoard()
	:
	pawn_attacks(),
	pawn_moves(),
	bishop_attacks(),
	rook_attacks(),
	mTurn()
{
	
}

ChessBoard::~ChessBoard()
{
}

void ChessBoard::init()
{

	std::ifstream file("Rook_Slider_Attacks.bin", std::ios::binary | std::ios::in);

	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 4096; j++)
		{
			file >> rook_attacks[i][j];
		}
	}

	file.close();

	file.open("Bishop_Slider_attacks.bin", std::ios::binary | std::ios::in);

	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 512; j++)
		{
			file >> bishop_attacks[i][j];
		}
	}

	file.close();

	file.open("Knight.bin", std::ios::binary | std::ios::in);

	for (int i = 0; i < 64; i++)
	{
		file >> knight_attacks[i];
	}

	file.close();

	file.open("Pawn.bin", std::ios::binary | std::ios::in);

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			file >> pawn_attacks[i][j];
		}
	}

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			file >> pawn_moves[i][j];
		}
	}

	file.close();

	reset();
}

void ChessBoard::reset()
{
	mTurn = WHITE;

	mBoard.PieceBoards[WHITE][PAWN] = set_bit(a2) | set_bit(b2) | set_bit(c2) | set_bit(d2) | set_bit(e2) | set_bit(f2) | set_bit(g2) | set_bit(h2);
	mBoard.PieceBoards[WHITE][ROOK] = set_bit(a1) | set_bit(h1);
	mBoard.PieceBoards[WHITE][KNIGHT] = set_bit(b1) | set_bit(g1);
	mBoard.PieceBoards[WHITE][BISHOP] = set_bit(c1) | set_bit(f1);
	mBoard.PieceBoards[WHITE][QUEEN] = set_bit(d1);
	mBoard.PieceBoards[WHITE][KING] = set_bit(e1);

	mBoard.PieceBoards[BLACK][PAWN] = set_bit(a7) | set_bit(b7) | set_bit(c7) | set_bit(d7) | set_bit(e7) | set_bit(f7) | set_bit(g7) | set_bit(h7);
	mBoard.PieceBoards[BLACK][ROOK] = set_bit(a8) | set_bit(h8);
	mBoard.PieceBoards[BLACK][KNIGHT] = set_bit(b8) | set_bit(g8);
	mBoard.PieceBoards[BLACK][BISHOP] = set_bit(c8) | set_bit(f8);
	mBoard.PieceBoards[BLACK][QUEEN] = set_bit(d8);
	mBoard.PieceBoards[BLACK][KING] = set_bit(e8);
		
	mBoard.Attacks = 0ull;
	mPromotionSelection = false;
	updateWhiteAndBlackBoards();

	mPreviousMoves.clear();
	mPreviousMoves.push_back(Move());

}

Piece ChessBoard::getPieceAtPosition(int index)
{
	Piece p;
	
	p.color = -1;
	p.piece = -1;

	u64 mask = 1ull << index;
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < NUM_BOARDS; j++)
		{
			if (mBoard.PieceBoards[WHITE][j] & mask)
			{
				p.color = WHITE;
				p.piece = j;
				return p;
			}
			if (mBoard.PieceBoards[BLACK][j] & mask)
			{
				p.color = BLACK;
				p.piece = j;
				return p;
			}
		}
	}
	return p;
}

bool ChessBoard::placePiece(int index, Piece p)
{
	bool success = false;
	u64 mask = 1ull << index;
	if (mask & mBoard.Attacks)
	{
	
		mBoard.PieceBoards[p.color][p.piece] |= mask;
		
		updateWhiteAndBlackBoards();
		
		success = true;
	}
	mBoard.Attacks = 0ull;
	return success;
}

void ChessBoard::setPiece(int index, Piece p)
{
	mBoard.PieceBoards[p.color][p.piece] |= 1ull << index;
	
	updateWhiteAndBlackBoards();
}

Move ChessBoard::generateMove(int from, int to, Piece piece)
{
	Move m; 
	m.setFromTile(from);
	m.setToTile(to);
	m.setMovedPiece(piece.piece);
	u64 mask = 1ull << to;
	int captured = -1; 
	for (int i = 0; i < NUM_BOARDS; i++)
		if (mBoard.PieceBoards[piece.color ^ 1][i] & mask)
			captured = i;
	
	m.setCapturedPiece(captured);
	m.setMovedColor(piece.color);


	if (mPreviousMoves.empty())
	{
		m.setWhiteKingCastleKingSide(true);
		m.setWhiteKingCastleQueenSide(true);
		m.setBlackKingCastleKingSide(true);
		m.setBlackKingCastleQueenSide(true);
	}
	else
	{
		Move last = mPreviousMoves.back();
		m.setWhiteKingCastleKingSide(last.getWhiteKingCastleKingSide());
		m.setWhiteKingCastleQueenSide(last.getWhiteKingCastleQueenSide());
		m.setBlackKingCastleKingSide(last.getBlackKingCastleKingSide());
		m.setBlackKingCastleQueenSide(last.getBlackKingCastleQueenSide());
	}


	if (piece.piece == ROOK)
	{
		if (from == 0)
			m.setBlackKingCastleQueenSide(false);
		if (from == 7)
			m.setBlackKingCastleKingSide(false);
		if (from == 56)
			m.setWhiteKingCastleQueenSide(false);
		if (from == 63)
			m.setWhiteKingCastleKingSide(false);
	}
	else if (piece.piece == KING)
	{
		if (piece.color == WHITE)
		{
			m.setWhiteKingCastleKingSide(false);
			m.setWhiteKingCastleQueenSide(false);
		}
		else
		{
			m.setBlackKingCastleKingSide(false);
			m.setBlackKingCastleQueenSide(false);
		}
	}

	return m;
}

void ChessBoard::applyMove(const Move& move)
{
	int color = move.getMovedColor();
	int moved = move.getMovedPiece();
	int captured = move.getCapturedPiece();
	int from = move.getFromTile();
	int to = move.getToTile();
		 
	mBoard.PieceBoards[color][moved] &= ~(1ull << move.getFromTile());
	mBoard.PieceBoards[color][moved] |= (1ull << move.getToTile());
	if (captured <= 6 && captured >= 0)
	{
		mBoard.PieceBoards[color ^ 1][move.getCapturedPiece()] &= ~(1ull << move.getToTile());
	}

	if (moved == KING)
	{
		if (std::abs(from - to) == 2) // we are castling
		{
			if (to > from) // king side
			{
				mBoard.PieceBoards[color][ROOK] &= ~(1ull << (to + 1));
				mBoard.PieceBoards[color][ROOK] |= (1ull << (to - 1));
			}
			else // queen side
			{
				mBoard.PieceBoards[color][ROOK] &= ~(1ull << (to - 2));
				mBoard.PieceBoards[color][ROOK] |= (1ull << (to + 1));
			}
		}
	}

	mTurn ^= 1;
	mPreviousMoves.push_back(move);
	updateWhiteAndBlackBoards();
}

void ChessBoard::undoMove(const Move& move)
{
	int color = move.getMovedColor();
	int moved = move.getMovedPiece();
	int captured = move.getCapturedPiece();
	int from = move.getFromTile();
	int to = move.getToTile();

	mBoard.PieceBoards[color][moved] &= ~(1ull << to);
	mBoard.PieceBoards[color][moved] |= (1ull << from);

	if (captured >= 0 && captured <= 6)
	{
		mBoard.PieceBoards[color ^ 1][captured] |= (1ull << to);
	}

	if (moved == KING)
	{
		if (std::abs(from - to) == 2) // we are castling
		{
			if (to > from) // king side
			{
				mBoard.PieceBoards[color][ROOK] &= ~(1ull << (to - 1));
				mBoard.PieceBoards[color][ROOK] |= (1ull << (to + 1));
			}
			else // queen side
			{
				mBoard.PieceBoards[color][ROOK] &= ~(1ull << (to + 1));
				mBoard.PieceBoards[color][ROOK] |= (1ull << (to - 2));
			}
		}
	}

	mTurn ^= 1;
	mPreviousMoves.pop_back();

	updateWhiteAndBlackBoards();
}

void ChessBoard::clearPiece(int index, Piece p)
{
	mBoard.PieceBoards[p.color][p.piece] &= ~(1ull << index);
	updateWhiteAndBlackBoards();
}

bool ChessBoard::update(int index)
{
	mBoard.Attacks = 0ull;

	if (index < 0 || index > 63)
		return false;

	Piece piece = getPieceAtPosition(index);

	if (piece.color != mTurn || piece.piece == -1)
		return false;

	// check that we have selected the right color
	if (!(1ull << index & mBoard.PieceBoards[piece.color][piece.piece]))
		return false;

	u64 attacks = getLegalMoves(mTurn, piece.piece, index);

	mBoard.Attacks = attacks;

	return true;
}

void ChessBoard::updateWhiteAndBlackBoards()
{
	mBoard.General[WHITE] = 0ull;
	mBoard.General[BLACK] = 0ull;

	for (int i = 0; i < NUM_BOARDS; i++)
	{
		mBoard.General[WHITE] |= mBoard.PieceBoards[WHITE][i];
		mBoard.General[BLACK] |= mBoard.PieceBoards[BLACK][i];
	}

	mBoard.All = mBoard.General[BLACK] | mBoard.General[WHITE];

}

int ChessBoard::numBitsSet(u64 word)
{
	int bits = 0;

	while (word)
	{
		bits++;
		word &= word - 1;
	}

	return bits;
}

u64 ChessBoard::getSuedoLegalMoves(int board_index, int piece_index, u64 blockerMask)
{
	u64 attacks = 0ull;

	switch (board_index)
	{
	case PAWN:
		attacks |= get_pawn_attacks(piece_index);
		break;
	case ROOK:
		attacks = get_rook_attacks(piece_index, blockerMask);
		break;
	case KNIGHT:
		attacks = knight_attacks[piece_index];
		break;
	case BISHOP:
		attacks = get_bishop_attacks(piece_index, blockerMask);
		break;
	case QUEEN:
		attacks = get_rook_attacks(piece_index, blockerMask);
		attacks |= get_bishop_attacks(piece_index, blockerMask);
		break;
	case KING:
		attacks = king_attacks[piece_index] & ~king_attacks[get_ls1b_index(mBoard.PieceBoards[mTurn ^ 1][KING])];
		if (!mPreviousMoves.empty())
		{
			Move last = mPreviousMoves.back();
			u64 pos = 1ull << piece_index;
			u64 left = (pos >> 1) | (pos >> 2) | (pos >> 3);
			u64 right = (pos << 1) | (pos << 2);

			if ((left & ~mBoard.All) == left) // castle left
			{
				if (mTurn == WHITE)
				{
					if (last.getWhiteKingCastleQueenSide())
					{
						attacks |= pos >> 2;
					}
				}
				else
				{
					if (last.getBlackKingCastleQueenSide())
					{
						attacks |= pos >> 2;
					}
				}
			}
			if ((right & ~mBoard.All) == right) // castle right
			{
				if (mTurn == WHITE)
				{
					if (last.getWhiteKingCastleKingSide())
					{
						attacks |= pos << 2;
					}
				}
				else
				{
					if (last.getBlackKingCastleKingSide())
					{
						attacks |= pos << 2;
					}
				}
			}
		}
		break;
	}

	attacks &= ~mBoard.General[mTurn];

	return attacks;
}

u64 ChessBoard::getLegalMoves(Color color, int board_index, int piece_index)
{
	// check that we have selected the right color
	if (!((1ull << piece_index) & mBoard.PieceBoards[color][board_index]))
		return false;
	
	u64 attacks = getSuedoLegalMoves(board_index, piece_index, mBoard.All);

	u64 copy = attacks;

	while (copy)
	{
		int index = get_ls1b_index(copy);
		u64 flag = ~(1ull << index);
		copy &= flag;
		Piece p;
		p.color = color;
		p.piece = board_index;
		Move m = generateMove(piece_index, index, p);
		applyMove(m);
		if (isKingInCheck(color))
			attacks &= flag;
		undoMove(m);
	}
	return attacks;
}

bool ChessBoard::positionUnderAttack(int index, Color color, u64 blockerMask)
{
	u64 attacks = 0ull;

	// NUM_BOARDS - 1 so we dont include king attacks
	for (int i = 1; i < NUM_BOARDS - 1; i++)
	{
		u64 board = mBoard.PieceBoards[color][i];

		while (board)
		{
			int index = get_ls1b_index(board);
			board &= ~(1ull << index);

			attacks |= getSuedoLegalMoves(i, index, blockerMask);
		}
	}

	if (color == BLACK)
	{
		if (mBoard.PieceBoards[BLACK][PAWN] & soEaOne(1ull << index))
			attacks |= 1ull << index;
		if (mBoard.PieceBoards[BLACK][PAWN] & soWeOne(1ull << index))
			attacks |= 1ull << index;
	}
	else
	{
		if (mBoard.PieceBoards[WHITE][PAWN] & noEaOne(1ull << index))
			attacks |= 1ull << index;
		if (mBoard.PieceBoards[WHITE][PAWN] & noWeOne(1ull << index))
			attacks |= 1ull << index;
	}

	return attacks & (1ull << index);
}

u64 ChessBoard::isKingInCheck(Color color)
{
	u64 attackers = 0ull;

	u64 king = mBoard.PieceBoards[color][KING];

	if (king == 0ull)
		return 0ull;

	int king_index = get_ls1b_index(king);

	int opp = color ^ 1;

	u64 rooks = get_rook_attacks(king_index, mBoard.All);
	attackers |= rooks & (mBoard.PieceBoards[opp][ROOK] | mBoard.PieceBoards[opp][QUEEN]);

	u64 bishop = get_bishop_attacks(king_index, mBoard.General[WHITE] | mBoard.General[BLACK]);
	attackers |= bishop & (mBoard.PieceBoards[opp][BISHOP] | mBoard.PieceBoards[opp][QUEEN]);

	attackers |= knight_attacks[king_index] & mBoard.PieceBoards[opp][KNIGHT];

	u64 king_pos = 1ull << king_index;

	// add pawns
	if (color == WHITE)
		attackers |= (soEaOne(king_pos) | soWeOne(king_pos)) & mBoard.PieceBoards[opp][PAWN];
	else
		attackers |= (noEaOne(king_pos) | noWeOne(king_pos)) & mBoard.PieceBoards[opp][PAWN];

	return attackers;
}

u64 ChessBoard::get_rook_attacks(int square, u64 mask)
{
	mask &= rook_masks[square];
	mask *= rook_magics[square];
	mask >>= 64 - rook_relevant_bits[square];

	// return rellevant attacks
	return rook_attacks[square][mask];
}

u64 ChessBoard::get_bishop_attacks(int square, u64 mask)
{
	mask &= bishop_masks[square];
	mask *= bishop_magics[square];
	mask >>= 64 - bishop_relevant_bits[square];

	// return rellevant attacks
	return bishop_attacks[square][mask];
}

u64 ChessBoard::get_pawn_attacks(int index)
{
	u64 enemyMask = mBoard.General[mTurn ^ 1];

	u64 attacks = 0ull;

	u64 pos = 1ull << index;
	u64 first = 255ull << (mTurn == WHITE ? 48 : 8);
	u64 move = 1ull << (mTurn == WHITE ? (index - 16) : (index + 16));

	u64 open = ~(mBoard.General[WHITE] | mBoard.General[BLACK]);

	attacks |= pawn_moves[mTurn][index] & open;
	if (attacks > 0ull
		&& (pos & first)
		&& (move & open))
		attacks |= move;
	attacks |= pawn_attacks[mTurn][index] & enemyMask;
	return attacks;
}

int ChessBoard::get_ls1b_index(u64 bitboard)
{
	// make sure bitboard is not empty
	if (bitboard != 0)
		// convert trailing zeros before LS1B to ones and count them
		return numBitsSet((bitboard & (-(long long)bitboard)) - 1);

	// otherwise
	else
		// return illegal index
		return -1;
}

 