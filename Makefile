OFILES = main.o map.o event.o page.o

a.out: $(OFILES)
	$(CC) -o $@ $(OFILES)
