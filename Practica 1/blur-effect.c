#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

int main(int argc,char *argv[]){
    //Lectura de argumentos -------
    char *inputFile = argv[1];
    char *outputFile = argv[2];
    int kernelSize = atoi(argv[3]);
    int threadNum = atoi(argv[4]);

/*
    printf("Input file: %s \n",inputFile);
    printf("Output file: %s \n",outputFile);
    printf("Kernel size: %i \n",kernelSize);
    printf("Threads: %i \n",threadNum);
*/

    //Lectura de la imagen ----------
    int width,height,channels;
    unsigned char *img = stbi_load(inputFile, &width, &height, &channels, 0);
    if(img == NULL){
        printf("Error leyendo imagen");
        exit(1);
    }
    printf("Image read: %s - Width: %i ,Height: %i ,Channels: %i\n",inputFile,width,height,channels);

    //Modificación imagen a una nueva
    size_t img_size = width * height * channels;

    unsigned char* newImage = malloc(img_size);
    if(newImage == NULL){
        printf("Error creando nueva imagen (malloc) (F)");
        exit(1);
    }

    //Conversión a escala de grises - Cada pixel tiene el valor promedio de sus componentes en todos
    for(unsigned char *p = img, *pg = newImage; p != img + img_size; p += channels, pg += channels) {
        uint8_t prom = (uint8_t)((*p + *(p + 1) + *(p + 2))/3.0);
        *pg = prom;
        *(pg+1) = prom;
        *(pg+2) = prom;

    }

    //Escritura imagen nueva
    stbi_write_jpg(outputFile, width, height, channels, newImage, 100);
    stbi_image_free(img);

    return 0;
}