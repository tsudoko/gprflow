.PHONY: all clean

#TARG = iconv.a
OFILES = iconv.o

all: iconv.o
#$(TARG): $(OFILES)

iconv.c: _iconv.c
_iconv.c: iconv.awk SHIFTJIS.TXT
	awk -f iconv.awk SHIFTJIS.TXT > $@

clean:
	rm -f $(TARG) $(OFILES) _iconv.c
