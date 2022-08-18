# Makefile

CC = g++
LIB=./lib/cximage/
CFLAGS = -Wall -c -g -DWIN32 -lwsock32 -lGdi32 -lws2_32 -lwinspool -ljpeg -L$(LIB) -lcximage -I ./include
allcflags=-lwsock32 -lws2_32 -lGdi32 -lwinspool -L$(LIB) -lcximage -ltiff -ldcr -ljasper -ljbig -ljpeg -lmng -lpng -lpsd -lzlib


all: speeder
# $(CC) -Wl,--subsystem,windows -o server server.o Socket.o error.o SocketStream.o HttpSocket.o -lwsock32  -lws2_32 -lgdi32
speeder: speeder.o HttpSocket.o Socket.o error.o SocketStream.o printer.o rc
	$(CC) -o speeder speeder.o Socket.o error.o SocketStream.o HttpSocket.o printer.o resource.o $(allcflags)
# $(CC) -mwindows -o speeder speeder.o Socket.o error.o SocketStream.o HttpSocket.o -lwsock32  -lws2_32 -lgdi32  
speeder.o: speeder.cpp HttpSocket.h
	$(CC) $(CFLAGS) speeder.cpp 
Socket.o: Socket.cpp Socket.h package.h SocketStream.h error.h
	$(CC) $(CFLAGS) Socket.cpp
HttpSocket.o: HttpSocket.h HttpSocket.cpp 
	$(CC) $(CFLAGS) HttpSocket.cpp
error.o: error.cpp error.h
	$(CC) $(CFLAGS) error.cpp
SocketStream.o: SocketStream.cpp SocketStream.h
	$(CC) $(CFLAGS) SocketStream.cpp
printer.o: printer.cpp printer.h
	$(CC) $(CFLAGS) printer.cpp
rc:
	windres -i resource.rc -o resource.o
clean:
	rm *.o 

checkprinter: checkprinter.o printer.o error.o TimeUtils.o
	$(CC) -o cprinter checkprinter.o printer.o TimeUtils.o -lGdi32 -lwinspool

checkprinter.o: checkprinter.cpp
	$(CC) -Wall -c -g -DWIN32 -lGdi32 -lwinspool -I ./include checkprinter.cpp

TimeUtils.o: TimeUtils.cpp TimeUtils.h
	$(CC) -Wall -c -g -DWIN32 -lGdi32 -lwinspool -I ./include TimeUtils.cpp