OUTPUTS = downloadManager
CC = gcc

all: $(OUTPUTS)

clean:
	rm -f $(OUTPUTS)
	rm -f *.o

.PHONY: all clean

%.o: %.c
	$(CC) -c $<

downloadManager: downloadManager.o
	$(CC) -o $@ $^ -lcurl -lpthread
