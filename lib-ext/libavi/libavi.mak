CC=cl.exe
CFLAGS=-c -nologo -W3
LINK=link.exe
LDFLAGS= -nologo -dll -def:libavi.def
OBJS=libavi.obj
LIBS=msacm32.lib vfw32.lib user32.lib gdi32.lib
RM=del /q

all: libavi.dll

libavi.dll: $(OBJS)
	$(LINK) $(LDFLAGS) $(OBJS) $(LIBS) -out:$@

.c.obj:
	$(CC) $(CFLAGS) $*.c -Fo$@

clean:
	$(RM) $(OBJS)
	$(RM) libavi.dll
	$(RM) libavi.exp libavi.lib
	$(RM) *~