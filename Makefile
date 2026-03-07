KeyLogger: keyLogger.o keyMaps.o
	gcc keyLogger.o keyMaps.o -o KeyLogger

keyLogger.o: keyLogger.c keyLogger.h
	gcc -c keyLogger.c -o keyLogger.o

keyMaps.o: keyMaps.c keyMaps.h
	gcc -c keyMaps.c -o keyMaps.o

clean:
	rm -f keyLogger.o keyMaps.o