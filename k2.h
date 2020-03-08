/* k2.h */
#pragma once

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

/* CONSTANT VALUE */
#define PICK1		0x00000001	//for parity calculation 
#define PICK8		0x00000100	//for detect x^8

#define ALPHA_SIZE	4
#define TABLE_SIZE	256
#define GF_INDEX	8
#define FSR_A_SIZE	5

#ifndef CHAR_BIT
#define CHAR_BIT	8
#endif

#define STR_NEWLINE "\n"

#define LOOP_A		64

#define HORIZONTAL_LINE "**************************************************"

#define INPUT_NUM	0
#define OUTPUT_NUM	1
#define FILE_NUM	2
#define STR_SIZE 2049
#define LOOP_MAX 65535
#define LOOP_MIN 1

#define INPUT_FILE "reg.dat"
#define OUTPUT_FILE "output.dat"

#define OPT_CHAR '-'

#define OFF			0
#define ON			1
#define FLAG_TEST	2

#define ERR_ARG			-1
#define ERR_FOPEN		-2
#define ERR_WRITE		-3
#define ERR_SETKEY		-4
#define ERR_WRITEKEY	-5
#define ERR_HELP		-6

#define STREAM stdout
#define DEBUG 1

typedef struct {
	uint32_t beta_multi[ALPHA_SIZE];
	uint32_t alpha_0[TABLE_SIZE];
	uint32_t fsr_a[FSR_A_SIZE];
} _Buf;

typedef struct {
	FILE* fp[FILE_NUM];
	FILE* stream;
	int32_t loop;
	char path[FILE_NUM][STR_SIZE];
	char open_mode[FILE_NUM + 1];
	char flag_files[FILE_NUM];
} _Control;

/* k2fsr_a.c */
uint32_t parity8(uint32_t x);
uint32_t parity32(uint32_t x);
uint32_t multi_GF(uint32_t a, uint32_t num, uint32_t gf, uint32_t gf_idx);
void make_coefficients(uint32_t multi[], const uint32_t index[], uint32_t gf);
void make_alpha_table(uint32_t alpha[], uint32_t multi[], uint32_t gf);
void update_fsr(uint32_t fsr[], uint32_t alpha[]);
void printf_fsr(uint32_t fsr[], int loop, FILE* stream);

/* k2_lib.c */
int get_arg(int argc, char** argv, _Control* ctrl);
void err_msg(int num);
void print_usage(char** argv);
void print_control(_Control* ctrl);
int open_files(_Control* ctrl);
void close_files(_Control* ctrl);
int set_key(_Buf* b, _Control* ctrl);
int write_int32(int32_t src, _Control* ctrl);
