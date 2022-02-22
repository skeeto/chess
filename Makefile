.POSIX:
CC      = cc
CC_EXE  = x86_64-w64-mingw32-gcc
CFLAGS  = -Wall -Wextra -Wno-unused-function -O3
LDFLAGS = -s
LDLIBS  =

src   = src/assets.c src/events.c src/fonts.c src/game.c src/pcgrandom.c \
        src/platform.c src/renderer.c
linux = $(src) src/linux_common.c src/linux_main.c
win32 = $(src) src/windows_common.c src/windows_main.c

host: chess$(EXE)

all: chess chess.exe

chess: $(linux)
	$(CC) -pthread $(CFLAGS) $(LDFLAG) -o $@ $(linux) -lm -lX11 $(LDLIBS)

chess.exe: $(win32)
	$(CC_EXE) -mwindows $(CFLAGS) $(LDFLAG) -o $@ $(win32) $(LDLIBS)

clean:
	rm -f chess chess.exe
