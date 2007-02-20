CFLAGS += -O2 -DNDEBUG

DOCFILES = $(addsuffix .txt,AUTHORS COPYING README DEVELOPER)

%.txt:../%
	sed 's/$$/'`/bin/echo -e \\\r`'/' < $< > $@

installer: $(PROG_NAME) $(DOCFILES)
	${CROSS}strip ${PROG_NAME}
	makensis installer.nsi
