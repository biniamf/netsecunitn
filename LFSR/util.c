/*
 * util.c
 *
 *  Created on: Nov 17, 2013
 *      Author: biniam
 */

#include "util.h"

void HexToBin(int *bits, unsigned long hex, int bitlen)
{
    int i;
	for (i = 0; i < bitlen; i++)
        bits[bitlen-i-1] = (hex >> i) & 1;
}
