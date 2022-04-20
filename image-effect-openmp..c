#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

// Declare STB_Image library

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define CHANNEL_NUM 1

int main(int argc, char *argv[] ) {
    
    // Validate number of parameters for program execution

    if(argc < 4){
    	printf("Usage : <img_in_name> <img_out_name> <number_of_threads>");
    	return -1;
    }

    // Declare filter matrix, information variables for image and threads  
 
    int width, height, bpp;
    int sobel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int sobel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    int number_threads = atoi(argv[3]);
    int thread_id[number_threads];

    // Load image 

    uint8_t* image = stbi_load(argv[1], &width, &height, &bpp, 1);
    printf("Loaded image with height %d and width %d \n", width, height);

    // Use malloc to reserve memory for ouput image

    uint8_t* edge_image;
    edge_image = malloc(width*height);

    // Threads Logic with openMP

    #pragma omp parallel num_threads(number_threads)
    {
        int thread_id = omp_get_thread_num();
        
        int amount_work_y = (int)((height)/number_threads);
        int amount_work_x = (int)((width)/number_threads);

        for(int x= (thread_id*amount_work_x); x< (thread_id*amount_work_x)+amount_work_x; x++){
    		for(int y= 1 ; y < height-2; y++){
    		int pixel_x = ((sobel_x[0][0]*image[x*height-1+y-1])+ (sobel_x[0][1]* image[x*height+y-1]) + (sobel_x[0][2] * image[x*height+1+y-1]))+
    			      ((sobel_x[1][0]*image[x*height-1+y])+ (sobel_x[1][1]* image[x*height+y]) + (sobel_x[1][2] * image[x*height+1+y]))+
    			      ((sobel_x[2][0]*image[x*height-1+y+1])+ (sobel_x[2][1]* image[x*height+y+1]) + (sobel_x[2][2] * image[x*height+1+y+1]));
    			      
    	    int pixel_y = ((sobel_y[0][0]*image[x*height-1+y-1])+ (sobel_y[0][1]* image[x*height+y-1]) + (sobel_y[0][2] * image[x*height+1+y-1]))+
    			      ((sobel_y[1][0]*image[x*height-1+y])+ (sobel_y[1][1]* image[x*height+y]) + (sobel_y[1][2] * image[x*height+1+y]))+
    			      ((sobel_y[2][0]*image[x*height-1+y+1])+ (sobel_y[2][1]* image[x*height+y+1]) + (sobel_y[2][2] * image[x*height+1+y+1]));
    		int val = ceil(sqrt((pixel_x*pixel_x)+(pixel_y*pixel_y)));
    		edge_image[x*height+y] = val;	
    	}
    }

    }

    printf("Sobel edge filter was sucessfully applied to %s \n", argv[1]);
    
    // Memory free for image and size

    stbi_image_free(image);    
    free(edge_image);

    // Write new PNG image

    stbi_write_png(argv[2], width, height, CHANNEL_NUM, edge_image, width*CHANNEL_NUM);
    
    return 0;
}