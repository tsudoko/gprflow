OFILES = src/main.o \
	src/map/reader.o \
	src/map/map.o \
	src/map/event.o \
	src/map/page.o

a.out: $(OFILES)
	$(CC) -o $@ $(OFILES)
