#pragma once

#include "ChessBoard.h"
#include "ChessRenderer.h"
#include "window.h"
#include "cam2d.h"
#include "ChessDefines.h"
#include "Algorithm.h"

class ChessGame
{
public:

	void init();
	bool run();

private:
	Window* mWindow;
	Camera* mCamera;
	ChessRenderer* mRenderer;
	ChessBoard* mChessBoard;
	Algorithm* mAI;

	unsigned short mTurn;
};

