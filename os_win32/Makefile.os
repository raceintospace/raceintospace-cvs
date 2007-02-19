CFLAGS += -O2 -DNDEBUG

installer: $(PROG_NAME)
	${CROSS}strip ${PROG_NAME}
	makensis installer.nsi
