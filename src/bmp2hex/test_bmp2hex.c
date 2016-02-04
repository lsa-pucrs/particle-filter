/*
 tests bmp2hex function.
 
 usage: bmp2hex <input map in bitmap (1bit per pixel) format> <output map in hex format (1bit per pixel)>
 test files: map_1bpp.bmp and hospital_1bpp.bmp
 compile: gcc -I../include test_bmp2hex.c -o bmp2hex
 */
#include <stdio.h>
#include <stdlib.h>
#include "bmp2hex.h"

//#define DEBUG

int main(int argc, char *argv[])
{
    int w, h, n;
    int i, j, k;
    unsigned char* img;
    unsigned char byte;
    
    if(argc != 3){
		printf("\nUSAGE: bmp2hex <input map in bitmap (1bit per pixel) format> <output map in hex format (1bit per pixel)>\n\n");
		return 1;
	}
    
    // read bitmap
    img = read_bmp(argv[1], &w, &h,&n,0);
    if (img == NULL) {
    	printf("Error: could open .bmp file\n");
    	return 1;
    }
    
    // output file .hex
    FILE *f = fopen(argv[2],"wb");
    if (f == NULL) {
    	printf("Error: could save .hex file\n");
    	return 1;
    }
   fwrite(img , 1 , n , f);
   fclose(f);
    
#ifdef DEBUG
    for(j = h-1 ; j >= 0; j--)
    {
        for(i = 0 ; i < w/8 ; i++){
			byte = img[j * (w/8) + i];
			for(k = 7 ; k >= 0 ; k--){
				printf("%c ", (byte & (1<<k)) ? '1' : '0' );
			}
		}
        printf("\n");
    }
#endif

	printf( "Memory size occupied by image (bytes) : %d\n", n);
	printf( "Image size  %d x %d\n", w,h);
    free(img);
    return 0;
}

