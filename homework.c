#include "homework.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>

int num_threads;
int resize_factor;
FILE* originalImg;
unsigned char imgType[5];
int width, height, maxval, numcolors;
image input, output;

void parseColor(image *img, unsigned char *buffer) {
	int i, j, k = 0;
	(*img).R = (unsigned char**) malloc(height * sizeof(unsigned char*));
	(*img).G = (unsigned char**) malloc(height * sizeof(unsigned char*));
	(*img).B = (unsigned char**) malloc(height * sizeof(unsigned char*));

	for(i = 0; i < height; i++) {
		(*img).R[i] = (unsigned char*) malloc(width);
		(*img).G[i] = (unsigned char*) malloc(width);
		(*img).B[i] = (unsigned char*) malloc(width);
	}

	for(i = 0; i < height; i++) {
		for(j = 0; j < width; j++) {
			(*img).R[i][j] = buffer[k];
			k++;
			(*img).G[i][j] = buffer[k];
			k++;
			(*img).B[i][j] = buffer[k];
			k++;

		}
	}
}

void parseGrey(image *img, unsigned char *buffer) {
	int i, j, k = 0;

	(*img).G = (unsigned char**) malloc(height * sizeof(unsigned char*));

	for(i = 0; i < height; i++) {
		(*img).G[i] = (unsigned char*) malloc(width);
	}

	for(i = 0; i < height; i++) {
		for(j = 0; j < width; j++) {
			(*img).G[i][j] = buffer[k];
			k++;
		}
	}
}

//color
void* threadFunctionColor(void *var) {
	int thread_id = *(int*)var;

	int intervalSize = ceil((float)height / num_threads / resize_factor);
	int low = thread_id * intervalSize;
	int high = fmin((thread_id + 1) * intervalSize, height / resize_factor);

	int l, k, i, j, mediaR, mediaG, mediaB;

		for(i = low; i < high; i++) {
		for(j = 0; j < (unsigned int)width / resize_factor; j++) {
			mediaR = mediaG = mediaB = 0;
			for(k = i * resize_factor; k < i * resize_factor + resize_factor; k++) {
				for(l = j * resize_factor; l < j * resize_factor + resize_factor; l++) {
					mediaR = mediaR + input.R[k][l];
					mediaG = mediaG + input.G[k][l];
					mediaB = mediaB + input.B[k][l];
				}
			}

			mediaR = (unsigned int)mediaR / (resize_factor * resize_factor);
			mediaG = (unsigned int)mediaG / (resize_factor * resize_factor);
			mediaB = (unsigned int)mediaB / (resize_factor * resize_factor);
			output.R[i][j] = (unsigned char)mediaR;
			output.G[i][j] = (unsigned char)mediaG;
			output.B[i][j] = (unsigned char)mediaB;
		}
	}
}

//alb negru
void* threadFunctionBW(void *var) {
	int thread_id = *(int*)var;

	int intervalSize = ceil((float)height / num_threads / resize_factor);
	int low = thread_id * intervalSize;
	int high = fmin((thread_id + 1) * intervalSize, height / resize_factor);

	int l, k, i, j, mediaG;

		for(i = low; i < high; i++) {
		for(j = 0; j < (unsigned int)width / resize_factor; j++) {
			mediaG = 0;
			for(k = i * resize_factor; k < i * resize_factor + resize_factor; k++) {
				for(l = j * resize_factor; l < j * resize_factor + resize_factor; l++) {
					mediaG = mediaG + input.G[k][l];
				}
			}

			mediaG = (unsigned int)mediaG / (resize_factor * resize_factor);
			output.G[i][j] = (unsigned char)mediaG;
		}
	}
}

//color 3
void* threadFunctionColor3(void *var) {
	int thread_id = *(int*)var;

	int intervalSize = ceil((float)height / num_threads / 3);
	int low = thread_id * intervalSize;
	int high = fmin((thread_id + 1) * intervalSize, height / 3);

	int l, k, i, j, mediaR, mediaG, mediaB;
	int gaussianKernel[3][3];
	gaussianKernel[0][0] =1;
	gaussianKernel[0][1] =2;
	gaussianKernel[0][2] =1;
	gaussianKernel[1][0] =2;
	gaussianKernel[1][1] =4;
	gaussianKernel[1][2] =2;
	gaussianKernel[2][0] =1;
	gaussianKernel[2][1] =2;
	gaussianKernel[2][2] =1;

		for(i = low; i < high; i++) {
		for(j = 0; j < (unsigned int)width / 3; j++) {
			mediaR = mediaG = mediaB = 0;
			for(k = i * 3; k < i * 3 + 3; k++) {
				for(l = j * 3; l < j * 3 + 3; l++) {
					mediaR = mediaR + input.R[k][l] * gaussianKernel[k % 3][l % 3];
					mediaG = mediaG + input.G[k][l] * gaussianKernel[k % 3][l % 3];
					mediaB = mediaB + input.B[k][l] * gaussianKernel[k % 3][l % 3];
				}
			}

			mediaR = (unsigned int)mediaR / 16;
			mediaG = (unsigned int)mediaG / 16;
			mediaB = (unsigned int)mediaB / 16;
			output.R[i][j] = (unsigned char)mediaR;
			output.G[i][j] = (unsigned char)mediaG;
			output.B[i][j] = (unsigned char)mediaB;
		}
	}
}

//alb negru 3
void* threadFunctionBW3(void *var) {
	int thread_id = *(int*)var;

	int intervalSize = ceil((float)height / num_threads / 3);
	int low = thread_id * intervalSize;
	int high = fmin((thread_id + 1) * intervalSize, height / 3);

	int l, k, i, j, mediaG;
	int gaussianKernel[3][3];
	gaussianKernel[0][0] =1;
	gaussianKernel[0][1] =2;
	gaussianKernel[0][2] =1;
	gaussianKernel[1][0] =2;
	gaussianKernel[1][1] =4;
	gaussianKernel[1][2] =2;
	gaussianKernel[2][0] =1;
	gaussianKernel[2][1] =2;
	gaussianKernel[2][2] =1;

		for(i = low; i < high; i++) {
		for(j = 0; j < (unsigned int)width / 3; j++) {
			mediaG = 0;
			for(k = i * 3; k < i * 3 + 3; k++) {
				for(l = j * 3; l < j * 3 + 3; l++) {
					mediaG = mediaG + input.G[k][l] * gaussianKernel[k % 3][l % 3];
				}
			}

			mediaG = (unsigned int)mediaG / 16;
			output.G[i][j] = (unsigned char)mediaG;
		}
	}
}

void readInput(const char * fileName, image *img) {

	originalImg = fopen(fileName, "rb");
	//fgets(imgType, 5, originalImg);
	fscanf(originalImg, "%s\n", imgType);

	fscanf(originalImg, "%d%d%d\n", &width, &height, &maxval);

	if(imgType[1] == '5') {
		numcolors = 1;
	} else {
		numcolors = 3;
	}

	unsigned char *buffer = (unsigned char*) malloc(height * width * numcolors);
	fread(buffer, height * width * numcolors, 1, originalImg);

	if(numcolors == 3) {
		parseColor(img, buffer);
	} else {
		parseGrey(img, buffer);
	}
}

void writeData(const char * fileName, image *img) {
	int i, j;
	FILE* out;
	out = fopen(fileName, "wb");

	fprintf(out, "%s\n", imgType);
	fprintf(out, "%d %d\n", (unsigned int)width /resize_factor, (unsigned int)height / resize_factor);
	fprintf(out, "%d\n", maxval);

	if(imgType[1] == '5') {
		for(i = 0; i < (unsigned int)height / resize_factor; i++) {
			for(j = 0; j < (unsigned int)width / resize_factor; j++) {
				fwrite(output.G[i] + j, 1, 1, out);
			}
		}
	} else {
		for(i = 0; i < (unsigned int)height / resize_factor; i++) {
			for(j = 0; j < (unsigned int)width / resize_factor; j++) {
				fwrite(output.R[i] + j, 1, 1, out);
				fwrite(output.G[i] + j, 1, 1, out);
				fwrite(output.B[i] + j, 1, 1, out);
			}
		}	
	}
}

void resize(image *in, image * out) {

	pthread_t tid[num_threads];
	int thread_id[num_threads], i;

	input = *in;
	
	output.R = (unsigned char**) malloc((unsigned int)height / resize_factor * sizeof(unsigned char*));
	output.G = (unsigned char**) malloc((unsigned int)height / resize_factor * sizeof(unsigned char*));
	output.B = (unsigned char**) malloc((unsigned int)height / resize_factor * sizeof(unsigned char*));

	for(i = 0; i < (unsigned int)height / resize_factor; i++) {
		output.R[i] = (unsigned char*) malloc((unsigned int)width / resize_factor);
		output.G[i] = (unsigned char*) malloc((unsigned int)width / resize_factor);
		output.B[i] = (unsigned char*) malloc((unsigned int)width / resize_factor);
	}

	for(i = 0;i < num_threads; i++)
		thread_id[i] = i;

	for(i = 0; i < num_threads; i++) {
		if(imgType[1] == '6') {
			if(resize_factor != 3) {
				pthread_create(&(tid[i]), NULL, threadFunctionColor, &(thread_id[i]));
			} else {
				pthread_create(&(tid[i]), NULL, threadFunctionColor3, &(thread_id[i]));
			}
		} else {
			if(resize_factor != 3) {
				pthread_create(&(tid[i]), NULL, threadFunctionBW, &(thread_id[i]));
			} else {
				pthread_create(&(tid[i]), NULL, threadFunctionBW3, &(thread_id[i]));
			}
		}
	}

	for(i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}

	*out = output;
}