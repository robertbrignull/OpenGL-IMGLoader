all: IMGLoader

IMGLoader : PNGLoader.c TGALoader.c main.c
	gcc -o IMGLoader PNGLoader.c TGALoader.c main.c -lX11 -lGL -lGLU

clean:
	rm IMGLoader