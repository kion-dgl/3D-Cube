all:
	gcc -c -o lib/dashgl.o lib/dashgl.c -lGL -lGLEW -lm
	gcc main.c lib/dashgl.o -lGL -lGLEW -lglut -lm

run:
	./a.out

clean:
	rm a.out
	rm lib/dashgl.o
