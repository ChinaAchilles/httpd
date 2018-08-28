LDFLAGS=-lpthread
httpd:httpd.c
	gcc -o $@ $^ $(LDFLAGS)
.PHONY:clean
clean:
	rm -f bin
