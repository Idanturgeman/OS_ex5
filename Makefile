.PHONY: all clean
CXX=gcc

all: server client
#server: Ex5.hpp server.o 
server: server.o 
	$(CXX) server.o -o server
#client: client.o Ex5.hpp
client: client.o 
	$(CXX) client.o -o client
# test: test.o Ex4.hpp
# 	$(CXX) test.o -o test -lpthread -ltbb
#server.o: server.cpp Ex5.hpp
server.o: server.cpp 
	$(CXX) -c server.cpp
#client.o: client.cpp Ex5.hpp
client.o: client.cpp 
	$(CXX) -c client.cpp
# test.o: test.cpp Ex5.hpp
# 	$(CXX) -c test.cpp

clean :
	rm -f *.o client server foo1.txt