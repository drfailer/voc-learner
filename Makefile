CC=gcc
CFLAGS=-Wall -Wextra

voc-learner: voc-learner.c
	$(CC) $(CFLAGS) -o $@ $^

debug: voc-learner.c
	$(CC) $(CFLAGS) -g -o $@ $^
