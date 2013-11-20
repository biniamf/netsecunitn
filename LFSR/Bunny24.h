/*
 * Bunny24.h
 *
 *  Created on: Nov 17, 2013
 *      Author: biniam
 */

#ifndef BUNNY24_H_
#define BUNNY24_H_

int Bunny24enc(int m, int key);
int Bunny24dec(int m, int key);

void Bunny24enc_cbc(int *m, int n, int iv, int key, int *cipher);
int Bunny24dec_cbc(int m, int iv, int key);

#endif /* BUNNY24_H_ */
