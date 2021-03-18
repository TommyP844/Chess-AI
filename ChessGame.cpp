#include "ChessGame.h"

void ChessGame::init()
{
	mWindow = new Window("Chess v1.0", 1080, 720);

	mCamera = new Camera;
	mCamera->update_window_dimensions(1080, 720);
	mCamera->update_camera_position(0, 0, 2);


	mChessBoard = new ChessBoard;
	mChessBoard->init();

	mRenderer = new ChessRenderer;
	mRenderer->init(mCamera, mChessBoard);

	mAI = new Algorithm;
	mAI->init(mChessBoard);

	
}

bool ChessGame::run()
{
	Piece selected;
	bool lastFrameLeftPressed = false;
	bool pieceSelected = false;
	int old_index = 0;
	glm::ivec2 oldMouse = { -1, -1 };

	// ai variables
	int depth = 5;
	bool orderMoves = true;

	bool pvp = true;

	while (mWindow->is_open())
	{
		mWindow->startFrame();
		bool mousePressed = mWindow->GetLeftIsPressed();

		glm::ivec2 m = (mWindow->GetMousePos() - (glm::ivec2)mRenderer->getBoardOffset()) / 60;
		
		int index = m.y * 8 + m.x;

		ImGui::Begin("Debug");
		ImGui::Text("Mouse X: %i", mWindow->GetMousePos().x);
		ImGui::Text("Mouse Y: %i", mWindow->GetMousePos().y);
		ImGui::Text("Board X: %i", (mWindow->GetMousePos().x - (glm::ivec2)mRenderer->getBoardOffset()) / 60);
		ImGui::Text("Board Y: %i", (mWindow->GetMousePos().y - (glm::ivec2)mRenderer->getBoardOffset()) / 60);
		ImGui::Text("Board Index: %i", index);
		ImGui::Text("White King Check: %s", (mChessBoard->isKingInCheck(WHITE) > 0ull ? "Yes" : "No"));
		ImGui::Text("Black King Check: %s", (mChessBoard->isKingInCheck(BLACK) > 0ull ? "Yes" : "No"));

		ImGui::Checkbox("PVP", &pvp);
		ImGui::End();

		if (mRenderer->isanimating() == false)
		{
			if (mChessBoard->getTurn() == WHITE)
			{
				if (mousePressed && !lastFrameLeftPressed) // select
				{
					selected = mChessBoard->getPieceAtPosition(index);
					if (selected.color == mChessBoard->getTurn() && selected.piece != -1)
					{
						old_index = index;
						mChessBoard->clearPiece(index, selected);
						pieceSelected = true;
					}
				}
				else if (!mousePressed && pieceSelected && lastFrameLeftPressed) // place
				{
					Move move = mChessBoard->generateMove(old_index, index, selected);
					// check if we can place the piece on a valid tile
					// if not put the selected piece back
					// otherwise apply the move
					if (!mChessBoard->placePiece(index, selected))
					{
						mChessBoard->setPiece(old_index, selected);
					}
					else
					{
						mChessBoard->applyMove(move);
						mRenderer->highLightLastMove(move);
					}
					pieceSelected = false;
				}
				else if ((m.x >= 0 && m.x < 8 && m.y >= 0 && m.y < 8) && !mousePressed && !pieceSelected)
				{
					mChessBoard->update(m.y * 8 + m.x);
				}
			}
			else
			{
				Move move = mAI->move();
				mRenderer->animateMove(move);
			}
		}

		mRenderer->render(mChessBoard->getBoard());
		if (pieceSelected)
			mRenderer->DrawPieceAtPosition(mWindow->GetMousePos(), selected);

		if (mRenderer->isanimating())
			mRenderer->stepAnimation();
		
		lastFrameLeftPressed = mousePressed;
		oldMouse = mWindow->GetMousePos();
		mWindow->endFrame();
	}
	return true;
}
