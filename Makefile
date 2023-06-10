
all: lighttoggle 

lighttoggle: lighttoggle.c
	$(CC) -Wall -Wextra lighttoggle.c -lhidapi-hidraw -o lighttoggle -g
	sudo ./lighttoggle
	

.PHONY: clean

clean:
	rm -f lighttoggle
