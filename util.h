#pragma once

#define EMPTY 0

#define WHITE 1
#define BLACK 2

#define INVALID_POS -200000

#define ABS(N) ((N<0)?(-N):(N))

#define W_PAWN		5
#define W_ROOK		100
#define W_KNIGHT	150
#define W_BISHOP	200
#define W_QUEEN		600
#define W_KING		1000

#define B_PAWN		-5
#define B_ROOK		-100
#define B_KNIGHT	-150
#define B_BISHOP	-200
#define B_QUEEN		-600
#define B_KING		-1000

#define PAWN_INDEX		0
#define ROOK_INDEX		1
#define KNIGHT_INDEX		2
#define BISHOP_INDEX		3
#define QUEEN_INDEX		4
#define KING_INDEX		5

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

static int get_index(int piece_value)
{
    switch (piece_value)
    {
    case   B_PAWN:case   W_PAWN: return PAWN_INDEX;
    case   B_ROOK:case   W_ROOK: return ROOK_INDEX;
    case B_KNIGHT:case W_KNIGHT: return KNIGHT_INDEX;
    case B_BISHOP:case W_BISHOP: return BISHOP_INDEX;
    case  B_QUEEN:case  W_QUEEN: return QUEEN_INDEX;
    case   B_KING:case   W_KING: return KING_INDEX;

   
    }
    return INVALID_POS;
}
