#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

int k;
int w, h, channels;
unsigned char *img;
unsigned char* blur;
int threadNum;
int kS;

int mirror(int a, int N){
    if(a < 0) return -a - 1;
    else if(a > N - 1) return N + N - 1 - a;
    else return a;    
}

void *calcColumn(void *arg){
    int threadId = *(int *)arg;
    int start = (w/threadNum) * threadId;
    int end = start + ((w/threadNum) - 1);
    if(threadId == threadNum-1) end = start + (w/threadNum);

    for (int cx = start; cx <= end; cx ++){
        for (int cy = 0; cy < h; cy ++){
            int R = 0;
            int G = 0;
            int B = 0;

            for( int j = -k; j<= k ; j++){
                
                int sR = 0;
                int sG = 0;
                int sB = 0;

                int ny = mirror(cy + j, h);
                
                for( int i = -k; i<= k ; i++){
                    
                    int nx = mirror(cx + i, w);
                    sR+= (uint8_t) *(img + channels*( nx + ny*w ));
                    sG+= (uint8_t) *(img + channels*( nx + ny*w ) + 1);
                    sB+= (uint8_t) *(img + channels*( nx + ny*w ) + 2);

                }
                R+= sR/kS;
                G+= sG/kS;
                B+= sB/kS;
            }

            *(blur + channels*( cx + cy*w ))       = R/kS;
            *(blur + channels*( cx + cy*w ) + 1)   = G/kS;
            *(blur + channels*( cx + cy*w ) + 2)   = B/kS;
        }
    }
    return 0;
}

int main(int argc,char *argv[]){
    //Lectura de argumentos -------
    char *inputFile = argv[1];
    char *outputFile = argv[2];
    //int kS; GLOBAL
    kS = atoi(argv[3]); //Kernel size
    //int threadNum; GLOBAL
    threadNum = atoi(argv[4]);

    //int k; GLOBAL
    k = (kS - 1) / 2;

    //Lectura de la imagen ----------
    //int w, h, channels; GLOBAL
    //unsigned char *img; GLOBAL
    img = stbi_load(inputFile, &w, &h, &channels, 0);
    if(img == NULL){
        printf("Error leyendo imagen");
        exit(1);
    }
    //printf("Image read: %s - Width: %i ,Height: %i ,Channels: %i\n",inputFile,w,h,channels);

    //Modificación imagen a una nueva
    size_t img_size = w * h * channels;
    
    //unsigned char* blur; GLOBAL
    blur = malloc(img_size);
    if(blur == NULL){
        printf("Error creando nueva imagen (malloc blur) (F)");
        exit(1);
    }
    
    //Vertical Blur
    /*for (int cx = 0; cx < w; cx ++){
        for (int cy = 0; cy < h; cy ++){
            int R = 0;
            int G = 0;
            int B = 0;

            for( int j = -k; j<= k ; j++){
                
                int sR = 0;
                int sG = 0;
                int sB = 0;

                int ny = mirror(cy + j, h);
                
                for( int i = -k; i<= k ; i++){
                    
                    int nx = mirror(cx + i, w);
                    sR+= (uint8_t) *(img + channels*( nx + ny*w ));
                    sG+= (uint8_t) *(img + channels*( nx + ny*w ) + 1);
                    sB+= (uint8_t) *(img + channels*( nx + ny*w ) + 2);

                }
                R+= sR/kS;
                G+= sG/kS;
                B+= sB/kS;
            }

            *(blur + channels*( cx + cy*w ))       = R/kS;
            *(blur + channels*( cx + cy*w ) + 1)   = G/kS;
            *(blur + channels*( cx + cy*w ) + 2)   = B/kS;
        }
    }*/

    int threadId[threadNum], i ,*retval;

    pthread_t thread[threadNum];

    for(i = 0; i < threadNum; i++){
        threadId[i] = i;
        pthread_create(&thread[i], NULL, (void *)calcColumn, &threadId[i]);    
    }

    for(i = 0; i < threadNum; i++){
        pthread_join(thread[i], (void **)&retval);
    }

    //Escritura imagen nueva
    stbi_write_jpg(outputFile, w, h, channels, blur, 100);
    stbi_image_free(img);
    return 0;
}