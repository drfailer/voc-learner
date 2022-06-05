CC=gcc
CFLAGS=-Wall -Wextra -g
SOURCES=voc-learner.c

voc-learner: $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

clean: clean
	rm voc-learner
