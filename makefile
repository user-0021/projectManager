RM = rm -f

#build cutable file
build:pmn 

#make objs
obj/linear_list.o: src/linear_list.c
	gcc  -o obj/linear_list.o src/linear_list.c  -I include -c

obj/main.o: src/main.c
	gcc  -o obj/main.o src/main.c  -I include -c

pmn: obj/linear_list.o obj/main.o
	gcc  -o pmn  obj/linear_list.o obj/main.o 

all: clean pmn 

clean:
	$(RM) pmn  obj/linear_list.o obj/main.o
