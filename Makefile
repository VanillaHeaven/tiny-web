CC = gcc

request_test: obj/request.o obj/rio.o
	$(CC) -o request_test -Iinclude test/request_test.c obj/request.o obj/rio.o -g

obj/request.o: obj
	$(CC) -o obj/request.o -Iinclude -c src/request.c -g

rio_test: obj/rio.o
	$(CC) -o rio_test -Iinclude test/rio_test.c obj/rio.o -g

obj/rio.o: obj
	$(CC) -o obj/rio.o -Iinclude -c src/rio.c -g

obj:
	mkdir obj

.PHONY:clean
clean:
	rm -rf obj
	rm -rf rio_test
	rm -rf request_test
