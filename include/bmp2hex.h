/*
  bmp2hex - Converts a 1-bit/pixel image in a BMP image file into hexadecimal
  data for use in initializing a C or assembler program data array.

  Only works on 1-bit/pixel images, and probably not on all of those.

  Allan Weber, USC Dept. of Electrical Engineering, 11/7/06
 
*/

/*
  Le bitmap binario (1 bit per pixel) e gera hex file onde cada byte representa 8 pixels BW.
  o bitmap de entrada deve ser tal que 0x00 representa vazio, 0xff representa parede.

  Para gerar um bitmap binario use um editor de bitmap qualquer. 
  No Ubuntu tem o GPaint, no Windows tem o Paint.
  Depois use o GIMP para converter o bitmap para 1bpp.
  No GIMP, vá no menu Image -> Mode -> Indexed ...
  Selecione a opcao 'Use Black and White 1bit pallete'
  
  Programa baseado nos exemplos: 
  http://ee-classes.usc.edu/ee459/library/plaintext.php?file=programs/bmp2hex.c
  http://stackoverflow.com/questions/14597043/converting-1-bit-bmp-file-to-array-in-c-c
 
  Para compilar: 
  $ gcc bmp2hex_2.c -o bmp2hex
 
  Amory - 17/Junho/2015
 */


#include <stdio.h>
#include <stdlib.h>

/*
 * fname - input file name in bmp (1bpp)
 * _w,   - image width in pixels
 * _h,   - image height in pixels
 * _n,   - array size in bytes
 * invert - 0 if 0x00 is white
 * 
 */
unsigned char *read_bmp(char *fname,int* _w, int* _h, int* _n, int invert)
{
    int offset, width, height, bitcount;
    int n, widthbytes, padbytes;
    unsigned char *img, *p;
    int row, col, ch;
    extern int optind;
    extern char *optarg;
    unsigned char fileheader[14];
    unsigned char infoheader[40];

	// input file 1bpp .bmp
	printf("reading file %s ...", fname);
    FILE *f = fopen(fname,"rb");

    if (f == NULL) {
    	printf("Error: could open file %s\n", fname);
    	return NULL;
    }
    printf(" Done !\n");
    
    if (fread(fileheader, 1, 14, f) != 14) {
		fprintf(stderr, "Error reading fileheader\n");
		exit(1);
    }
    if (strncmp(fileheader, "BM", 2) != 0) {
		fprintf(stderr, "File is not BMP format\n");
		exit(1);
    }
    if (fread(infoheader, 1, 40, f) != 40) {
		fprintf(stderr, "Error reading fileheader\n");
		exit(1);
    }

    /* Get some numbers from the headers */
    offset = get4(fileheader + 10);
    width  = get4(infoheader + 4);
    height = get4(infoheader + 8);
    bitcount = get2(infoheader + 14);
	
//#ifdef DEBUG
    printf("offset=%d, width=%d, height=%d, bitcount=%d\n",
	   offset, width, height, bitcount);
//#endif

    /* Skip over any color table */
    n = offset - (14 + 40);
    if (n > 0)
		fseek(f, n, SEEK_CUR);

    /* Check to make sure it's 1-bit per pixel */
    if (bitcount != 1) {
		fprintf(stderr, "Unsupported bit depth - %d\n", bitcount);
		exit(1);
    }

    widthbytes = (width + 7) / 8; /* number of data bytes per line */
    padbytes = ((width + 31) / 32) * 4; /* data+pad bytes per line */

//#ifdef DEBUG
    printf("widthbytes=%d, padbytes=%d\n", widthbytes, padbytes);
//#endif

    /* Allocate memory for image data */
    n = padbytes * height;
    if ((img = malloc(n)) == NULL) {
		fprintf(stderr, "Can\'t allocate memory\n");
		exit(1);
    }

    /* Read in image data */
    if (fread(img, 1, n, f) != n) {
		fprintf(stderr, "Error reading image data\n");
		exit(1);
    }

/*
#ifdef DEBUG
    // Loop through image data and print it out
    for (row = height-1; row >= 0; row--) {
		p = img + (padbytes * row);
		for (col = 0; col < widthbytes; col++) {
			ch = *p++;
			if (invert) 
			   ch ^= 0xff;
			printf("0x%02x, ", ch);
		}
		printf("\n");
    }
#endif
*/
    
    fclose(f);
    *_w = width; *_h = height; *_n = n;
    return img;
    
}

/* Turn an Intel 4-byte quantity into an 32-bit value */
int get4(unsigned char *p)
{
    int x;
    x = (*(p+3) << 24) + (*(p+2) << 16) + (*(p+1) << 8) + (*p);
    return(x);
}

/* Turn an Intel 2-byte quantity into an 32-bit value */
int get2(unsigned char *p)
{
    int x;
    x = (*(p+1) << 8) + (*p);
    return(x);
}

