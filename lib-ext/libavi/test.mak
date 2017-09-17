CC=i386-mingw32msvc-gcc
CFLAGS=-c -Wall
LINK=i386-mingw32msvc-gcc
LDFAGS=
DLLTOOL=i386-mingw32msvc-dlltool

OBJS=test.o aviplayer.o
LIBS=-L. -lopengl32 -lglut32 -ldsound -lavi

all: test.exe

test.exe: $(OBJS) libavi.a
	$(LINK) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.def.a:
	$(DLLTOOL) -l $@ -D $*.dll -d $<

clean:
	$(RM) $(OBJS)
	$(RM) test.exe
	$(RM) *~