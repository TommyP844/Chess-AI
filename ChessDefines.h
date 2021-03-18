#pragma once


const unsigned int BLACK = 0;
const unsigned int WHITE = 1;


// Pieces
const int PAWN = 0;
const int ROOK = 1;
const int KNIGHT = 2;
const int BISHOP = 3;
const int QUEEN = 4;
const int KING = 5;
const int NUM_BOARDS = 6;
const int EMPTY = -1;
typedef unsigned __int64 u64;
typedef int Color;



struct Board
{
	// can be indexed with WHITE or BLACK
	u64 PieceBoards[2][NUM_BOARDS] = { 0ull };
	u64 General[2] = { 0ull };
	u64 All = 0ull;
	u64 Attacks = 0ull;
};

struct Piece
{
	int piece;
	int color;
};

enum Tiles {
	a8, b8, c8, d8, e8, f8, g8, h8,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a1, b1, c1, d1, e1, f1, g1, h1
};

// Texture index
#define W_PAWN_INDEX		0
#define W_ROOK_INDEX		1
#define W_KNIGHT_INDEX		2
#define W_BISHOP_INDEX		3
#define W_QUEEN_INDEX		4
#define W_KING_INDEX		5

#define B_PAWN_INDEX		6
#define B_ROOK_INDEX		7
#define B_KNIGHT_INDEX		8
#define B_BISHOP_INDEX		9
#define B_QUEEN_INDEX		10
#define B_KING_INDEX		11


/*
* 
*	Code used to generate the move tables
*	No need to run everytime just calculate once save and go from there
* 
*/


/*
* 
* #define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define set_bit(bitboard, square) (bitboard |= (1ULL << square))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? (bitboard ^= (1ULL << square)) : 0)


void ChessBoard::init_slider_attacks(bool isBishop)
{
	for (int square = 0; square < 64; square++)
	{
		// init bishop & rook masks
		bishop_masks[square] = mask_bishop_attacks(square);
		rook_masks[square] = mask_rook_attacks(square);

		// init current mask
		u64 mask = isBishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);

		// count attack mask bits
		int bit_count = count_bits(mask);

		// occupancy variations count
		int occupancy_variations = 1 << bit_count;

		// loop over occupancy variations
		for (int count = 0; count < occupancy_variations; count++)
		{
			// bishop
			if (isBishop)
			{
				// init occupancies, magic index & attacks
				u64 occupancy = set_occupancy(count, bit_count, mask);
				u64 magic_index = occupancy * bishop_magics[square] >> 64 - bishop_relevant_bits[square];
				bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
			}

			// rook
			else
			{
				// init occupancies, magic index & attacks
				u64 occupancy = set_occupancy(count, bit_count, mask);
				u64 magic_index = occupancy * rook_magics[square] >> 64 - rook_relevant_bits[square];
				rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);
			}
		}
	}
}

u64 ChessBoard::set_occupancy(int index, int bits_in_mask, u64 attack_mask)
{
	// occupancy map
	u64 occupancy = 0ULL;

	// loop over the range of bits within attack mask
	for (int count = 0; count < bits_in_mask; count++)
	{
		// get LS1B index of attacks mask
		int square = get_ls1b_index(attack_mask);

		// pop LS1B in attack map
		pop_bit(attack_mask, square);

		// make sure occupancy is on board
		if (index & (1 << count))
			// populate occupancy map
			occupancy |= (1ULL << square);
	}

	// return occupancy map
	return occupancy;
}

int ChessBoard::get_ls1b_index(u64 bitboard)
{
	// make sure bitboard is not empty
	if (bitboard != 0)
		// convert trailing zeros before LS1B to ones and count them
		return count_bits((bitboard & (u64)-((long long)bitboard)) - 1);

	// otherwise
	else
		// return illegal index
		return -1;
}

int ChessBoard::count_bits(u64 bitboard)
{
	// bit count
	int count = 0;

	// pop bits untill bitboard is empty
	while (bitboard)
	{
		// increment count
		count++;

		// consecutively reset least significant 1st bit
		bitboard &= bitboard - 1;
	}

	// return bit count
	return count;
}

u64 ChessBoard::rook_attacks_on_the_fly(int square, u64 block)
{
	// attacks bitboard
	u64 attacks = 0ULL;

	// init files & ranks
	int f, r;

	// init target files & ranks
	int tr = square / 8;
	int tf = square % 8;

	// generate attacks
	for (r = tr + 1; r <= 7; r++)
	{
		attacks |= (1ULL << (r * 8 + tf));
		if (block & (1ULL << (r * 8 + tf))) break;
	}

	for (r = tr - 1; r >= 0; r--)
	{
		attacks |= (1ULL << (r * 8 + tf));
		if (block & (1ULL << (r * 8 + tf))) break;
	}

	for (f = tf + 1; f <= 7; f++)
	{
		attacks |= (1ULL << (tr * 8 + f));
		if (block & (1ULL << (tr * 8 + f))) break;
	}

	for (f = tf - 1; f >= 0; f--)
	{
		attacks |= (1ULL << (tr * 8 + f));
		if (block & (1ULL << (tr * 8 + f))) break;
	}

	// return attack map for bishop on a given square
	return attacks;
}

u64 ChessBoard::bishop_attacks_on_the_fly(int square, u64 block)
{
	// attack bitboard
	u64 attacks = 0ULL;

	// init files & ranks
	int f, r;

	// init target files & ranks
	int tr = square / 8;
	int tf = square % 8;

	// generate attacks
	for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
	{
		attacks |= (1ULL << (r * 8 + f));
		if (block & (1ULL << (r * 8 + f))) break;
	}

	for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
	{
		attacks |= (1ULL << (r * 8 + f));
		if (block & (1ULL << (r * 8 + f))) break;
	}

	for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
	{
		attacks |= (1ULL << (r * 8 + f));
		if (block & (1ULL << (r * 8 + f))) break;
	}

	for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
	{
		attacks |= (1ULL << (r * 8 + f));
		if (block & (1ULL << (r * 8 + f))) break;
	}

	// return attack map for bishop on a given square
	return attacks;
}

u64 ChessBoard::mask_bishop_attacks(int square)
{
	// attack bitboard
	u64 attacks = 0ULL;

	// init files & ranks
	int f, r;

	// init target files & ranks
	int tr = square / 8;
	int tf = square % 8;

	// generate attacks
	for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
	for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
	for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
	for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));

	// return attack map for bishop on a given square
	return attacks;
}

// mask rook attacks
u64 ChessBoard::mask_rook_attacks(int square)
{
	// attacks bitboard
	u64 attacks = 0ULL;

	// init files & ranks
	int f, r;

	// init target files & ranks
	int tr = square / 8;
	int tf = square % 8;

	// generate attacks
	for (r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf));
	for (r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
	for (f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
	for (f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));

	// return attack map for bishop on a given square
	return attacks;
}


*/