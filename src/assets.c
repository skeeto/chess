#include "assets.h"
#include "renderer.h"
#include "platform.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void loadBmp(const uint8_t *bmpData, Image *image)
{
    uint32_t startingPixelIndex = *(uint32_t *)&bmpData[10];
    int32_t width = *(int32_t *)&bmpData[18];
    int32_t height = *(int32_t *)&bmpData[22];
    uint32_t pixelDataSize = *(uint32_t *)&bmpData[34];
    uint32_t expectedDataSize = width * height * 4;
    if (expectedDataSize != pixelDataSize)
    {
        char message[LOG_SIZE];
        snprintf(message, LOG_SIZE, "Expected %u bytes. Header reads %u", expectedDataSize, pixelDataSize);
        debugLog(message);
        return;
    }
    uint8_t *pixelData = malloc(pixelDataSize);
    if (pixelData == NULL)
    {
        debugLog("malloc failed");
        return;
    }
    // BMP images have origin at bottom left.
    // Move data so that origin is top left.
    const uint8_t *readPointer = bmpData + startingPixelIndex;
    uint8_t *writePointer = (pixelData + pixelDataSize) - (width * 4);
    for (int i = 0; i < height; i++)
    {
        memcpy(writePointer, readPointer, width * 4);
        readPointer += (width * 4);
        writePointer -= (width * 4);
    }
    image->width = width;
    image->height = height;
    image->data = pixelData;
}

void loadImages(void)
{
    static const uint8_t blackBishopBmp[] = {
        #include "../assets/images/black-bishop.c"
    };
    loadBmp(blackBishopBmp, &blackBishop);
    static const uint8_t blackKingBmp[] = {
        #include "../assets/images/black-king.c"
    };
    loadBmp(blackKingBmp, &blackKing);
    static const uint8_t blackKnightBmp[] = {
        #include "../assets/images/black-knight.c"
    };
    loadBmp(blackKnightBmp, &blackKnight);
    static const uint8_t blackPawnBmp[] = {
        #include "../assets/images/black-pawn.c"
    };
    loadBmp(blackPawnBmp, &blackPawn);
    static const uint8_t blackQueenBmp[] = {
        #include "../assets/images/black-queen.c"
    };
    loadBmp(blackQueenBmp, &blackQueen);
    static const uint8_t blackRookBmp[] = {
        #include "../assets/images/black-rook.c"
    };
    loadBmp(blackRookBmp, &blackRook);
    static const uint8_t whiteBishopBmp[] = {
        #include "../assets/images/white-bishop.c"
    };
    loadBmp(whiteBishopBmp, &whiteBishop);
    static const uint8_t whiteKingBmp[] = {
        #include "../assets/images/white-king.c"
    };
    loadBmp(whiteKingBmp, &whiteKing);
    static const uint8_t whiteKnightBmp[] = {
        #include "../assets/images/white-knight.c"
    };
    loadBmp(whiteKnightBmp, &whiteKnight);
    static const uint8_t whitePawnBmp[] = {
        #include "../assets/images/white-pawn.c"
    };
    loadBmp(whitePawnBmp, &whitePawn);
    static const uint8_t whiteQueenBmp[] = {
        #include "../assets/images/white-queen.c"
    };
    loadBmp(whiteQueenBmp, &whiteQueen);
    static const uint8_t whiteRookBmp[] = {
        #include "../assets/images/white-rook.c"
    };
    loadBmp(whiteRookBmp, &whiteRook);
}
