RM = cmd.exe /C del

#build executable file
build:pmn.exe

#make objs
obj\main.o: src\main.c
	gcc -o obj\main.o src\main.c  -I include -c

pmn.exe: obj\main.o
	gcc -o pmn.exe  obj\main.o

all: clean pmn.exe

clean:
	$(RM) pmn.exe obj\main.o