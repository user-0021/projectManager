RM = cmd.exe /C del

#build executable file
build:pmn.exe

#make objs
obj\linear_list.o: src\linear_list.c
	gcc -o obj\linear_list.o src\linear_list.c  -I include -c

obj\main.o: src\main.c
	gcc -o obj\main.o src\main.c  -I include -c

pmn.exe: obj\linear_list.o obj\main.o
	gcc -o pmn.exe  obj\linear_list.o obj\main.o

all: clean pmn.exe

clean:
	$(RM) pmn.exe obj\linear_list.o obj\main.o