RM = rm

#build executable file
build:pmn

#make objs
obj/main.o: src/main.c
	gcc -o obj/main.o src/main.c  -I include -c

pmn: obj/main.o
	gcc -o pmn  obj/main.o

all: clean pmn

clean:
	$(RM) pmn obj/main.o