

#include "ChessGame.h"
#include <fstream>
#include <ostream>
#include <algorithm>
#include <iostream>
#include <thread>

extern "C" {
	_declspec(dllexport) int NvOptimusEnablement = 1;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

//typedef unsigned long long u64;

void print(u64 board);

u64 notAFile = 0xfefefefefefefefeL;
u64 notHFile = 0x7f7f7f7f7f7f7f7fL;
u64 notABFile = 0xfcfcfcfcfcfcfcfcL;
u64 notGHFile = 0x3f3f3f3f3f3f3f3fL;

u64 no(u64 b)   { return (b >> 8); }
u64 noEa(u64 b) { return (b >> 9) & notHFile; }
u64 Ee(u64 b)   { return (b >> 1) & notHFile; }
u64 soEa(u64 b) { return (b << 9) & notAFile; }
u64 so(u64 b)   { return (b << 8); }
u64 soWe(u64 b) { return (b << 7) & notHFile; }
u64 we(u64 b)   { return (b << 1) & notAFile; }
u64 noWe(u64 b) { return (b >> 7) & notAFile; }

u64 eastOne1(u64 b) { return (b << 1) & notAFile; }
u64 noEaOne1(u64 b) { return (b << 9) & notAFile; }
u64 soEaOne1(u64 b) { return (b >> 7) & notAFile; }
u64 westOne1(u64 b) { return (b >> 1) & notHFile; }
u64 soWeOne1(u64 b) { return (b >> 9) & notHFile; }
u64 noWeOne1(u64 b) { return (b << 7) & notHFile; }

int main(int argc, char** argv)
{
	ChessGame* game = new ChessGame;
	game->init();
	game->run();

	return 0;
}


void print(u64 board)
{
	for (int i = 0; i < 64; i++)
	{
		std::cout << (board & 1ull << i ? "1" : ".");
		
		if ((i + 1) % 8 == 0)
			std::cout << "\n";
	}
	std::cout << "\n";
}