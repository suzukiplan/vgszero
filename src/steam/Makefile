all:
	NMAKE /nologo src/gamepkg.c

src/gamepkg.c: game.pkg
	CL /MD src\bin2var.cpp
	bin2var.exe game.pkg src\gamepkg.c src\gamepkg.h
