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

// N: numero de linhas = 300
// M: numero de colunas = 240
int resize_map(double *map, char *map_opt, int row, int col)
{
    int i,j,k, col_opt;
    unsigned char byte;
	
	col_opt=(col+(col%8))/8;
	
	int index=0,pos=0;

    for(i = 0 ; i < row; i++)
    {
        for(j = 0 ; j < col; j=j+8)
        {
			byte = 0;
			for(k = 0 ; k <=7 ; k++)
			{
				if((j+k)>=col)
				{
					byte = (byte << 1);
				}
				else
				{
					byte = (byte << 1) | (int)map[index];
				}
				//printf("%d ", index);
				//printf("%u ", byte);
				index++;				
			}
			
			pos=(i*col_opt)+(j/8);
			//printf("%d ", pos);
			
			map_opt[pos]= (char)byte;			
			//printf("%d ", (int)map_opt[pos]);
			
			//printf("\n");			
		}
	//	printf("\n");
    }
    return col_opt;
}
