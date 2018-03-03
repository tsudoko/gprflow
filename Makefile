.PHONY: clean

TARG = a.out
OFILES = src/main.o \
	src/reader.o \
	src/map/map.o \
	src/map/event.o \
	src/map/page.o

$(TARG): $(OFILES)
	$(CC) -o $@ $(OFILES)

clean:
	rm -f $(TARG) $(OFILES)
