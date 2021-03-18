#pragma once


#include <vector>
#include <glm/glm.hpp>
#include "shader.h"
#include "cam2d.h"
#include "Texture2D.h"
#include "ChessDefines.h"
#include "ChessBoard.h"
#include <chrono>
#include "ImGui/imgui.h"


class ChessRenderer
{
public:
    ChessRenderer();
    ~ChessRenderer();

    void init(Camera* cam, ChessBoard* board);

    void animateMove(const Move& m);
    void highLightLastMove(const Move& m);
    void stepAnimation();
    bool isanimating() const { return mIsAnimating; }
  
    void render(Board board);
    void DrawPieceAtPosition(glm::ivec2 pos, Piece piece);

    glm::vec2 getBoardOffset() const { return mOffset; }

private:
    // Begin animation variables
    Move mAnimatedMove;
    const float pps = 0.01f;
    bool mIsAnimating;
    time_t mStartTime;
    float mDuration;
    int mSprite;
    glm::vec2 mBegin;
    glm::vec2 mEnd;
    int mEndIndex;
    const float millis = 1.f / 1000.f;
    glm::vec2 mDir;
    // End animation variables

    Move mLastMove;

    void draw_sprite(Texture2D& texture, glm::vec2 position,
        glm::vec2 size, float rotate);
    void draw_tile(glm::vec2 position, glm::vec2 size, glm::vec4 color);

    const glm::vec2 c_scale = { 60.f, 60.f };

    glm::vec2 mOffset = { 250.f, 100.f };

    ImFont* mBoardLabelFont;

    ChessBoard* mBoard;
    Camera* m_camera;
	Texture2D m_texture_pieces[14];
    Shader shader;
    Shader tileShader;

    glm::vec4 m_dark_tile;
    glm::vec4 m_light_tile;
    glm::vec4 m_highlighted_tile;

    unsigned int quadVAO;
    unsigned int VBO;
    int model_index;
    int projection_index;
    int image_index;

    int tile_model_index;
    int tile_projection_index;
    int color_index;

    const float vertices[24] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
};

