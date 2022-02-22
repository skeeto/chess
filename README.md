# chess

Chess game written in C99 with an AI opponent, originally by Daniel Bomar.
Currently supports Linux, BSD, and Windows.

Most everything has been written from scratch. The only libraries used are
Xlib (BSD/Linux) and `stb_truetype` (header only library, comes included
in source). The Windows build uses the Windows API directly.

## Building

To build, you will need a C compiler and Make. Build has been tested with
GCC, Clang, and MSVC.

Linux/BSD:

	make

Windows (native or cross-compiled):

	make EXE=.exe

MSVC:

	nmake /f NMakefile

## Command-line arguments

* `-test` or `-test -verbose`: Calculates number of positions from some
  test FEN strings. Compares to known good results. Used for bug testing.

* `-ai`: Has the AI play a game against itself.
