.PHONY: clean

TARG = libwolfrpg.a
OFILES = reader.o \
	map.o \
	command.o \
	route.o \
	commonevent.o \
	enumstr.o \
	database.o

# incomplete for now
HFILES = database.h \
	command.h

$(TARG): $(OFILES)
	ar cr $@ $(OFILES)

enumstr.c: _enumstr.c
_enumstr.c: enumstr.awk $(HFILES)
	awk -f enumstr.awk $(HFILES) > $@

clean:
	rm -f $(TARG) $(OFILES) _enumstr.c
