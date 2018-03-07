.PHONY: clean

TARG = a.out
OFILES = src/main.o \
	src/reader.o \
	src/map/map.o \
	src/map/event.o \
	src/map/page.o \
	src/database.o

$(TARG): $(OFILES)
	$(CC) $(CFLAGS) -o $@ $(OFILES)

src/sjistab.c: res/SHIFTJIS.TXT
	awk -f res/sjistabgen.awk $< > $@

clean:
	rm -f $(TARG) $(OFILES) src/sjistab.c
