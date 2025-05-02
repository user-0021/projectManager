#build executable file
build:pmn

#make objs
obj:
	@mkdir obj

obj/tokenizer.o: src/tokenizer.c
	gcc -o obj/tokenizer.o src/tokenizer.c -I include -I library -c

obj/main.o: src/main.c
	gcc -o obj/main.o src/main.c -I include -I library -c

obj/init.o: src/commands/init.c
	gcc -o obj/init.o src/commands/init.c -I include -I library -c

obj/build.o: src/commands/build.c
	gcc -o obj/build.o src/commands/build.c -I include -I library -c

obj/linear_list.o: library/linear_list/linear_list.c
	gcc -o obj/linear_list.o library/linear_list/linear_list.c -I include -I library -c

obj/stringLib.o: library/stringLib/stringLib.c
	gcc -o obj/stringLib.o library/stringLib/stringLib.c -I include -I library -c

pmn: obj obj/tokenizer.o obj/main.o obj/init.o obj/build.o obj/linear_list.o obj/stringLib.o
	gcc -o pmn obj/tokenizer.o obj/main.o obj/init.o obj/build.o obj/linear_list.o obj/stringLib.o

all: clean pmn

clean:
	$(RM) pmn obj/tokenizer.o obj/main.o obj/init.o obj/build.o obj/linear_list.o obj/stringLib.o