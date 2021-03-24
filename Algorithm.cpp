#include "Algorithm.h"


int clamp(int a, int low, int high) { if (a > high) return high; if (a < low) return low; return a; }


Algorithm::Algorithm()
{
}

Algorithm::~Algorithm()
{
}

void Algorithm::init(ChessBoard* board)
{
    mChessBoard = board;


    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            mTranspositionNumbers[i][j] = random();
            //std::cout << mTranspositionNumbers[i][j] << std::endl;
        }
    }

}

Move Algorithm::move()
{
    long start = clock();
    Move bestMove;
    int bestMoveScore = INT_MIN;

    int depth = 3;
    mPrunedNodes = 0;
    mSearchedNodes = 0;
    mZobristMove = 0;

    float timeToMove = 1.0f;
    u64 zobristBoard = boardToZobristKey(mChessBoard->mBoard);
    
    mTranspositionTable.clear();

    while ((clock() - start) * millis < timeToMove)
    {
        mTranspositionTable.clear();
        depth++;
        std::vector<Move> moves = getAllValidMoves(BLACK);
        for (int i = 0; i < moves.size(); i++)
        {
            Move move = moves[i];
            mChessBoard->applyMove(move);
            applyZobristMove(move, zobristBoard);
            int eval = -NegaMax(depth, INT_MIN, INT_MAX, WHITE, zobristBoard);
            undoZobristMove(move, zobristBoard);
            mChessBoard->undoMove(move);
            if (eval > bestMoveScore)
            {
                bestMoveScore = eval;
                bestMove = move;
            }
        }
    }


    std::cout << "---------------" << std::endl;
    std::cout << "-      AI     -" << std::endl;
    std::cout << "---------------" << std::endl;
    std::cout << "Depth: " << depth << std::endl;
    std::cout << "Nodes Pruned: " << mPrunedNodes << std::endl;
    std::cout << "Searched Nodes: " << mSearchedNodes << std::endl;
    std::cout << "Zobrist Nodes: " << mZobristMove << std::endl;
    std::cout << "Hash Table Size: " << mTranspositionTable.size() << std::endl;
    double nps = ((mSearchedNodes + mZobristMove) / ((clock() - start) * millis));
    std::string str = "";
    if (nps > 1e9)
    {
        nps /= 1e9; 
        str = " Bn/s";
    }
    else if (nps > 1e6)
    {
        nps /= 1e6;
        str = " Mn/s";
    }
    else if (nps > 1e3)
    {
        nps /= 1e3;
        str = " Kn/s";
    }
    std::cout << "Nodes / sec: " << std::setprecision(4) << nps << str << std::endl;
    std::cout << "Move Score: " << bestMoveScore << std::endl;
    std::cout << "Time: " << std::setprecision(4) << (clock() - start) * millis << "s" << std::endl;
    std::cout << "---------------" << std::endl;

    mChessBoard->applyMove(bestMove);
    mChessBoard->mTurn = WHITE; // switch turn back to white

    return bestMove;
}

void Algorithm::applyZobristMove(const Move& m, u64& zobrist)
{
    zobrist ^= mTranspositionNumbers[m.getMovedPiece() + (m.getMovedColor() == WHITE ? 0 : 6)][m.getFromTile()];
    zobrist ^= mTranspositionNumbers[m.getMovedPiece() + (m.getMovedColor() == WHITE ? 0 : 6)][m.getToTile()];
}

void Algorithm::undoZobristMove(const Move& m, u64& zobrist)
{
    zobrist ^= mTranspositionNumbers[m.getMovedPiece() + (m.getMovedColor() == WHITE ? 0 : 6)][m.getToTile()];
    zobrist ^= mTranspositionNumbers[m.getMovedPiece() + (m.getMovedColor() == WHITE ? 0 : 6)][m.getFromTile()];
}

u64 Algorithm::boardToZobristKey(const Board& board)
{
    u64 hash = 0ull;
    for (int color = 0; color < 2; color++)
    {
        for (int i = 0; i < 6; i++)
        {
            u64 temp = board.PieceBoards[color][i];

            while (temp)
            {
                int index = mChessBoard->get_ls1b_index(temp);
                temp &= ~(1ull << index);
                hash ^= mTranspositionNumbers[color == 0 ? i : i + 6][index];
            }
        }
    }
    return hash;
}

int Algorithm::NegaMax(int depth, int alpha, int beta, Color color, u64 zobrist)
{
    if (depth == 0)
        return evaluate() * (color == 0 ? 1 : -1);
    if (mChessBoard->isKingInCheck(color))
        return -2000;
    if (mChessBoard->isKingInCheck(color ^ 1))
        return 2000;

    if ((iter = mTranspositionTable.find(zobrist)) != mTranspositionTable.end())
    {
        const ZobristMove& zm = iter->second;
        mZobristMove++;
        return zm.eval;
    }

    mSearchedNodes++;
    const std::vector<Move> moves = getAllValidMoves(color);
 
    int bestVal = INT_MIN;
    for (int i = 0; i < moves.size(); i++)
    {
        Move m = moves[i];

        mChessBoard->applyMove(m);
        applyZobristMove(m, zobrist);
        int value = -NegaMax(depth - 1, -beta, -alpha, color ^ 1, zobrist);
        applyZobristMove(m, zobrist);
        mChessBoard->undoMove(m);
        mTranspositionTable.insert({ zobrist, {value} });
        bestVal = max(bestVal, value);
        alpha = max(alpha, bestVal);
        if (alpha >= beta)
        {
            mPrunedNodes++;
            break;
        }        
    }

    return bestVal;
}

std::vector<Move> Algorithm::getAllValidMoves(Color color)
{
    std::vector<Move> arr;
    arr.reserve(200);
    for (int i = 0; i < NUM_BOARDS; i++)
    {
        u64 board = mChessBoard->mBoard.PieceBoards[color][i];
        while (board)
        {
            int index = mChessBoard->get_ls1b_index(board);
            board &= ~(1ull << index);

            mChessBoard->getSuedoLegalMoves(i, index, color, mChessBoard->mBoard.All, arr);
        }
    }

    return arr;
}

int Algorithm::evaluate()
{
    const int PawnVal = 1;
    const int RookVal = 3;
    const int KnightVal = 3;
    const int BishopVal = 6;
    const int QueenVal = 9;
    const int KingVal = 1000;

    int val = 0;

    val += (mChessBoard->numBitsSet(mChessBoard->mBoard.PieceBoards[WHITE][PAWN])
        - mChessBoard->numBitsSet(mChessBoard->mBoard.PieceBoards[BLACK][PAWN])) * PawnVal;
    val += (mChessBoard->numBitsSet(mChessBoard->mBoard.PieceBoards[WHITE][ROOK])
        - mChessBoard->numBitsSet(mChessBoard->mBoard.PieceBoards[BLACK][ROOK])) * RookVal;
    val += (mChessBoard->numBitsSet(mChessBoard->mBoard.PieceBoards[WHITE][BISHOP])
        - mChessBoard->numBitsSet(mChessBoard->mBoard.PieceBoards[BLACK][BISHOP])) * BishopVal;
    val += (mChessBoard->numBitsSet(mChessBoard->mBoard.PieceBoards[WHITE][KNIGHT])
        - mChessBoard->numBitsSet(mChessBoard->mBoard.PieceBoards[BLACK][KNIGHT])) * KnightVal;
    val += (mChessBoard->numBitsSet(mChessBoard->mBoard.PieceBoards[WHITE][QUEEN])
        - mChessBoard->numBitsSet(mChessBoard->mBoard.PieceBoards[BLACK][QUEEN])) * QueenVal;

    if (mChessBoard->isKingInCheck(mChessBoard->mTurn))
        val -= KingVal;

    return val;
}

u64 Algorithm::random()
{
    static u64 begin = 5342522533ull; // start with a prime
    u64 x = begin;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return (begin = x);
}

