#build executable file
build:pmn 

#make objs
obj:
	@mkdir obj

obj/default_src_tokenizer.o:src/tokenizer.c
	gcc -o obj/default_src_tokenizer.o src/tokenizer.c -I include -I library -c

obj/default_src_main.o:src/main.c
	gcc -o obj/default_src_main.o src/main.c -I include -I library -c

obj/default_src_commands_init.o:src/commands/init.c
	gcc -o obj/default_src_commands_init.o src/commands/init.c -I include -I library -c

obj/default_src_commands_build.o:src/commands/build.c
	gcc -o obj/default_src_commands_build.o src/commands/build.c -I include -I library -c

obj/default_library_stringLib_stringLib.o:library/stringLib/stringLib.c
	gcc -o obj/default_library_stringLib_stringLib.o library/stringLib/stringLib.c -I include -I library -c

obj/default_library_linear_list_linear_list.o:library/linear_list/linear_list.c
	gcc -o obj/default_library_linear_list_linear_list.o library/linear_list/linear_list.c -I include -I library -c

pmn:obj obj/default_src_tokenizer.o obj/default_src_main.o obj/default_src_commands_init.o obj/default_src_commands_build.o obj/default_library_stringLib_stringLib.o obj/default_library_linear_list_linear_list.o 
	gcc -o pmn obj/default_src_tokenizer.o obj/default_src_main.o obj/default_src_commands_init.o obj/default_src_commands_build.o obj/default_library_stringLib_stringLib.o obj/default_library_linear_list_linear_list.o 

all: clean pmn obj/default_src_tokenizer.o obj/default_src_main.o obj/default_src_commands_init.o obj/default_src_commands_build.o obj/default_library_stringLib_stringLib.o obj/default_library_linear_list_linear_list.o 

clean:
	$(RM) obj/*.o pmn 