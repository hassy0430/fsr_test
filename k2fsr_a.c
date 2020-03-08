/******************************************************************************/
/* k2fsr_a.c                                                                  */
/* KCipher-2 FSR-A internal function.                                         */
/* Reference: S. Kiyomoto, W. Shin KDDI R&D Laboratories, Inc                 */
/* "A Description of the KCipher-2 Encryption Algorithm" ISSN: 2070-1721      */
/* Takahiro Hashimoto                                                         */
/*                                                              March 8, 2020 */
/******************************************************************************/

#include <stdio.h>

#include "k2.h"

/* 8bit parity */
uint32_t parity8(uint32_t x)
{
	uint32_t p;
	p = (x);
	p ^= (p >> 4);
	p ^= (p >> 2);
	p ^= (p >> 1);
	return p & PICK1;
}

/* 32bit parity */
uint32_t parity32(uint32_t x)
{
	uint32_t p;
	p = (x);
	p ^= (p >> 16);
	p ^= (p >> 8);
	p ^= (p >> 4);
	p ^= (p >> 2);
	p ^= (p >> 1);
	return p & PICK1;
}

/* Multiplication in Galois Field */
uint32_t multi_GF(uint32_t a, uint32_t num, uint32_t gf, uint32_t gf_idx)
{
	uint32_t t = 0;
	int pos;
	for (pos = 7; pos >= 0; pos--) {
		if ((num) & (PICK1 << pos))break;
	}
	for (int i = pos; i >= 0; i--) {
		t = (t << 1) ^ (((num) & (PICK1 << i)) ? (a) : 0);
		if (t & (PICK1 << (gf_idx)))
			t ^= (gf);
	}
	return t;
}

/* Make coefficients of a monic polynomial  */
void make_coefficients(uint32_t multi[], const uint32_t index[], uint32_t gf)
{
	for (int i = 0; i < ALPHA_SIZE; i++) {
		for (int j = 0; j < (int)index[i]; j++) {
			multi[i] <<= 1;
			if (multi[i] & PICK8)
				multi[i] ^= gf | !(parity8(multi[i] ^ gf));
		}
	}
}

/* Make alpha look up table */
void make_alpha_table(uint32_t alpha[], uint32_t multi[], uint32_t gf)
{
	uint32_t temp_u32;
	for (int i = 0; i < TABLE_SIZE; i++) {
		for (int j = 0; j < ALPHA_SIZE; j++) {
			temp_u32 = multi_GF(multi[j], (uint32_t)i, gf, GF_INDEX);
			alpha[i] |= temp_u32 << (j * 8);
		}
	}
}

/* Update FSR-A registers */
void update_fsr(uint32_t fsr[], uint32_t alpha[])
{
	uint32_t fb;
	fb = (fsr[0] << 8) ^ alpha[(fsr[0] >> 24)] ^ fsr[3];
	fsr[0] = fsr[1];
	fsr[1] = fsr[2];
	fsr[2] = fsr[3];
	fsr[3] = fsr[4];
	fsr[4] = fb;
}

/* Print FSR-A registers */
void printf_fsr(uint32_t fsr[], int loop, FILE* stream)
{
	fprintf(stream, "%s%s", HORIZONTAL_LINE, STR_NEWLINE);
	fprintf(stream, "loop:%2d%s", loop, STR_NEWLINE);
	for (int i = 0; i < FSR_A_SIZE; i++) {
		fprintf(stream, "FSR-A[%d]:%08X%s", i, fsr[i], STR_NEWLINE);
	}
}
