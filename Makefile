CC = gcc
CFLAGS= -std=c99 -Wall -pedantic -g3 

all: Heatmap Unit

Heatmap: heatmap.c track.o trackpoint.o location.o 
	${CC} ${CFLAGS} -lm -o Heatmap heatmap.c track.o trackpoint.o location.o 

Unit: track_unit.c track.o trackpoint.o location.o
	${CC} ${CFLAGS} -lm -o Unit track_unit.c track.o trackpoint.o location.o


track.o: track.c track.h
	${CC} ${CFLAGS} -c track.c

trackpoint.o: trackpoint.c trackpoint.h
	${CC} ${CFLAGS} -c trackpoint.c

location.o: location.c
	${CC} ${CFLAGS} -c location.c

clean:
	rm -r *.o Unit vgcore.*