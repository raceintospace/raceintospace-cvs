EXTRA_WARNINGS =  -Wextra -Wno-unused-parameter \
	-Wno-char-subscripts \
	-Werror

CFLAGS = -g -Wall -Dfar= -Dpascal= -I. `sdl-config --cflags`

LIBS = `sdl-config --libs` -lvorbisfile -lm

.c.o:
	@echo "$(CC) ...flags... -c $*.c"
	@$(CC) $(CFLAGS) $(EXTRA_WARNINGS) -O -c -o TMP.o $*.c
	@$(CC) $(CFLAGS) $(EXTRA_WARNINGS) -c $*.c

PROGS = raceintospace imgsplit vtest decode getport mkmovie getvab mtest sdltest
all: $(PROGS) 

EXTRA_SOURCES = README raceintospace.1

rpm_all: raceintospace

DESTDIR=/

install: raceintospace
	mkdir -p $(DESTDIR)/usr/bin
	mkdir -p $(DESTDIR)/usr/share/man/man1
	mkdir -p $(DESTDIR)/usr/share/doc/raceintospace
	mkdir -p $(DESTDIR)/usr/share/raceintospace/music
	install -c -m 755 raceintospace $(DESTDIR)/usr/bin/raceintospace
	gzip < raceintospace.1 \
	   > $(DESTDIR)/usr/share/man/man1/raceintospace.1.gz
	install -c -m 644 README $(DESTDIR)/usr/share/doc/raceintospace/.

rpm_install: install
	cp -pr /usr/share/raceintospace/music/*.ogg \
		$(DESTDIR)/usr/share/raceintospace/music/.

uninstall:
	rm -f $(DESTDIR)/usr/bin/raceintospace
	rm -f $(DESTDIR)/usr/share/man/man1/raceintospace.1*
	rm -rf $(DESTDIR)/usr/share/doc/raceintospace
	rm -f $(DESTDIR)/usr/share/raceintospace/README

BARIS_HFILES = Buzz_inc.h data.h externs.h mis.h mtype.h	\
	music.h nn.h pace.h pcx_hdr.h proto.h records.h replay.h \
	soundfx.h	\
	sv_lib.h uc.h soundint.h

BARIS_OBJS = admin.o aimast.o aimis.o aipur.o ast0.o ast1.o	    \
	ast2.o ast3.o ast4.o budget.o crew.o endgame.o	    \
	futbub.o future.o hardef.o intel.o intro.o main.o mc2.o mc.o \
	mis_c.o mis_m.o museum.o name.o newmis.o news.o news_sup.o	    \
	news_suq.o							    \
	place.o port.o prefs.o prest.o radar.o rdplex.o recods.o \
	replay.o review.o rush.o start.o   \
	vab.o pace.o gx.o gr.o sdl.o music.o
raceintospace: $(BARIS_OBJS)
	$(CC) $(CFLAGS) -o raceintospace $(BARIS_OBJS) $(LIBS)

# $(BARIS_OBJS): $(BARIS_HFILES)


imgsplit: imgsplit.o
	$(CC) $(CFLAGS) -o imgsplit imgsplit.o -lm

vtest: vtest.o
	$(CC) $(CFLAGS) -o vtest vtest.o -lasound -lm

decode: decode.o
	$(CC) $(CFLAGS) -o decode decode.o -lm

getport: getport.o
	$(CC) $(CFLAGS) -o getport getport.o -lm

mkmovie: mkmovie.o
	$(CC) $(CFLAGS) -o mkmovie mkmovie.o -lm

getvab: getvab.o
	$(CC) $(CFLAGS) -o getvab getvab.o -lm

mtest: mtest.o
	$(CC) $(CFLAGS) -o mtest mtest.o -lm

sdl.o: sdl.c
	$(CC) $(CFLAGS) `sdl-config --cflags` -c sdl.c

sdltest: sdltest.o
	$(CC) $(CFLAGS) -o sdltest sdltest.o `sdl-config --libs` -lvorbisfile -lm

clean:
	rm -f *.o *~

tar:
	rm -f raceintospace.tar raceintospace.tar.gz
	tar -czf raceintospace.tar.gz Makefile ChangeLog *.[ch] domount .gdbinit $(EXTRA_SOURCES)





