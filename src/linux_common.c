#include "linux_common.h"
#include "renderer.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

Display *display;
Window window;
XImage *ximage;
GC gc;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void linuxBlitToScreen(void)
{
    XPutImage(display, window, gc, ximage, 0, 0, 0, 0, framebuffer.width, framebuffer.height);
}

void linuxDebugLog(const char *message)
{
    puts(message);
}

void linuxMakeComputerMove(void)
{
    pthread_cond_signal(&cond);
}
