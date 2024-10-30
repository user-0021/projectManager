#build executable file
build:pmn

#make objs
obj/main.o: src/main.c
	gcc -o obj/main.o src/main.c -I include -c

obj/linear_list.o: src/linear_list.c
	gcc -o obj/linear_list.o src/linear_list.c -I include -c

pmn: obj/main.o obj/linear_list.o
	gcc -o pmn obj/main.o obj/linear_list.o

all: clean pmn

clean:
	$(RM) pmn obj/main.o obj/linear_list.o