#include "ChessRenderer.h"
#include "ImGui/imgui.h"
#include <iostream>

ChessRenderer::ChessRenderer()
    :
    quadVAO(-1),
    VBO(-1),
    model_index(-1),
    projection_index(-1),
    image_index(-1),
    color_index(-1),
    tile_model_index(-1),
    tile_projection_index(-1),
    m_camera(nullptr),
    m_dark_tile({ 0.3f, 0.5f, 0.3f, 1.f }),
    m_light_tile({ 0.6f, 0.1f, 0.1f, 1.f }),
    m_highlighted_tile({ 0.2f, 0.1f, 0.7f, 0.6f }),
    mIsAnimating(false)
{    
}

ChessRenderer::~ChessRenderer()
{
    for (auto& t : m_texture_pieces)
        t.release();
}

void ChessRenderer::init(Camera* cam, ChessBoard* board)
{
    mBoard = board;
    m_camera = cam;
    m_texture_pieces[W_PAWN_INDEX].load("./res/Textures/PawnW.png");
    m_texture_pieces[W_ROOK_INDEX].load("./res/Textures/RookW.png");
    m_texture_pieces[W_KNIGHT_INDEX].load("./res/Textures/KnightW.png");
    m_texture_pieces[W_BISHOP_INDEX].load("./res/Textures/BishopW.png");
    m_texture_pieces[W_QUEEN_INDEX].load("./res/Textures/QueenW.png");
    m_texture_pieces[W_KING_INDEX].load("./res/Textures/KingW.png");
    m_texture_pieces[B_PAWN_INDEX].load("./res/Textures/PawnB.png");
    m_texture_pieces[B_ROOK_INDEX].load("./res/Textures/RookB.png");
    m_texture_pieces[B_KNIGHT_INDEX].load("./res/Textures/KnightB.png");
    m_texture_pieces[B_BISHOP_INDEX].load("./res/Textures/BishopB.png");
    m_texture_pieces[B_QUEEN_INDEX].load("./res/Textures/QueenB.png");
    m_texture_pieces[B_KING_INDEX].load("./res/Textures/KingB.png");
    m_texture_pieces[12].load("./res/Textures/a1.png");
    m_texture_pieces[13].load("./res/Textures/a2.png");

    shader.load("./res/Shaders/sprite.fs.txt", "./res/Shaders/sprite.vs.txt");
    shader.bind();
    model_index = shader.getUniformLocation("model");
    projection_index = shader.getUniformLocation("projection");
    image_index = shader.getUniformLocation("image");
    
    mat4 orth = m_camera->get_view_ortho_matrix();
    shader.setMat4(projection_index, orth);
    
    shader.unbind();
    tileShader.load("./res/Shaders/boardTile.fs.txt", "./res/Shaders/boardTile.vs.txt");
    tileShader.bind();
    color_index = tileShader.getUniformLocation("tileColor");
    tile_model_index = tileShader.getUniformLocation("model");
    tile_projection_index = tileShader.getUniformLocation("projection");
    tileShader.setMat4(tile_projection_index, orth);

    tileShader.unbind();

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glEnable(GL_BLEND);


    mBoardLabelFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("./res/Textures/Fonts/Roboto-Black.ttf", 18);

    mDuration = 1.f;
}

void ChessRenderer::animateMove(const Move& m)
{
    
    mIsAnimating = true;
    int begin = m.getFromTile();
    int end = m.getToTile();
    mBegin = { (m.getFromTile() % 8) * 60, (m.getFromTile() / 8) * 60 };
    mEnd = { (m.getToTile() % 8) * 60, (m.getToTile() / 8) * 60 };
    
    float dist = glm::length(mEnd - mBegin);
    
    mSprite = m.getMovedPiece() + (m.getMovedColor() == BLACK ? B_PAWN_INDEX : 0);

    mBoard->mBoard.PieceBoards[m.getMovedColor()][m.getMovedPiece()] &= ~(1ull << m.getToTile());
    if(m.getCapturedPiece() >= 0 && m.getCapturedPiece() <= 6)
        mBoard->mBoard.PieceBoards[m.getMovedColor() ^ 1][m.getCapturedPiece()] |= (1ull << m.getToTile());
    mBoard->updateWhiteAndBlackBoards();
    mAnimatedMove = m;
    glm::vec2 dir;
    
    mStartTime = clock();
    mDir = glm::normalize(mEnd - mBegin);
    
}

void ChessRenderer::highLightLastMove(const Move& m)
{
    mLastMove = m;
}

void ChessRenderer::stepAnimation()
{
    time_t now = clock();

    if ((now - mStartTime) * millis <= mDuration)
    {
        float progress = (((now - mStartTime) * millis) / mDuration);
        glm::vec2 off = progress * (mEnd - mBegin);
        glm::vec2 pos = mOffset + mBegin + off + 0.5f * glm::vec2(c_scale);
        //std::cout << mBegin.x << ", " << mBegin.y << std::endl;
        draw_sprite(m_texture_pieces[mSprite], pos, c_scale, 0.f);
        
        if ((mAnimatedMove.getCapturedPiece() >= 0 && mAnimatedMove.getCapturedPiece() <= 6) && progress >= 0.75)
            mBoard->mBoard.PieceBoards[mAnimatedMove.getMovedColor() ^ 1][mAnimatedMove.getCapturedPiece()] &= ~(1ull << mAnimatedMove.getToTile());
        mBoard->updateWhiteAndBlackBoards();
    }
    else
    {
        mIsAnimating = false;
        mBoard->mBoard.PieceBoards[mAnimatedMove.getMovedColor()][mAnimatedMove.getMovedPiece()] |= (1ull << mAnimatedMove.getToTile());
        if (mAnimatedMove.getCapturedPiece() >= 0 && mAnimatedMove.getCapturedPiece() <= 6)
            mBoard->mBoard.PieceBoards[mAnimatedMove.getMovedColor() ^ 1][mAnimatedMove.getCapturedPiece()] &= ~(1ull << mAnimatedMove.getToTile());
        draw_sprite(m_texture_pieces[mSprite], mOffset + glm::vec2(mAnimatedMove.getToTile() % 8, mAnimatedMove.getToTile() / 8) * c_scale + 0.5f * c_scale, c_scale, 0.f);
        mBoard->updateWhiteAndBlackBoards();
        highLightLastMove(mAnimatedMove);
    }
}

void ChessRenderer::render(Board board)
{
    
    int balance = 0;
    for (int i = 0; i < 8; i++) //render board
    {
        for (int j = 0; j < 8; j++)
        {   
            if((i + (balance++)) % 2 == 0)
                draw_tile(mOffset + c_scale * 0.5f + glm::vec2(i, j) * c_scale, c_scale, m_dark_tile);
            else
                draw_tile(mOffset + c_scale * 0.5f + glm::vec2(i, j) * c_scale, c_scale, m_light_tile);
        }
    }
    
    /*
    if (promotion != Point({ -1, -1 })) // draw pawn promotion selection
    {
        int x = promotion.x;
        int y = (promotion.y == WHITE ? 0 : 7);
        int off = (promotion.y == WHITE ? 0 : 6);
        draw_sprite(m_texture_pieces[W_ROOK_INDEX + off]  , offset + glm::vec2(0.25f * c_scale, 0.25f * c_scale) + glm::vec2((float)x * c_scale, (float)y * c_scale), glm::vec2(25.f), 0.f);
        draw_sprite(m_texture_pieces[W_KNIGHT_INDEX + off], offset + glm::vec2(0.75f * c_scale, 0.25f * c_scale) + glm::vec2((float)x * c_scale, (float)y * c_scale), glm::vec2(25.f), 0.f);
        draw_sprite(m_texture_pieces[W_BISHOP_INDEX + off], offset + glm::vec2(0.25f * c_scale, 0.75f * c_scale) + glm::vec2((float)x * c_scale, (float)y * c_scale), glm::vec2(25.f), 0.f);
        draw_sprite(m_texture_pieces[W_QUEEN_INDEX + off],  offset + glm::vec2(0.75f * c_scale, 0.75f * c_scale) + glm::vec2((float)x * c_scale, (float)y * c_scale), glm::vec2(25.f), 0.f);
    }
    */

    if (mLastMove.isValid())
    {
        draw_tile(mOffset + (glm::vec2(mLastMove.getFromTile() % 8, mLastMove.getFromTile() / 8) * c_scale) + 0.5f * c_scale, c_scale, glm::vec4(0.95, 0.98, 0.015, 0.7));
        draw_tile(mOffset + (glm::vec2(mLastMove.getToTile() % 8, mLastMove.getToTile() / 8) * c_scale) + 0.5f * c_scale, c_scale, glm::vec4(0.95, 0.98, 0.015, 0.7));
    }
  
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++) // render pieces on board
        {
            int i = 8 * y + x;
            u64 mask = 1ull << i;

            if(board.Attacks & mask)
                draw_tile(mOffset + glm::vec2(x, y) * c_scale + 0.5f * c_scale, c_scale, m_highlighted_tile);


            if (board.General[WHITE] & mask)
            {
                for (int j = 0; j < NUM_BOARDS; j++)
                    if (board.PieceBoards[WHITE][j] & mask)
                        draw_sprite(m_texture_pieces[j], mOffset + glm::vec2(x, y) * c_scale + 0.5f * c_scale, c_scale, 0.f);
            }
            else if (board.General[BLACK] & mask)
            {
                for (int j = 0; j < NUM_BOARDS; j++)
                    if (board.PieceBoards[BLACK][j] & mask)
                        draw_sprite(m_texture_pieces[j + B_PAWN_INDEX], mOffset + glm::vec2(x, y) * c_scale + 0.5f * c_scale, c_scale, 0.f);

            }
        }
    }

    /*
    int y = 130;
    int x = 130;

    for (ChessBoard::Piece p : mBoard->mWhiteCaptered)
    {
        int index = getTextureIndexFromPiece(p);
        draw_sprite(m_texture_pieces[index], glm::vec2(x, y), glm::vec2(c_scale), 0.f);
        y += 60;
        if (y >= 560)
        {
            y = 130;
            x += 60;
        }
    }

    
    y = 130;
    x = 790;
    //std::cout << mBoard->mWhiteCaptered.size() << std::endl;
    //std::cout << mBoard->mBlackCaptured.size() << std::endl;
    for (ChessBoard::Piece p : mBoard->mBlackCaptured)
    {
        int index = getTextureIndexFromPiece(p);
        draw_sprite(m_texture_pieces[index], glm::vec2(x, y), glm::vec2(c_scale), 0.f);
        y += 60;
        if (y >= 560)
        {
            y = 130;
            x += 60;
        }
    }

    */
    int LetterX = 250 + 30 - 6;
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(LetterX, 77), ImColor(0, 0, 0, 255), "A");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(LetterX + 60 * 1, 77), ImColor(0, 0, 0, 255), "B");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(LetterX + 60 * 2, 77), ImColor(0, 0, 0, 255), "C");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(LetterX + 60 * 3, 77), ImColor(0, 0, 0, 255), "D");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(LetterX + 60 * 4, 77), ImColor(0, 0, 0, 255), "E");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(LetterX + 60 * 5, 77), ImColor(0, 0, 0, 255), "F");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(LetterX + 60 * 6, 77), ImColor(0, 0, 0, 255), "G");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(LetterX + 60 * 7, 77), ImColor(0, 0, 0, 255), "H");

    LetterX = 250 + 30 - 6;
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(LetterX, 591), ImColor(0, 0, 0, 255), "A");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(LetterX + 60 * 1, 591), ImColor(0, 0, 0, 255), "B");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(LetterX + 60 * 2, 591), ImColor(0, 0, 0, 255), "C");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(LetterX + 60 * 3, 591), ImColor(0, 0, 0, 255), "D");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(LetterX + 60 * 4, 591), ImColor(0, 0, 0, 255), "E");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(LetterX + 60 * 5, 591), ImColor(0, 0, 0, 255), "F");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(LetterX + 60 * 6, 591), ImColor(0, 0, 0, 255), "G");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(LetterX + 60 * 7, 591), ImColor(0, 0, 0, 255), "H");
    int LetterY = 130 - 9;
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(230, LetterY + 0), ImColor(0, 0, 0, 255), "8");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(230, LetterY + 60 * 1), ImColor(0, 0, 0, 255), "7");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(230, LetterY + 60 * 2), ImColor(0, 0, 0, 255), "6");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(230, LetterY + 60 * 3), ImColor(0, 0, 0, 255), "5");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(230, LetterY + 60 * 4), ImColor(0, 0, 0, 255), "4");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(230, LetterY + 60 * 5), ImColor(0, 0, 0, 255), "3");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(230, LetterY + 60 * 6), ImColor(0, 0, 0, 255), "2");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(230, LetterY + 60 * 7), ImColor(0, 0, 0, 255), "1");

    LetterY = 130 - 9;
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(741, LetterY + 0), ImColor(0, 0, 0, 255), "8");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(741, LetterY + 60 * 1), ImColor(0, 0, 0, 255), "7");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(741, LetterY + 60 * 2), ImColor(0, 0, 0, 255), "6");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(741, LetterY + 60 * 3), ImColor(0, 0, 0, 255), "5");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(741, LetterY + 60 * 4), ImColor(0, 0, 0, 255), "4");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(741, LetterY + 60 * 5), ImColor(0, 0, 0, 255), "3");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(741, LetterY + 60 * 6), ImColor(0, 0, 0, 255), "2");
    ImGui::GetOverlayDrawList()->AddText(mBoardLabelFont, 18, ImVec2(741, LetterY + 60 * 7), ImColor(0, 0, 0, 255), "1");
    
}

void ChessRenderer::DrawPieceAtPosition(glm::ivec2 pos, Piece p)
{
    draw_sprite(m_texture_pieces[p.piece + (p.color == BLACK ? B_PAWN_INDEX : 0)], pos, c_scale, 0.f);
}

void ChessRenderer::draw_sprite(Texture2D& texture, glm::vec2 position,
    glm::vec2 size, float rotate)
{
    // prepare transformations
    shader.bind();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));

    //model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));
    //model = glm::mat4(1.f);
    shader.setMat4(model_index, model);

    shader.setInt(image_index, 0);
    texture.bind();

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    shader.unbind();
}

void ChessRenderer::draw_tile(glm::vec2 position, glm::vec2 size, glm::vec4 color)
{
    // prepare transformations
    tileShader.bind();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));

    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));
    //model = glm::mat4(1.f);
    tileShader.setMat4(tile_model_index, model);
    tileShader.setVec4(color_index, color);


    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    tileShader.unbind();
}
