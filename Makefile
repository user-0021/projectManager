#build executable file
build:pmn

#make objs
obj/main.o: src/main.c
	gcc -o obj/main.o src/main.c -I include -I library/linear_list -c

obj/linear_list.o: library/linear_list/linear_list.c
	gcc -o obj/linear_list.o library/linear_list/linear_list.c -I include -I library/linear_list -c

pmn: obj/main.o obj/linear_list.o
	gcc -o pmn obj/main.o obj/linear_list.o

all: clean pmn

clean:
	$(RM) pmn obj/main.o obj/linear_list.o