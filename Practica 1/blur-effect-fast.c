#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

int mirror(int a, int N){
    if(a < 0) return -a - 1;
    else if(a > N - 1) return N + N - 1 - a;
    else return a;

    //if(a < 0) a = -a - 1;
    //else if(a > N - 1) a = N + N - 1 - a;
    
}

int main(int argc,char *argv[]){
    //Lectura de argumentos -------
    char *inputFile = argv[1];
    char *outputFile = argv[2];
    int kS = atoi(argv[3]); //Kernel size
    int threadNum = atoi(argv[4]);

    int k = (kS - 1) / 2;
/*  
    printf("Input file: %s \n",inputFile);
    printf("Output file: %s \n",outputFile);
    printf("Kernel size: %i \n",kernelSize);
    printf("Threads: %i \n",threadNum);
*/

    //Lectura de la imagen ----------
    int w, h, channels;
    unsigned char *img = stbi_load(inputFile, &w, &h, &channels, 0);
    if(img == NULL){
        printf("Error leyendo imagen");
        exit(1);
    }
    //printf("Image read: %s - Width: %i ,Height: %i ,Channels: %i\n",inputFile,w,h,channels);

    //Modificación imagen a una nueva
    size_t img_size = w * h * channels;

    unsigned char* blur0 = malloc(img_size);
    if(blur0 == NULL){
        printf("Error creando nueva imagen (malloc) (F)");
        exit(1);
    }
    
    unsigned char* blur = malloc(img_size);
    if(blur == NULL){
        printf("Error creando nueva imagen (malloc) (F)");
        exit(1);
    }
    
    //Vertical Blur
    for (int cx = 0; cx < w; cx ++){
        for (int cy = 0; cy < h; cy ++){
            int R = 0;
            int G = 0;
            int B = 0;

            for( int j = -k; j<= k ; j++){

                int ny = mirror(cy + j, h);

                R+= (uint8_t) *(img + channels*( cx + ny*w ));
                G+= (uint8_t) *(img + channels*( cx + ny*w ) + 1);
                B+= (uint8_t) *(img + channels*( cx + ny*w ) + 2);

            }

            *(blur0 + channels*( cx + cy*w ))       = R/kS;
            *(blur0 + channels*( cx + cy*w ) + 1)   = G/kS;
            *(blur0 + channels*( cx + cy*w ) + 2)   = B/kS;
        }
    }

    //Horizontal Blur on Vertical Blurred image
    for (int cx = 0; cx < w; cx ++){
        for (int cy = 0; cy < h; cy ++){
            int R = 0;
            int G = 0;
            int B = 0;

            for( int i = -k; i<= k ; i++){

                int nx = mirror(cx + i, w);

                R+= (uint8_t) *(blur0 + channels*( nx + cy*w ));
                G+= (uint8_t) *(blur0 + channels*( nx + cy*w ) + 1);
                B+= (uint8_t) *(blur0 + channels*( nx + cy*w ) + 2);

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