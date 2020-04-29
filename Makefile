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
	$(RM) 100-0.txt 100-0.txt.md5

100-0.txt:
	@echo "Downloading the complete works of William Shakespeare..."
	@wget --quiet http://www.gutenberg.org/files/100/100-0.txt
	@md5sum $@ > $@.md5

check: $(EXE) 100-0.txt
	@$(RM) out
	@./fastcat 100-0.txt > out
	@diff 100-0.txt out
	@./fastcat 100-0.txt | md5sum --status -c 100-0.txt.md5
	@echo "OK!"

bench: $(EXE) simple 100-0.txt
	@printf "simple: "
	@./simple simple 100-0.txt | pv -r > /dev/null
	@bench './simple 100-0.txt'
	@printf "cat from coreutils: "
	@cat simple 100-0.txt | pv -r > /dev/null
	@bench 'cat 100-0.txt'
	@printf "fastcat: "
	@./fastcat simple 100-0.txt | pv -r > /dev/null
	@bench './fastcat 100-0.txt'

.PHONY: clean
