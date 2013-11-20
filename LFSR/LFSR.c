/*
 * LFSR.c
 *
 *  Created on: Oct 24, 2013
 *      Author: biniam fisseha
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "Bunny24.h"
#include "LFSR.h"

int lfsr(int poly, int init_state, int out_len, int len)
{
	int vector = init_state, dec=0;
	int i, j, p, v, k, xor, xored=0, first=1;

	v = init_state;
	for (i=0, k=out_len; i<out_len; i++)
	{
		p = poly;
		j = len;
		while (j-- > 0){
			if ((p & 1) == 1) {
				xored = 1;
				if (first == 1) {
					xor = v & 1;
					first = 0;
				}
				else
					xor = xor ^ (v & 1);
			}
			p >>= 1;
			v >>= 1;
		}

		vector >>= 1;
		if (xored == 1)
		{
			vector |= xor << (len-1);
			xored = 0;

			xor <<= k-1;
			dec |= xor;
			k--;
		}

		v = vector;
		j = len;
		first = 1;
	}
	printf("%d\n", dec);
	return dec;
}

// 0000001 1 1 0 0 1 1
/*
void lfsr(int *poly, int *init_state, int out_len, int len)
{
	int *vector = init_state;
	int i, j, xor, xored=0, first=1;
	int *res = (int*)malloc(sizeof(int)*len);

	for (i=0; i<out_len; i++) {
		for (j=0; j<len; j++) {
			if(poly[j] == 1) {
				xored = 1;
				if (first == 1) {
					xor = vector[j];
					first = 0;
				}
				else
					xor = xor ^ vector[j];
			}
		}

		shift_right(vector, len, res);
		if (xored == 1) {
			res[0] = xor;
			xored = 0;
		}

		for (j=0; j<len; j++)
			vector[j] = res[j];

		printf(" ");
		for (j=0; j<len; j++) {
			printf("%d", vector[j]);
		}
		first = 1;
	}
}

*/




