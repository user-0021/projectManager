#build executable file
build:pmn

#make objs
obj:
	@mkdir obj

obj/main.o: src/main.c
	gcc -o obj/main.o src/main.c -I include -I library -c

obj/linear_list.o: library/linear_list/linear_list.c
	gcc -o obj/linear_list.o library/linear_list/linear_list.c -I include -I library -c

obj/stringLib.o: library/stringLib/stringLib.c
	gcc -o obj/stringLib.o library/stringLib/stringLib.c -I include -I library -c

pmn: obj obj/main.o obj/linear_list.o obj/stringLib.o
	gcc -o pmn obj/main.o obj/linear_list.o obj/stringLib.o

all: clean pmn

clean:
	$(RM) pmn obj/main.o obj/linear_list.o obj/stringLib.o