/*
 * vecAdd.cu
 *
 *  Created on: Jul 31, 2015
 *      Author: lsa
 */

#include <stdio.h>

__global__ void vecAdd(int *a, int *b, int *c){
	*c = *a + *b;
}

int main(){
	int a, b, c;					//Host side variables
	int *d_a, *d_b, *d_c;		//Device side variables
	int size = sizeof(int);

	//Allocate device copies of a, b and c
	cudaMalloc((void**)&d_a, size);
	cudaMalloc((void**)&d_b, size);
	cudaMalloc((void**)&d_c, size);

	//Allocate host side arrays
	a = 2;
	b = 7;

	//Copy inputs to device
	cudaMemcpy(d_a, &a, size, cudaMemcpyHostToDevice);
	cudaMemcpy(d_b, &b, size, cudaMemcpyHostToDevice);

	//Launch kernel
	vecAdd<<<1,1>>>(d_a, d_b, d_c);

	//Copy result back to host
	cudaMemcpy(&c, d_c, size, cudaMemcpyDeviceToHost);

	//Cleanup
	cudaFree(d_a); cudaFree(d_b); cudaFree(d_c);

	//Print
	printf("\nGPU result: %d\nCPU result: %d", c, a+b);

	return 0;
}
