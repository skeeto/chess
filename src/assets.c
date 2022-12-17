#include "assets.h"
#include "renderer.h"
#include "platform.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct qoidecoder {
    int width, height, alpha, srgb;
    unsigned char *p;        // internal
    int last, run;           // internal
    unsigned c, table[64];   // internal
};

static struct qoidecoder qoidecoder(const void *buf)
{
    unsigned char *p = (void *)buf;
    unsigned g = (unsigned)p[ 0]<<24 | p[ 1]<<16 | p[ 2]<< 8 | p[ 3];
    unsigned w = (unsigned)p[ 4]<<24 | p[ 5]<<16 | p[ 6]<< 8 | p[ 7];
    unsigned h = (unsigned)p[ 8]<<24 | p[ 9]<<16 | p[10]<< 8 | p[11];
    assert(g == 0x716f6966U); (void)g;
    assert(w>0 && w<1024);
    assert(h>0 && h<1024);
    assert(p[12]-3u <= 1);
    assert(p[13] <= 1);
    struct qoidecoder q = {
        w, h, p[12]==4, p[13]==1, p+14, -1, 0, 0xff000000, {0}
    };
    return q;
}

static unsigned qoidecode(struct qoidecoder *q)
{
    if (q->run) {
        q->run--;
    } else {
        int n, v=*q->p++;
        unsigned char *p=q->p, r, g, b, a;
        switch (v&0xc0) {
        case 0x00:  // INDEX
            q->c = q->table[v];
            break;
        case 0x40:  // DIFF
            r=q->c, g=q->c>>8, b=q->c>>16, a=q->c>>24;
            r += (v>>4 & 3) - 2;
            g += (v>>2 & 3) - 2;
            b += (v>>0 & 3) - 2;
            q->c = r | g<<8 | b<<16 | (unsigned)a<<24;
            break;
        case 0x80:  // LUMA
            n = v - (0x80 + 32);
            r=q->c, g=q->c>>8, b=q->c>>16, a=q->c>>24;
            r += n + (*p>>4) - 8;
            g += n;
            b += n + (*p&15) - 8;
            q->c = r | g<<8 | b<<16 | (unsigned)a<<24;
            q->p += 1;
            break;
        case 0xc0:
            switch ((n = v&63)) {
            case 63:  // RGBA
                q->c = p[0] | p[1]<<8 | p[2]<<16 | (unsigned)p[3]<<24;
                q->p += 4;
                break;
            case 62:  // RGB
                r=p[0], g=p[1], b=p[2], a=q->c>>24;
                q->c = r | g<<8 | b<<16 | (unsigned)a<<24;
                q->p += 3;
                break;
            default:  // RUN
                q->run = n;
                break;
            }
        }
        r=q->c, g=q->c>>8, b=q->c>>16, a=q->c>>24;
        q->table[(r*3 + g*5 + b*7 + a*11)&63] = q->c;
        q->last = v;
    }
    return q->c;
}


static void loadQoi(const uint8_t *data, Image *image)
{
    struct qoidecoder q = qoidecoder(data);
    image->width  = q.width;
    image->height = q.height;
    image->data   = malloc(4*q.width*q.height);
    int count = q.width * q.height;
    for (int i = 0; i < count; i++) {
        unsigned abgr = qoidecode(&q);
        image->data[4*i+0] = abgr >> 16;
        image->data[4*i+1] = abgr >>  8;
        image->data[4*i+2] = abgr >>  0;
        image->data[4*i+3] = abgr >> 24;
    }
}

void loadImages(void)
{
    static const uint8_t blackBishopQoi[] = {
        #include "../assets/images/black-bishop.c"
    };
    loadQoi(blackBishopQoi, &blackBishop);
    static const uint8_t blackKingQoi[] = {
        #include "../assets/images/black-king.c"
    };
    loadQoi(blackKingQoi, &blackKing);
    static const uint8_t blackKnightQoi[] = {
        #include "../assets/images/black-knight.c"
    };
    loadQoi(blackKnightQoi, &blackKnight);
    static const uint8_t blackPawnQoi[] = {
        #include "../assets/images/black-pawn.c"
    };
    loadQoi(blackPawnQoi, &blackPawn);
    static const uint8_t blackQueenQoi[] = {
        #include "../assets/images/black-queen.c"
    };
    loadQoi(blackQueenQoi, &blackQueen);
    static const uint8_t blackRookQoi[] = {
        #include "../assets/images/black-rook.c"
    };
    loadQoi(blackRookQoi, &blackRook);
    static const uint8_t whiteBishopQoi[] = {
        #include "../assets/images/white-bishop.c"
    };
    loadQoi(whiteBishopQoi, &whiteBishop);
    static const uint8_t whiteKingQoi[] = {
        #include "../assets/images/white-king.c"
    };
    loadQoi(whiteKingQoi, &whiteKing);
    static const uint8_t whiteKnightQoi[] = {
        #include "../assets/images/white-knight.c"
    };
    loadQoi(whiteKnightQoi, &whiteKnight);
    static const uint8_t whitePawnQoi[] = {
        #include "../assets/images/white-pawn.c"
    };
    loadQoi(whitePawnQoi, &whitePawn);
    static const uint8_t whiteQueenQoi[] = {
        #include "../assets/images/white-queen.c"
    };
    loadQoi(whiteQueenQoi, &whiteQueen);
    static const uint8_t whiteRookQoi[] = {
        #include "../assets/images/white-rook.c"
    };
    loadQoi(whiteRookQoi, &whiteRook);
}
