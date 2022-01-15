#include <stdlib.h>
#include <stdio.h>

#include "game.h"
#include "platform.h"

GameState gameState;

void movePiece(uint8_t moveTo, uint8_t moveFrom, GameState *state)
{
    uint8_t destination = moveTo & MOVE_MASK;
    uint8_t piece = state->board[moveFrom];
    uint8_t pieceOwner = piece & PIECE_OWNER_MASK;
    uint8_t pieceType = piece & PIECE_TYPE_MASK;
    if (pieceType == PAWN || state->board[destination] != 0)
    {
        state->halfMoves = 0;
    }
    else
    {
        state->halfMoves++;
    }
    if (moveTo & ENPASSANT_MASK)
    {
        if (pieceType == BLACK)
        {
            state->board[destination - 8] = 0;
        }
        else
        {
            state->board[destination + 8] = 0;
        }
    }
    else if (moveTo & CASTLE_MASK)
    {
        if (destination > moveFrom)
        {
            state->board[destination - 1] = state->board[destination + 1];
            state->board[destination + 1] = 0;
        }
        else
        {
            state->board[destination + 1] = state->board[destination - 2];
            state->board[destination - 2] = 0;
        }
    }
    state->enPassantSquare = 255;
    if (pieceType == PAWN)
    {
        if (pieceOwner == BLACK)
        {
            if (destination >= 56)
            {
                piece = BLACK | QUEEN;
            }
            else if (destination - moveFrom == 16)
            {
                state->enPassantSquare = destination - 8;
            }
        }
        else
        {
            if (destination <= 7)
            {
                piece = WHITE | QUEEN;
            }
            else if (moveFrom - destination == 16)
            {
                state->enPassantSquare = destination + 8;
            }
        }
    }
    else if (pieceType == KING)
    {
        if (pieceOwner == WHITE)
        {
            state->castlingAvailablity &= ~(CASTLE_WHITE_KING | CASTLE_WHITE_QUEEN);
        }
        else
        {
            state->castlingAvailablity &= ~(CASTLE_BLACK_KING | CASTLE_BLACK_QUEEN);
        }
    }
    if (destination == 0 || moveFrom == 0)
    {
        state->castlingAvailablity &= ~CASTLE_BLACK_QUEEN;
    }
    if (destination == 7 || moveFrom == 7)
    {
        state->castlingAvailablity &= ~CASTLE_BLACK_KING;
    }
    if (destination == 56 || moveFrom == 56)
    {
        state->castlingAvailablity &= ~CASTLE_WHITE_QUEEN;
    }
    if (destination == 63 || moveFrom == 63)
    {
        state->castlingAvailablity &= ~CASTLE_WHITE_KING;
    }
    state->board[destination] = piece;
    state->board[moveFrom] = 0;
}

static uint8_t getKingLocation(uint8_t owner, GameState *state)
{
    uint8_t king = owner | KING;
    for (int i = 0; i < 64; i++)
    {
        if (state->board[i] == king)
        {
            return i;
        }
    }
    debugLog("wtf couldn't find king");
    return 0;
}

static int pawnPossibleMoves(uint8_t cell, uint8_t *moves, GameState *state)
{
    uint8_t pawnOwner = state->board[cell] & PIECE_OWNER_MASK;
    int numMoves = 0;
    int direction;
    int startingRow;
    if (pawnOwner == BLACK)
    {
        direction = 8;
        startingRow = 8;
    }
    else
    {
        direction = -8;
        startingRow = 48;
    }
    int move = cell + direction;
    if (move >= 0 && move < 64 && state->board[move] == 0)
    {
        moves[numMoves++] = move;
        if (cell >= startingRow && cell < startingRow + 8)
        {
            move += direction;
            if (move >= 0 && move < 64 && state->board[move] == 0)
            {
                moves[numMoves++] = move;
            }
        }
    }
    uint8_t col = cell % 8;
    int captureMoves[2];
    int numCaptureMoves = 0;
    if (pawnOwner == BLACK)
    {
        if (col > 0)
        {
            captureMoves[numCaptureMoves++] = cell + 7;
        }
        if (col < 7)
        {
            captureMoves[numCaptureMoves++] = cell + 9;
        }
    }
    else
    {
        if (col > 0)
        {
            captureMoves[numCaptureMoves++] = cell - 9;
        }
        if (col < 7)
        {
            captureMoves[numCaptureMoves++] = cell - 7;
        }
    }
    for (int i = 0; i < numCaptureMoves; i++)
    {
        if (captureMoves[i] >= 0 && captureMoves[i] < 64)
        {
            if (state->board[captureMoves[i]] != 0 && (state->board[captureMoves[i]] & PIECE_OWNER_MASK) != pawnOwner)
            {
                moves[numMoves++] = captureMoves[i];
            }
            // Check for en passant
            else if (captureMoves[i] == state->enPassantSquare)
            {
                moves[numMoves++] = captureMoves[i] | ENPASSANT_MASK;
            }
        }
    }
    return numMoves;
}

static int knightPossibleMoves(uint8_t cell, uint8_t *moves, GameState *state)
{
    uint8_t knightOwner = state->board[cell] & PIECE_OWNER_MASK;
    uint8_t col = cell % 8;
    int possibleMoves[8];
    int numPossibleMoves = 0;
    if (col < 7)
    {
        possibleMoves[numPossibleMoves++] = cell + 17;
        possibleMoves[numPossibleMoves++] = cell - 15;
        if (col < 6)
        {
            possibleMoves[numPossibleMoves++] = cell + 10;
            possibleMoves[numPossibleMoves++] = cell - 6;
        }
    }
    if (col > 0)
    {
        possibleMoves[numPossibleMoves++] = cell + 15;
        possibleMoves[numPossibleMoves++] = cell - 17;
        if (col > 1)
        {
            possibleMoves[numPossibleMoves++] = cell + 6;
            possibleMoves[numPossibleMoves++] = cell - 10;
        }
    }
    int numMoves = 0;
    for (int i = 0; i < numPossibleMoves; i++)
    {
        if (possibleMoves[i] >= 0 && possibleMoves[i] < 64 && (state->board[possibleMoves[i]] & PIECE_OWNER_MASK) != knightOwner)
        {
            moves[numMoves++] = possibleMoves[i];
        }
    }
    return numMoves;
}

static int bishopPossibleMoves(uint8_t cell, uint8_t *moves, GameState *state)
{
    uint8_t bishopOwner = state->board[cell] & PIECE_OWNER_MASK;
    uint8_t col = cell % 8;
    uint8_t row = cell / 8;
    uint8_t x = col;
    uint8_t y = row;
    int numMoves = 0;
    // Up left
    uint8_t move = cell - 9;
    while (x > 0 && y > 0)
    {
        if (state->board[move] != 0)
        {
            if ((state->board[move] & PIECE_OWNER_MASK) != bishopOwner)
            {
                moves[numMoves++] = move;
            }
            break;
        }
        moves[numMoves++] = move;
        move -= 9;
        x--;
        y--;
    }
    x = col;
    y = row;
    // Down left
    move = cell + 7;
    while (x > 0 && y < 7)
    {
        if (state->board[move] != 0)
        {
            if ((state->board[move] & PIECE_OWNER_MASK) != bishopOwner)
            {
                moves[numMoves++] = move;
            }
            break;
        }
        moves[numMoves++] = move;
        move += 7;
        x--;
        y++;
    }
    x = col;
    y = row;
    // Up right
    move = cell - 7;
    while (x < 7 && y > 0)
    {
        if (state->board[move] != 0)
        {
            if ((state->board[move] & PIECE_OWNER_MASK) != bishopOwner)
            {
                moves[numMoves++] = move;
            }
            break;
        }
        moves[numMoves++] = move;
        move -= 7;
        x++;
        y--;
    }
    x = col;
    y = row;
    // Down right
    move = cell + 9;
    while (x < 7 && y < 7)
    {
        if (state->board[move] != 0)
        {
            if ((state->board[move] & PIECE_OWNER_MASK) != bishopOwner)
            {
                moves[numMoves++] = move;
            }
            break;
        }
        moves[numMoves++] = move;
        move += 9;
        x++;
        y++;
    }
    return numMoves;
}

static int rookPossibleMoves(uint8_t cell, uint8_t *moves, GameState *state)
{
    uint8_t rookOwner = state->board[cell] & PIECE_OWNER_MASK;
    uint8_t col = cell % 8;
    uint8_t row = cell / 8;
    int numMoves = 0;
    uint8_t x = col;
    uint8_t move = cell - 1;
    while (x > 0)
    {
        if (state->board[move] != 0)
        {
            if ((state->board[move] & PIECE_OWNER_MASK) != rookOwner)
            {
                moves[numMoves++] = move;
            }
            break;
        }
        moves[numMoves++] = move;
        move--;
        x--;
    }
    x = col;
    move = cell + 1;
    while (x < 7)
    {
        if (state->board[move] != 0)
        {
            if ((state->board[move] & PIECE_OWNER_MASK) != rookOwner)
            {
                moves[numMoves++] = move;
            }
            break;
        }
        moves[numMoves++] = move;
        move++;
        x++;
    }
    uint8_t y = row;
    move = cell - 8;
    while (y > 0)
    {
        if (state->board[move] != 0)
        {
            if ((state->board[move] & PIECE_OWNER_MASK) != rookOwner)
            {
                moves[numMoves++] = move;
            }
            break;
        }
        moves[numMoves++] = move;
        move -= 8;
        y--;
    }
    y = row;
    move = cell + 8;
    while (y < 7)
    {
        if (state->board[move] != 0)
        {
            if ((state->board[move] & PIECE_OWNER_MASK) != rookOwner)
            {
                moves[numMoves++] = move;
            }
            break;
        }
        moves[numMoves++] = move;
        move += 8;
        y++;
    }
    return numMoves;
}

static int queenPossibleMoves(uint8_t cell, uint8_t *moves, GameState *state)
{
    int numMoves = bishopPossibleMoves(cell, moves, state);
    return numMoves + rookPossibleMoves(cell, moves + numMoves, state);
}

static int getCastlingMoves(uint8_t kingOwner, uint8_t *moves, GameState *state)
{
    uint8_t backRow;
    bool queenSide;
    bool kingSide;
    if (kingOwner == BLACK)
    {
        backRow = 0;
        if (state->castlingAvailablity & CASTLE_BLACK_QUEEN)
        {
            queenSide = true;
        }
        else
        {
            queenSide = false;
        }
        if (state->castlingAvailablity & CASTLE_BLACK_KING)
        {
            kingSide = true;
        }
        else
        {
            kingSide = false;
        }
    }
    else
    {
        backRow = 56;
        if (state->castlingAvailablity & CASTLE_WHITE_QUEEN)
        {
            queenSide = true;
        }
        else
        {
            queenSide = false;
        }
        if (state->castlingAvailablity & CASTLE_WHITE_KING)
        {
            kingSide = true;
        }
        else
        {
            kingSide = false;
        }
    }
    if (queenSide)
    {
        for (int i = 1; i < 4; i++)
        {
            if (state->board[backRow + i] != 0)
            {
                queenSide = false;
                break;
            }
        }
    }
    if (kingSide)
    {
        for (int i = 5; i < 7; i++)
        {
            if (state->board[backRow + i] != 0)
            {
                kingSide = false;
                break;
            }
        }
    }
    int numMoves = 0;
    if (queenSide)
    {
        moves[numMoves++] = (backRow + 2) | CASTLE_MASK;
    }
    if (kingSide)
    {
        moves[numMoves++] = (backRow + 6) | CASTLE_MASK;
    }
    return numMoves;
}

static int kingPossibleMoves(uint8_t cell, uint8_t *moves, GameState *state)
{
    uint8_t kingOwner = state->board[cell] & PIECE_OWNER_MASK;
    uint8_t col = cell % 8;
    uint8_t row = cell / 8;
    int numMoves = 0;
    uint8_t move = cell - 1;
    if (col > 0 && (state->board[move] & PIECE_OWNER_MASK) != kingOwner)
    {
        moves[numMoves++] = move;
    }
    move = cell + 1;
    if (col < 7 && (state->board[move] & PIECE_OWNER_MASK) != kingOwner)
    {
        moves[numMoves++] = move;
    }
    move = cell - 8;
    if (row > 0 && (state->board[move] & PIECE_OWNER_MASK) != kingOwner)
    {
        moves[numMoves++] = move;
    }
    move = cell + 8;
    if (row < 7 && (state->board[move] & PIECE_OWNER_MASK) != kingOwner)
    {
        moves[numMoves++] = move;
    }
    move = cell - 9;
    if (col > 0 && row > 0 && (state->board[move] & PIECE_OWNER_MASK) != kingOwner)
    {
        moves[numMoves++] = move;
    }
    move = cell + 7;
    if (col > 0 && row < 7 && (state->board[move] & PIECE_OWNER_MASK) != kingOwner)
    {
        moves[numMoves++] = move;
    }
    move = cell - 7;
    if (col < 7 && row > 0 && (state->board[move] & PIECE_OWNER_MASK) != kingOwner)
    {
        moves[numMoves++] = move;
    }
    move = cell + 9;
    if (col < 7 && row < 7 && (state->board[move] & PIECE_OWNER_MASK) != kingOwner)
    {
        moves[numMoves++] = move;
    }
    return numMoves + getCastlingMoves(kingOwner, moves + numMoves, state);
}

static int piecePossibleMoves(uint8_t cell, uint8_t *moves, GameState *state)
{
    switch(state->board[cell] & PIECE_TYPE_MASK)
    {
        case PAWN:
            return pawnPossibleMoves(cell, moves, state);
        case KNIGHT:
            return knightPossibleMoves(cell, moves, state);
        case BISHOP:
            return bishopPossibleMoves(cell, moves, state);
        case ROOK:
            return rookPossibleMoves(cell, moves, state);
        case QUEEN:
            return queenPossibleMoves(cell, moves, state);
        case KING:
            return kingPossibleMoves(cell, moves, state);
    }
    return 0;
}

int pieceLegalMoves(uint8_t cell, uint8_t *moves, GameState *state)
{
    uint8_t owner = state->board[cell] & PIECE_OWNER_MASK;
    uint8_t opponent;
    if (owner == BLACK)
    {
        opponent = WHITE;
    }
    else
    {
        opponent = BLACK;
    }
    uint8_t possibleMoves[64];
    int numPossibleMoves = piecePossibleMoves(cell, possibleMoves, state);
    int numLegalMoves = 0;
    for (int i = 0; i < numPossibleMoves; i++)
    {
        bool legalMove = true;
        if (possibleMoves[i] & CASTLE_MASK)
        {
            uint8_t backRow = owner == BLACK ? 0 : 56;
            uint8_t king = backRow + 4;
            bool kingSide = (possibleMoves[i] & MOVE_MASK) > cell;
            for (int j = 0; j < 64; j++)
            {
                if ((state->board[j] & PIECE_OWNER_MASK) == opponent)
                {
                    uint8_t opponentMoves[64];
                    int numOpponentMoves = piecePossibleMoves(j, opponentMoves, state);
                    for (int k = 0; k < numOpponentMoves; k++)
                    {
                        uint8_t move = opponentMoves[k] & MOVE_MASK;
                        if (move == king)
                        {
                            legalMove = false;
                            break;
                        }
                        if (kingSide)
                        {
                            if (move == backRow + 5 || move == backRow + 6)
                            {
                                legalMove = false;
                                break;
                            }
                        }
                        else
                        {
                            if (move == backRow + 2 || move == backRow + 3)
                            {
                                legalMove = false;
                                break;
                            }
                        }
                    }
                    if (!legalMove)
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            GameState copyState = *state;
            movePiece(possibleMoves[i], cell, &copyState);
            uint8_t king = getKingLocation(owner, &copyState);
            for (int j = 0; j < 64; j++)
            {
                if ((copyState.board[j] & PIECE_OWNER_MASK) == opponent)
                {
                    uint8_t opponentMoves[64];
                    int numOpponentMoves = piecePossibleMoves(j, opponentMoves, &copyState);
                    for (int k = 0; k < numOpponentMoves; k++)
                    {
                        if ((opponentMoves[k] & MOVE_MASK) == king)
                        {
                            legalMove = false;
                            break;
                        }
                    }
                    if (!legalMove)
                    {
                        break;
                    }
                }
            }
        }
        if (legalMove)
        {
            moves[numLegalMoves++] = possibleMoves[i];
        }
    }
    return numLegalMoves;
}

int getAllLegalMoves(uint8_t owner, uint8_t *moveTo, uint8_t *moveFrom, GameState *state)
{
    int totalMoves = 0;
    for (uint8_t cell = 0; cell < 64; cell++)
    {
        if ((state->board[cell] & PIECE_OWNER_MASK) == owner)
        {
            int numMoves = pieceLegalMoves(cell, moveTo + totalMoves, state);
            for (int i = 0; i < numMoves; i++)
            {
                moveFrom[totalMoves + i] = cell;
            }
            totalMoves += numMoves;
        }
    }
    return totalMoves;
}

bool playerInCheck(uint8_t player)
{
    uint8_t opponent;
    if (player == BLACK)
    {
        opponent = WHITE;
    }
    else
    {
        opponent = BLACK;
    }
    uint8_t king = getKingLocation(player, &gameState);
    for (uint8_t cell = 0; cell < 64; cell++)
    {
        if ((gameState.board[cell] & PIECE_OWNER_MASK) == opponent)
        {
            uint8_t moves[64];
            int numMoves = piecePossibleMoves(cell, moves, &gameState);
            for (int i = 0; i < numMoves; i++)
            {
                if ((moves[i] & MOVE_MASK) == king)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

uint64_t calculatePositions(int depth, GameState *state, uint8_t player)
{
    if (depth == 0)
    {
        return 1;
    }
    uint8_t opponent;
    if (player == BLACK)
    {
        opponent = WHITE;
    }
    else
    {
        opponent = BLACK;
    }
    uint64_t totalPositions = 0;
    uint8_t moveTo[1024];
    uint8_t moveFrom[1024];
    int numMoves = getAllLegalMoves(player, moveTo, moveFrom, state);
    for (int i = 0; i < numMoves; i++)
    {
        GameState copyState = *state;
        movePiece(moveTo[i], moveFrom[i], &copyState);
        uint64_t positions = calculatePositions(depth - 1, &copyState, opponent);
        totalPositions += positions;
        if (depth == 6)
        {
            uint8_t fromCell = moveFrom[i] & MOVE_MASK;
            uint8_t toCell = moveTo[i] & MOVE_MASK;
            char string[5];
            string[0] = 'a' + (fromCell % 8);
            string[1] = '8' - (fromCell / 8);
            string[2] = 'a' + (toCell % 8);
            string[3] = '8' - (toCell / 8);
            string[4] = 0;
            printf("%s: %lu\n", string, positions);
        }
    }
    return totalPositions;
}

void initGameState(void)
{
    gameState.halfMoves = 0;
    gameState.enPassantSquare = 255;
    gameState.castlingAvailablity = 255;

    gameState.board[0] = BLACK | ROOK;
    gameState.board[1] = BLACK | KNIGHT;
    gameState.board[2] = BLACK | BISHOP;
    gameState.board[3] = BLACK | QUEEN;
    gameState.board[4] = BLACK | KING;
    gameState.board[5] = BLACK | BISHOP;
    gameState.board[6] = BLACK | KNIGHT;
    gameState.board[7] = BLACK | ROOK;

    for (int i = 8; i < 16; i++)
    {
        gameState.board[i] = BLACK | PAWN;
    }

    for (int i = 16; i < 48; i++)
    {
        gameState.board[i] = 0;
    }

    for (int i = 48; i < 56; i++)
    {
        gameState.board[i] = WHITE | PAWN;
    }

    gameState.board[56] = WHITE | ROOK;
    gameState.board[57] = WHITE | KNIGHT;
    gameState.board[58] = WHITE | BISHOP;
    gameState.board[59] = WHITE | QUEEN;
    gameState.board[60] = WHITE | KING;
    gameState.board[61] = WHITE | BISHOP;
    gameState.board[62] = WHITE | KNIGHT;
    gameState.board[63] = WHITE | ROOK;
}
