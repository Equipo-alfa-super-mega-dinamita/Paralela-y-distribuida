#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h> 
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"
#define MAXTASKS 8
int k;
int w, h, channels;
unsigned char * img;
unsigned char * blur;
int thread_num;
int kS;
int mirror(int a, int N){
    if(a < 0) return -a - 1;
    else if(a > N - 1) return N + N - 1 - a;
    else return a;    
}
int main(int argc,char *argv[]){
    int n_tasks, iam;
    //Lectura de argumentos -------
    char *inputFile = argv[1];
    char *outputFile = argv[2];
    kS = atoi(argv[3]); //Kernel size :)
    k = (kS - 1) / 2;
    MPI_Status status;
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n_tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &iam);

    //Lectura de la imagen ----------
    img = stbi_load(inputFile, &w, &h, &channels, 0);
    if(img == NULL){
        printf("Error leyendo imagen");
        exit(1);
    }

    int chunk_size = (int) ceil(h/n_tasks);
    int start = chunk_size * iam;
    int end = start + chunk_size - 1 > w - 1? w - 1: start + chunk_size - 1;
    
    if(iam == n_tasks-1) end = h;



    //Modificación imagen a una nueva
    size_t img_size = w * chunk_size * channels;    
    blur = malloc(img_size);

    
    if(blur == NULL){
        printf("Error creando nueva imagen (malloc blur) (F)");
        exit(1);
    }
    
    for (int cy = start; cy < end; cy ++){
      for (int cx = 0; cx <= w; cx ++){
        
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
            *(blur + channels*( cx + (cy - start) * w ))       = R/kS;
            *(blur + channels*( cx + (cy - start) * w ) + 1)   = G/kS;
            *(blur + channels*( cx + (cy - start) * w ) + 2)   = B/kS;
        }
    }

    
    //Escritura imagen nueva    
    unsigned char * final_img = (unsigned char *) malloc(img_size*n_tasks);
       
    MPI_Gather((void *) blur, img_size, MPI_CHAR, final_img, img_size, MPI_CHAR, 0, MPI_COMM_WORLD);

    if(iam == 0){            
      stbi_write_jpg(outputFile, w, h, channels, final_img, 100);   
      free(final_img);   
    }

    stbi_image_free(img);
    MPI_Finalize();
    return 0;
}
