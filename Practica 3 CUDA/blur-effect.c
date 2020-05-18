#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h> 

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

int main(int argc,char *argv[]){
    //Lectura de argumentos -------
    char *inputFile = argv[1];
    char *outputFile = argv[2];
    kS = atoi(argv[3]); //Kernel size
    threadNum = atoi(argv[4]); //Numero de hilos

    //int k; GLOBAL
    k = (kS - 1) / 2;

    //Lectura de la imagen ----------
    img = stbi_load(inputFile, &w, &h, &channels, 0);
    if(img == NULL){
        printf("Error leyendo imagen");
        exit(1);
    }

    //Modificación imagen a una nueva
    size_t img_size = w * h * channels;
    
    blur = malloc(img_size);
    if(blur == NULL){
        printf("Error creando nueva imagen (malloc blur) (F)");
        exit(1);
    }

    //omp_set_dynamic(0);
    omp_set_num_threads(threadNum);

    #pragma omp parallel
    #pragma omp for schedule(static)
    for (int cx = 0; cx <= w; cx ++){
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


    //Escritura imagen nueva
    stbi_write_jpg(outputFile, w, h, channels, blur, 100);
    stbi_image_free(img);
    return 0;
}