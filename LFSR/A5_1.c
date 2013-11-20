/*
 * A5_1.c
 *
 *  Created on: Nov 17, 2013
 *      Author: biniam
 */

#include <stdio.h>
#include "A5_1.h"
#include "util.h"

void shift_right(int *vector, int len, int* res){
  int i; // [] 1 0 1 1 -> 0
  for (i=0; i<len-1; i++)
    res[i+1] = vector[i];
}

void A5_1_Cycle(int *input, int len, int *lfsr1, int *lfsr2, int *lfsr3)
{
	int lfsr1_1[19], lfsr2_2[22], lfsr3_3[23];

	int i, k, xor;
	for (i=0; i<len; i++){

		// R1
		xor = lfsr1[18] ^ lfsr1[17] ^ lfsr1[16] ^ lfsr1[13];
		xor = xor ^ input[i];
		shift_right(lfsr1, 19, lfsr1_1);
		lfsr1_1[0] = xor;

		for (k=0; k<19; k++)
			lfsr1[k] = lfsr1_1[k];

		// R2
		xor = lfsr2[20] ^ lfsr2[21];
		xor = xor ^ input[i];
		shift_right(lfsr2, 22, lfsr2_2);
		lfsr2_2[0] = xor;

		for (k=0; k<22; k++)
			lfsr2[k] = lfsr2_2[k];

		// R3
		xor = lfsr3[7] ^ lfsr3[20] ^ lfsr3[21] ^ lfsr3[22];
		xor = xor ^ input[i];
		shift_right(lfsr3, 23, lfsr3_3);
		lfsr3_3[0] = xor;

		for (k=0; k<23; k++)
			lfsr3[k] = lfsr3_3[k];

	}


}

void ir_clock(int *lfsr1, int *lfsr2, int *lfsr3, int len, int *output)
{
	int lfsr1_1[19], lfsr2_2[22], lfsr3_3[23];
	int maj = lfsr1[8] + lfsr2[10] +  lfsr3[10];
	int i, majority, xor, k, j=0;
	int out1, out2, out3;
	int size = len;

	if (size == 0) size = 100;

	for (i=0; i<size; i++)
	{

		if (maj <= 1)
			majority = 0;
		else
			majority = 1;

		out1 = lfsr1[18];
		if (lfsr1[8] == majority) {
			xor = lfsr1[18] ^ lfsr1[17] ^ lfsr1[16] ^ lfsr1[13];
			shift_right(lfsr1, 19, lfsr1_1);
			lfsr1_1[0] = xor;

			for (k=0; k<19; k++)
				lfsr1[k] = lfsr1_1[k];
		}

		out2 = lfsr2[21];
		if (lfsr2[10] == majority) {
			xor = lfsr2[20] ^ lfsr2[21];
			shift_right(lfsr2, 22, lfsr2_2);
			lfsr2_2[0] = xor;

			for (k=0; k<22; k++)
				lfsr2[k] = lfsr2_2[k];
		}

		out3 = lfsr3[22];
		if (lfsr3[10] == majority) {
			xor = lfsr3[7] ^ lfsr3[20] ^ lfsr3[21] ^ lfsr3[22];
			shift_right(lfsr3, 23, lfsr3_3);
			lfsr3_3[0] = xor;

			for (k=0; k<23; k++)
				lfsr3[k] = lfsr3_3[k];
		}

		if (len != 0)
			output[j++] = out1 ^ out2 ^ out3;

		maj = lfsr1[8] + lfsr2[10] +  lfsr3[10];
	}
}


void A5_1(int *key, int *keystream)
{
	int lfsr1[19] = {0};
	int lfsr2[22] = {0};
	int lfsr3[23] = {0};
	unsigned long k = 0x48C4A2E691D5B3F7; // key
	int f[22]; // frame bits 0xB2000

	HexToBin(key, k, 64);
	HexToBin(f, 0xB2000, 22);

	// key injection
	A5_1_Cycle(key, 64, lfsr1, lfsr2, lfsr3);
	// frame injection
	A5_1_Cycle(f, 22, lfsr1, lfsr2, lfsr3);
	// 100 irregular clocks with no output
	ir_clock(lfsr1, lfsr2, lfsr3, 0, NULL);
	// 228 irregular clocks with output stream
	ir_clock(lfsr1, lfsr2, lfsr3, 228, keystream);
}
