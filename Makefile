.PHONY: clean

TARG = a.out
OFILES = src/main.o \
	src/reader.o \
	src/map/map.o \
	src/map/event.o \
	src/map/page.o \
	src/command.o \
	src/route.o \
	src/commonevent.o \
	src/enumstr.o \
	src/database.o

# incomplete for now
HFILES = src/database.h \
	src/command.h

$(TARG): $(OFILES)
	$(CC) $(CFLAGS) -o $@ $(OFILES)

src/enumstr.c: gen/enumstr.awk $(HFILES)
	awk -f gen/enumstr.awk $(HFILES) > $@

src/sjistab.c: gen/sjistab.awk res/SHIFTJIS.TXT
	awk -f gen/sjistab.awk res/SHIFTJIS.TXT > $@

clean:
	rm -f $(TARG) $(OFILES) src/sjistab.c
