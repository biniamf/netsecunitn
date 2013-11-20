/*
 * field.c
 *
 *  Created on: Nov 17, 2013
 *      Author: biniam
 */

#include "field.h"

int sum(int a, int b)
{
	return a ^ b;
}

// based on peasant's
int mul(int a, int b)
{
	int p=0;
	int count, carry=0;

	for (count=0; count<6; count++)
	{
		if (a == 0 || b == 0) break;
		p ^= a * (b & 1);
		b >>= 1;
		carry = a & (1 << 5);
		a <<= 1;
		if (carry)
			a ^= 91; // primitive poly: x^6 + x^4 + x^3 + x + 1 (1011011)
	}
	return p;
}

int pow_poly(int a, int n)
{
	int p=1;

	while (n != 0)
	{
		if ((n & 1) != 0)
			p = mul(p, a);
		a = mul(a, a);
		n >>= 1;
	}
	return p;
}
