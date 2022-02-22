.POSIX:
.SUFFIXES: .c .bmp .ttf
CC      = cc
CC_EXE  = x86_64-w64-mingw32-gcc
CFLAGS  = -Wall -Wextra -Wno-unused-function -O3
LDFLAGS = -s
LDLIBS  =

src   = src/assets.c src/events.c src/fonts.c src/game.c src/pcgrandom.c \
        src/platform.c src/renderer.c
embed = assets/images/black-bishop.c assets/images/black-king.c \
        assets/images/black-knight.c assets/images/black-pawn.c \
        assets/images/black-queen.c  assets/images/black-rook.c \
        assets/images/white-bishop.c assets/images/white-king.c \
        assets/images/white-knight.c assets/images/white-pawn.c \
        assets/images/white-queen.c  assets/images/white-rook.c \
        assets/fonts/LiberationSans-Regular.c
linux = $(src) src/linux_common.c src/linux_main.c
win32 = $(src) src/windows_common.c src/windows_main.c

host: chess$(EXE)

all: chess chess.exe

chess: $(linux) $(embed)
	$(CC) -pthread $(CFLAGS) $(LDFLAG) -o $@ $(linux) -lm -lX11 $(LDLIBS)

chess.exe: $(win32) $(embed)
	$(CC_EXE) -mwindows $(CFLAGS) $(LDFLAG) -o $@ $(win32) $(LDLIBS)

.bmp.c:
	xxd -i <$< >$@

.ttf.c:
	xxd -i <$< >$@

clean:
	rm -f chess chess.exe $(embed)
