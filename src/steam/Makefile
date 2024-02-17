CFLAGS = /permissive-\
	/GS\
	/W2\
	/Gy\
	/Zc:wchar_t\
	/Zi\
	/Gm-\
	/O2\
	/sdl\
	/Zc:inline\
	/fp:precise\
	/utf-8\
	/D "NDEBUG"\
	/D "_WINDOWS"\
	/D Z80_DISABLE_DEBUG\
	/D Z80_DISABLE_BREAKPOINT\
	/D Z80_DISABLE_NESTCHECK\
	/D Z80_CALLBACK_WITHOUT_CHECK\
	/D Z80_CALLBACK_PER_INSTRUCTION\
	/D Z80_UNSUPPORT_16BIT_PORT\
	/D Z80_NO_FUNCTIONAL\
	/D Z80_NO_EXCEPTION\
	/D _CRT_SECURE_NO_WARNINGS\
	/I ../core\
	/errorReport:prompt\
	/WX-\
	/Zc:forScope\
	/Gd\
	/Oi\
	/MT\
	/FC\
	/EHsc\
	/nologo\
	/Fp"game.pch"\
	/diagnostics:column

OBJECTS = winmain.obj\
	vgs0math.obj\
	vgstone.obj\
	resource.res

all: VGSZERO.exe steam_api.dll
	-rmdir /S /Q release
	mkdir release
	copy VGSZERO.exe release
	copy VGSZERO.exe.manifest release
	copy steam_api.dll release
	copy README.txt release\README.TXT
	copy game.pkg release
	copy LICENSE-PICOJSON.txt release
	copy LICENSE-VGS0.txt release
	copy LICENSE-VGS0LIB.txt release
	copy LICENSE-Z80.txt release
	VGSZERO.exe

steam_api.dll: ./sdk/redistributable_bin/steam_api.dll
	COPY /Y .\sdk\redistributable_bin\steam_api.dll steam_api.dll

VGSZERO.exe: $(OBJECTS)
	LINK\
	/OUT:VGSZERO.exe\
	/MANIFEST\
	/NXCOMPAT\
	/PDB:game.pdb\
	/DYNAMICBASE\
	"kernel32.lib"\
	"user32.lib"\
	"gdi32.lib"\
	"winspool.lib"\
	"comdlg32.lib"\
	"advapi32.lib"\
	"shell32.lib"\
	"ole32.lib"\
	"oleaut32.lib"\
	"imm32.lib"\
	"uuid.lib"\
	"odbc32.lib"\
	"odbccp32.lib"\
	"winmm.lib"\
	"dsound.lib"\
	"dxguid.lib"\
	"d3d9.lib"\
	"dinput8.lib"\
	"./sdk/redistributable_bin/steam_api.lib"\
	/DEBUG\
	/MACHINE:X86\
	/OPT:REF\
	/SUBSYSTEM:WINDOWS\
	/MANIFESTUAC:"level='asInvoker'\
	uiAccess='false'"\
	/ManifestFile:"VGSZERO.exe.manifest"\
	/OPT:ICF\
	/ERRORREPORT:PROMPT\
	/ILK:"game.ilk"\
	/NOLOGO\
	/TLBID:1\
	$(OBJECTS)

clean:
	DEL /S /Q $(OBJECTS)
	DEL /S /Q *.exe
	DEL /S /Q *.exe.manifest
	DEL /S /Q *.pdb
	DEL /S /Q *.iobj

winmain.obj: src/winmain.cpp ../core/vdp.hpp ../core/vgs0.hpp ../core/vgs0def.h ../core/vgsdecv.hpp ../core/z80.hpp src/keyconfig.hpp src/inputmgr.hpp src/steam.hpp
	CL $(CFLAGS) /c src/winmain.cpp

vgs0math.obj: ../core/vgs0math.c
	CL $(CFLAGS) /c ../core/vgs0math.c

vgstone.obj: ../core/vgstone.c
	CL $(CFLAGS) /c ../core/vgstone.c

resource.res: src/resource.rc src/resource.h
	RC /fo resource.res /I src src\resource.rc