OUTPUTS = downloadManager

all: $(OUTPUTS)

clean:
	rm -f $(OUTPUTS)
	rm -f *.o

.PHONY: all clean

%.o: %.c
	$(CC) -c -o $@ $<

%: %.o
	$(CC) -o $@ $^
