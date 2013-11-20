/*
 * main.c
 *
 *  Created on: Nov 17, 2013
 *      Author: biniam
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "Bunny24.h"
#include "field.h"
#include "LFSR.h"
#include "Maj5.h"
#include "All5.h"
#include "A5_1.h"
#include "util.h"

int main(int argc, char const *argv[]) {

  //int poly[] = {0,1,1};
  //int init_state[] = {1,0,1};
//  int out_len = 10;
//  int len = 3;
//  int poly = 3;
//  int init_state = 5;
//  int i;


  //sbox1(16777215);
//  printf("%d\n%d", MixingLayer(9590645, 0), MixingLayer(3551430, 1));
  //Bunny24dec(0x383830, 0x91AEF5);
   printf("%lX\n%lX\n%lX\n\n",0x123456aaaaaaa, 0xC1196C, Bunny24enc_cbc(0x123456aaaaaaa, 0x5E3E14, 0xC1196C));
   printf("%X\n%X\n%X\n",0xFA71CE, 0xF8275D, Bunny24dec(0xFA71CE, 0xF8275D));

  //mul(27, 11);

//  BE502C
//  91AEF5
//  383830
  //for (i=0; i<228; i++)
	//  printf("%d", keystream[i]);
  //printf("\n");

  // printf("\n");
  //lfsr2(poly, init_state, out_len, len);

  return 0;
}
