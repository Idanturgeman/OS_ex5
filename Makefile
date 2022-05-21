all: server client
server: Ex5.hpp server.o 
	gcc server.o -o server
client: client.o Ex5.hpp
	gcc client.o -o client
server.o: server.cpp Ex5.hpp
	gcc -c server.cpp
client.o: client.cpp Ex5.hpp
	gcc -c client.cpp

clean :
	rm -f *.o client server foo1.txt

.PHONY: all clean