.POSIX:
.SUFFIXES: .c .qoi .ttf
CC      = cc
HOST_CC = cc
EXE_CC  = $(CROSS)gcc
WINDRES = $(CROSS)windres
CROSS   = x86_64-w64-mingw32-
CFLAGS  = -Wall -Wextra -Wno-unused-function -O3 -DNDEBUG
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

chess.exe: $(win32) $(embed) icon.o
	$(EXE_CC) -mwindows $(CFLAGS) $(LDFLAG) -o $@ $(win32) icon.o $(LDLIBS)

icon.o: assets/chess.ico
	echo '1 ICON "assets/chess.ico"' | $(WINDRES) -o $@

embed$(EXE): src/embed.c
	$(HOST_CC) -o $@ src/embed.c

clean:
	rm -f chess chess.exe embed$(EXE) icon.o $(embed)

.qoi.c:
	./embed <$< >$@

.ttf.c:
	./embed <$< >$@

assets/images/black-bishop.c: embed$(EXE)
assets/images/black-king.c: embed$(EXE)
assets/images/black-knight.c: embed$(EXE)
assets/images/black-pawn.c: embed$(EXE)
assets/images/black-queen.c: embed$(EXE)
assets/images/black-rook.c: embed$(EXE)
assets/images/white-bishop.c: embed$(EXE)
assets/images/white-king.c: embed$(EXE)
assets/images/white-knight.c: embed$(EXE)
assets/images/white-pawn.c: embed$(EXE)
assets/images/white-queen.c: embed$(EXE)
assets/images/white-rook.c: embed$(EXE)
assets/fonts/LiberationSans-Regular.c: embed$(EXE)
