#include "windows_common.h"
#include "renderer.h"
#include "platform.h"
#include "game.h"
#include "events.h"

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

HDC windowDC;
HDC frameBufferDC;

HANDLE event;

void windowsBlitToScreen(void)
{
	BitBlt(windowDC, 0, 0, framebuffer.width, framebuffer.height, frameBufferDC, 0, 0, SRCCOPY);
}

void windowsDebugLog(const char *message)
{
	char messageCopy[LOG_SIZE];
	int len = 0;
	while (len < LOG_SIZE - 3)
	{
		char c = message[len];
		if (c == 0)
		{
			break;
		}
		messageCopy[len] = c;
		len++;
	}
	messageCopy[len] = '\r';
	messageCopy[len + 1] = '\n';
	messageCopy[len + 2] = 0;
	OutputDebugStringA(messageCopy);
}

void windowsMakeComputerMove(void)
{
	SetEvent(event);
}
