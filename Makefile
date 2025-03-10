.SUFFIXES: .c .qoi. .ttf
CC      = cl /nologo
RC      = rc /nologo
CFLAGS  = /O2

src   = src\assets.c src\events.c src\fonts.c src\game.c src\pcgrandom.c \
        src\platform.c src\renderer.c src\windows_common.c src\windows_main.c
embed = assets\images\black-bishop.c assets\images\black-king.c \
        assets\images\black-knight.c assets\images\black-pawn.c \
        assets\images\black-queen.c  assets\images\black-rook.c \
        assets\images\white-bishop.c assets\images\white-king.c \
        assets\images\white-knight.c assets\images\white-pawn.c \
        assets\images\white-queen.c  assets\images\white-rook.c \
        assets\fonts\LiberationSans-Regular.c

chess.exe: $(src) icon.res $(embed)
	$(CC) $(CFLAGS) /Fe:$@ $(src) icon.res gdi32.lib user32.lib advapi32.lib

embed.exe: src\embed.c
	$(CC) src\embed.c

icon.rc:
	echo 1 ICON "assets\chess.ico" >$@

clean:
	del /f chess.exe embed.exe icon.res icon.rc $(embed) *.obj

.qoi.c:
	embed <$< >$@

.ttf.c:
	embed <$< >$@

assets\images\black-bishop.c: embed.exe
assets\images\black-king.c: embed.exe
assets\images\black-knight.c: embed.exe
assets\images\black-pawn.c: embed.exe
assets\images\black-queen.c: embed.exe
assets\images\black-rook.c: embed.exe
assets\images\white-bishop.c: embed.exe
assets\images\white-king.c: embed.exe
assets\images\white-knight.c: embed.exe
assets\images\white-pawn.c: embed.exe
assets\images\white-queen.c: embed.exe
assets\images\white-rook.c: embed.exe
assets\fonts\LiberationSans-Regular.c: embed.exe
