
all: bwtencode bwtsearch

bwtencode: main.c
	gcc main.c -o bwtencode

bwtsearch: decode.c
	gcc decode.c -lm -o bwtsearch

clean:
	rm bwtencode bwtsearch