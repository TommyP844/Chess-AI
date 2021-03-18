#pragma once


class Move
{
public:
	Move() : move(0) {}

	Move(int from, int to, int moved, int captured)
		:
		move(0)
	{
		setFromTile(from);
		setToTile(to);
		setMovedPiece(moved);
		setCapturedPiece(captured);
	}


	inline void setFromTile(int from) { move |= (from) & 0x3f; }
	inline void setToTile(unsigned int to) { move |= (to << 6) & 0xfc0; }
	inline void setMovedPiece(unsigned int moved) { move |= (moved << 12) & 0x7000; }
	inline void setCapturedPiece(unsigned int captured) { move |= (captured << 15) & 0x38000; }
	inline void setMovedColor(unsigned int color) { if (color == 0) move &= ~0x40000; else move |= 0x40000; }
	inline void setEnPassant(unsigned int enpassant) { if (enpassant) move |= 0x80000; else move &= ~0x80000; }
	inline void setCastle(unsigned int castle) { (castle == 1 ? move |= 0x100000 : move &= ~0x100000); }

	inline int getFromTile() const { return (move & 0x3F); }
	inline int getToTile() const { return ((move & 0xFC0) >> 6); }
	inline int getMovedPiece() const { return (move & 0x7000) >> 12; }
	inline int getCapturedPiece() const { return (move & 0x38000) >> 15; }
	inline int getMovedColor() const { return ((move & 0x40000) >> 18); }
	inline int getEnPassant() const { return ((move >> 19) & 1); }
	inline int getCastle() const { return ((move & 0x100000) >> 20); }

	inline int getMoveWord() const { return move; }

private:

	//			binary move representation							Hex Mask
	//
	// 0000 0000 0000 0000 0000 0000 0011 1111  -  from				0x3F
	// 0000 0000 0000 0000 0000 1111 1100 0000  -  to				0xFC0
	// 0000 0000 0000 0000 0111 0000 0000 0000  -  moved piece		0x7000
	// 0000 0000 0000 0011 1000 0000 0000 0000  -  captured			0x38000
	// 0000 0000 0000 0100 0000 0000 0000 0000  -  move color		0x40000
	// 0000 0000 0000 1000 0000 0000 0000 0000  -  enPassant		0x80000
	// 0000 0000 1111 0000 0000 0000 0000 0000  -  castle rights 	0x100000
	// 0000 0000 1111 0000 0000 0000 0000 0000  -  promotion piece 	0x100000

	unsigned int move;
};