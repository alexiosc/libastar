SOURCES = \
	../src/astar.c \
	../src/astar.h

all:	doc-stamp

doc-stamp: $(SOURCES)
	doxygen doxygen.rc || true
	touch doc-stamp

clean:
	rm -f doc-stamp || true

distclean: clean
	rm -rf doc html man || true

# End of file.
