#------------------------------------------------------------------------------
# Makefile for Borland make
#------------------------------------------------------------------------------
!ifndef ROOT
ROOT = $(MAKEDIR)\..
!endif
#------------------------------------------------------------------------------
BC=$(ROOT)

EXEFILE = dda_settings.exe
OBJFILES = main.obj dda_protocol.obj ..\dda_ctrl\get_opts.obj ..\dda_ctrl\pc_serial.obj ..\dda_ctrl\logfile.obj
INCLUDE_PATH = -I. -I$(BC)\include -I..\dda_ctrl
LIB_PATH = -L$(BC)\lib

.AUTODEPEND
BCC32   = bcc32
ILINK32 = ilink32
BRC32   = brc32

CFLAGS  = -Vx -Ve -X- -r- -a8 -b- -k -y -v -vi- -tWC -tWM -DWIN32 -c $(INCLUDE_PATH)
LFLAGS  = -ap -Tpe -x -Gn -v
RFLAGS  = -X -R 
STDOBJS = c0x32.obj
STDLIBS = import32.lib cw32.lib

$(EXEFILE) : $(OBJFILES) $(RESFILES)
   $(ILINK32) $(LFLAGS) $(LIB_PATH) $(OBJFILES) $(STDOBJS), $(EXEFILE), , \
      $(LIBFILES) $(STDLIBS), $(DEFFILE), $(RESFILES)

clean:
   del *.obj *.res *.tds *.map

.cpp.obj:
      $(BCC32) $(CFLAGS) $(CPPFLAGS) $(INCLUDE_PATH) /c $*.cpp

.c.obj:
      $(BCC32) $(CFLAGS) $(INCLUDE_PATH) /c $*.c
