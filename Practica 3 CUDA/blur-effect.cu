/****** BLUR effect *******/
#include <stdio.h>
#include <math.h>
// For the CUDA runtime routines (prefixed with "cuda_")
#include <cuda_runtime.h>
//For image loading and writing
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

/*****************************************************************************
/*kernel and device functions
*****************************************************************************/

__device__ int mirror(int a, int N){
    if(a < 0) return -a - 1;
    else if(a > N - 1) return N + N - 1 - a;
    else return a;
}


__global__ void blurKernel(unsigned char *blur, unsigned char *img, int width,int height,int channels,int kS, int totalThreads)
{       
 
  int chunkSize = (int)floorf(width/totalThreads);
  int index = (blockDim.x * blockIdx.x) + threadIdx.x;
  int start = chunkSize * index;
  int end = start + (chunkSize - 1);
  if(index == totalThreads-1) end = width;

  int k = (kS - 1) / 2;
 
  for (int cx = start; cx <= end; cx ++){
        for (int cy = 0; cy < height; cy ++){
            int R = 0;
            int G = 0;
            int B = 0;

            for( int j = -k; j<= k ; j++){
                
                int sR = 0;
                int sG = 0;
                int sB = 0;

                int ny = mirror(cy + j, height);
                
                for( int i = -k; i<= k ; i++){                    
                    int nx = mirror(cx + i, width);
                    //MODIFICAR, LECTURA DE LA IMAGEN
                    sR+= (uint8_t) *(img + channels*( nx + ny*width ));
                    sG+= (uint8_t) *(img + channels*( nx + ny*width ) + 1);
                    sB+= (uint8_t) *(img + channels*( nx + ny*width ) + 2);                
                }
                R+= sR/kS;
                G+= sG/kS;
                B+= sB/kS;
            }
            //MODIFICAR, ESCRITURA DE LA IMAGEN
            *(blur + channels*( cx + cy*width ))       = R/kS;
            *(blur + channels*( cx + cy*width ) + 1)   = G/kS;
            *(blur + channels*( cx + cy*width ) + 2)   = B/kS;
        }
    }
  
}


/******************************************************************************/
/*Host code
*******************************************************************************/

int main(int argc,char *argv[])
{   
    /*device info*/
    int maxNumTh;
    int numBlocks;
    int device;   
    cudaGetDevice ( &device );
    cudaDeviceGetAttribute ( &maxNumTh, cudaDevAttrMaxThreadsPerBlock, device);
    cudaDeviceGetAttribute ( &numBlocks, cudaDevAttrMultiProcessorCount, device);
    printf("numTh: %i numBlocks: %i \n",maxNumTh,numBlocks);


    //****Lectura de argumentos***//
    char *inputFile = argv[1];
    char *outputFile = argv[2];
    int kS = atoi(argv[3]); //Kernel size
    //int threadNum; GLOBAL
    //threadNum = atoi(argv[4]);

    //****Lectura imagen y reserva de memoria en el host*****/
    unsigned char *img, *blur;
    int w,h,channels;
    img = stbi_load(inputFile, &w, &h, &channels, 0);
    if(img == NULL){
        printf("Error leyendo imagen");
        exit(1);
    }

    size_t img_size = w * h * channels;   //TAMAÑO IMAGEN 
    blur = (unsigned char *)malloc(img_size);
    if(blur == NULL){
        printf("Error creando nueva imagen (malloc blur) (F)");
        exit(1);
    }

    /****Reserva de espacio en dispositivo****/
    unsigned char *d_img, *d_blur;

    cudaError_t err = cudaSuccess;
    err = cudaMalloc((void **)&d_img,img_size);
    if(err != cudaSuccess){
        fprintf(stderr, "Error reservando memoria para img en dispositivo (codigo de error %s)!'n'",cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    err = cudaMalloc((void **)&d_blur,img_size);
    if(err != cudaSuccess){
        fprintf(stderr, "Error reservando memoria para img en dispositivo (codigo de error %s)!'n'",cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    /***Copiado de Img en dispositivo*****/
    err = cudaMemcpy(d_img,img,img_size,cudaMemcpyHostToDevice);
    if(err != cudaSuccess){
        fprintf(stderr, "Error copiando img a dispositivo (codigo de error %s)!'n'",cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    /****Lanzamiento de kernel*****/
    int NUMTHREADS = numBlocks*maxNumTh;
    int threadsPerBlock = NUMTHREADS/numBlocks;
    int totalThreads = numBlocks * threadsPerBlock;
    printf("CUDA kernel lanzado con %d blocks of %d threads Total: %i       ", numBlocks, threadsPerBlock, totalThreads  );
    blurKernel<<<numBlocks,threadsPerBlock>>>(d_blur,d_img,w,h,channels,kS,totalThreads);
    err = cudaGetLastError();
    if(err != cudaSuccess){
        fprintf(stderr, "Error lanzando blur kernel (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    /***Copia de Blur de dispositivo a host****/
    err = cudaMemcpy(blur,d_blur,img_size,cudaMemcpyDeviceToHost);
    if(err != cudaSuccess){
        fprintf(stderr, "Error copiando blur de dispositivo a host (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    /***Liberado de memoria de dispositivo***/
    err = cudaFree(d_img);
    if (err != cudaSuccess){
        fprintf(stderr, "Error liberando espacio de d_img (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }
    err = cudaFree(d_blur);
    if (err != cudaSuccess){
        fprintf(stderr, "Error liberando espacio de d_blur (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    /****Guardado de la imagen en archivo y liberado de memoria****/
    stbi_write_jpg(outputFile, w, h, channels, blur, 100);
    stbi_image_free(img);
    free(blur);

    err = cudaDeviceReset();
    if (err != cudaSuccess){
        fprintf(stderr, "Error reiniciando dispositivo! error=%s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }
        
    return 0;
}