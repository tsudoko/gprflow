.PHONY: clean

TARG = libwolfrpg.a
OFILES = src/reader.o \
	src/map.o \
	src/command.o \
	src/route.o \
	src/commonevent.o \
	src/enumstr.o \
	src/database.o

# incomplete for now
HFILES = src/database.h \
	src/command.h

$(TARG): $(OFILES)
	ar cr $@ $(OFILES)

src/enumstr.c: src/_enumstr.c
src/_enumstr.c: gen/enumstr.awk $(HFILES)
	awk -f gen/enumstr.awk $(HFILES) > $@

clean:
	rm -f $(TARG) $(OFILES) src/_enumstr.c
