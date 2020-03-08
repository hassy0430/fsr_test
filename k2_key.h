#include "k2.h"

#define BETA_GF		0x000001C3	//x^8 + x^7 + x^6 + x^1 + x^0
#if 0
#define GANMA_GF	0x0000012D
#define DELTA_GF	0x0000014D
#define ZETA_GF		0x00000165
#endif
const uint32_t beta_init[ALPHA_SIZE]   = {1, 1, 1, 1};
const uint32_t beta_index[ALPHA_SIZE]  = { 71,  12,   3,  24 };
#if 0
const uint32_t ganma_index[ALPHA_SIZE] = { 29,  93, 156, 230 };
const uint32_t delta_index[ALPHA_SIZE] = { 248, 199, 16,  34 };
const uint32_t zeta_index[ALPHA_SIZE]  = { 16,  56, 253, 157 };
#endif
