CC?=gcc
CFLAGS?=`sdl-config --cflags --libs` -std=c99 -O2

grabber: main.c
	$(CC) $< -o $@ $(CFLAGS)

clean:
	rm -f *~ grabber
