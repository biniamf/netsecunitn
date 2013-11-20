/*
 * Bunny24.c
 *
 *  Created on: Nov 15, 2013
 *      Author: biniam
 */

#include <stdio.h>
#include <string.h>
#include "Bunny24.h"
#include "field.h"

// lambda^-1
const int lambda_i[4][4] = {
  {0x1d, 0x3, 0xb, 0x19},
  {0x11, 0x2f, 0x3e, 0x3d},
  {0x7, 0x17, 0x39, 0xc},
  {0xa, 0x3a, 0xd, 0x29}
};

// lambda
const int lambda[4][4] = {
  {0x23, 0x3b, 0x38, 0x3d},
  {0x0d, 0x3c, 0x16, 0x18},
  {0x03, 0x20, 0x17, 0x37},
  {0x2c, 0x26, 0x38, 0x13}
};


int RoundFunction(int word)
{
	return 0;
}

int sbox1(int m)
{
	return pow_poly(m, 62);
}

int sbox2(int m)
{
	return pow_poly(m, 5);
}

int sbox3(int m)
{
	return pow_poly(m, 17);
}

int sbox4(int m)
{
	int e = 2; // primitive element
	return sum(pow_poly(m, 62), pow_poly(e, 2));
}
// m is 24-bit and returns 24-bit
int sbox_enc(int m)
{
	int sb=0, n;
	int e = 2; // primitive element

	// sbox1
	n = (m >> 18) & 63;
	n = pow_poly(n, 62);
	sb = n << 18;

	// sbox2
	n = (m >> 12) &  63;
	n = pow_poly(n, 5);
	sb += n << 12;

	// sbox3
	n = (m >> 6) & 63;
	n = pow_poly(n, 17);
	sb += n << 6;

	// sbox4
	n =  m & 63;
	n = sum(pow_poly(n, 62), pow_poly(e, 2));
	sb += n;

	return sb;
}

// m is 24-bit and returns 24-bit
int sbox_dec(int m)
{
	int sb=0, n;
	int e = 2; // primitive element

	// sbox1_inv
	n = (m >> 18) & 63;
	n = pow_poly(n, 62);
	sb = n << 18;

	// sbox2_inv
	n = (m >> 12) &  63;
	n = pow_poly(n, 38);
	sb += n << 12;

	// sbox3_inv
	n = (m >> 6) & 63;
	n = pow_poly(n, 26);
	sb += n << 6;

	// sbox4_inv
	n =  m & 63;
	n = pow_poly(sum(n, pow_poly(e, 2)), 62);
	sb += n;

	return sb;
}

// in: 24 bit key, out 16 24bit round keys
void KeySchedule(int key, int *out)
{
	int w[88] = {0};
	int j, round, idx;

	// step 1: w_-8 - w_-1
	w[0] = (key >> 18) & 63;
	w[1] = (key >> 12) & 63;
	w[2] = (key >> 6) & 63;
	w[3] = key & 63;


	w[4] = sum(sbox1(w[0]), w[1]);
	w[5] = sum(sbox2(w[1]), w[2]);
	w[6] = sum(sbox3(w[2]), w[3]);
	w[7] = sum(sbox4(w[3]), w[0]);

	// step 2
	for (j=8; j<88; j++)
	{
		if ((j % 4) != 0)
		  w[j] = sum(w[j-8], w[j-1]);

		//W_i = W_i-8 + RB(W_i−1)^5 + (1, 0, 1, 0, 1, 0)
		if ((j % 8) == 0)
		  w[j] = sum(w[j-8], sum(sbox2((((w[j-1] << 1) & 0x3F) | ((w[j-1] >> 5) & 1))), 0x2A));

		if ((j % 8) == 4)
		  w[j] = sum(w[j-8], sbox3(w[j-1]));
	}

	// step 3
	memset(out, 0, sizeof(out));
	for (round=0; round<16; round++)
	{
		idx = round % 5 + (round/5)*20 + 8;
		out[round] = w[idx] << 18;
		out[round] += w[idx+5] << 12;
		out[round] += w[idx+10] << 6;
		out[round] += w[idx+15] & 63;
	}
}

// matrix mul word with vector
int MixingLayer(int m, int inverse)
{
	int n, i, j, mask;
	int prod=0, s=0, r;

	// j columns, i rows
	for (j=0; j<4; j++)
	{
		mask = 24; // vector size, 24bit
		for (i=0; i<4; i++)
		{
			mask -= 6;
			n = (m >> mask) & 63; 		// i-th vector component (6 bits)
			r = inverse ? mul(n, lambda_i[i][j]) :  mul(n, lambda[i][j]); 	// do matrix mul at i-th x j-th
			s = sum(s, r); 			// sum the result
		}
		prod += s << (18 - 6*j);		// find the location in the 24bit space
		s = 0;							// and place s
	}

	return prod;
}
// in: 24 bit msg, 24 bit key out: 24 bit cipher
int Bunny24enc(int m, int key)
{
	int keys[16];
	int round;

	KeySchedule(key, keys);

	// whitening
	m = sum(m, keys[0]);

	for (round=1; round<16; round++)
	{
		m = MixingLayer(sbox_enc(m), 0);
		m = sum(m, keys[round]);
	}

	return m;
}

int Bunny24dec(int m, int key)
{
	int keys[16];
	int round;

	KeySchedule(key, keys);

	for (round=15; round>0; round--)
	{
		m = sum(m, keys[round]);
		m = sbox_dec(MixingLayer(m, 1));
	}
	m = sum(m, keys[0]);

	return m;
}

// expects a 24bit message m, initialization vector iv & key
int Bunny24enc_cbc_ex(int m, int iv, int key)
{
	if (iv == 0)
		return Bunny24enc(m, key);

	m = sum(m, iv);
	return Bunny24enc(m, key);

//	int blocks=0, i;
//	int plain;
//	long int message = m, c=0;
//
//	if (iv == 0)
//		return Bunny24enc(m, key);
//
//	// do some padding
//	// how many blocks of 24bit is expected?
//	while (message != 0){
//		message >>= 4;
//		blocks++;
//	}
//
//	// compute the number of pads needed
//	i = blocks%6;
//
//	// pad the message
//	m <<= (6-i)*4;
//
//	// number of 24bit blocks
//	blocks = (blocks + 6 - i)/6;

//	for (i=1; i<=blocks; i++)
//	{
//		plain = m >> (24*(blocks-i));
//		plain = sum(plain & 0xFFFFFF, iv);
//		iv = Bunny24enc(plain, key);
//		c += ((long int)iv << (blocks-i)*24);
//	}

//	return c;
}


int Bunny24dec_cbc_ex(int m, int iv, int key)
{
	if (iv == 0)
		return Bunny24dec(m, key);

	m = Bunny24dec(m, key);

	return sum(m, iv);
}

void Bunny24enc_cbc(int *m, int n, int iv, int key, int *cipher)
{

}


