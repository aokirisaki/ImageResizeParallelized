#ifndef HOMEWORK_H
#define HOMEWORK_H

typedef struct {
	// G este ori green or gray in fct de ce tip de imagine este
	// daca e grayscale ignor R si B
	unsigned char **R, **G, **B;

}image;

void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

#endif /* HOMEWORK_H */