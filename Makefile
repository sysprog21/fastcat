override CFLAGS := -Wall -pedantic -std=c99 -O2 $(CFLAGS)
override LDFLAGS := $(LDFLAGS)

EXE = fastcat

all: $(EXE) simple

$(EXE): $(EXE).c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

simple: simple.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	$(RM) fastcat simple out

distclean: clean
	$(RM) 100-0.txt

100-0.txt: 
	wget --quiet http://www.gutenberg.org/files/100/100-0.txt
	md5sum $@

check: $(EXE) 100-0.txt
	@$(RM) out
	@./fastcat 100-0.txt > out
	@diff 100-0.txt out
	@echo "OK!"

bench: $(EXE) simple 100-0.txt
	bench './simple 100-0.txt'
	bench 'cat 100-0.txt'
	bench './fastcat 100-0.txt'

.PHONY: clean
