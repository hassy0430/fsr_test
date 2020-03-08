/* k2_lib.c */

#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

#include "k2.h"



int get_arg(int argc, char** argv, _Control* ctrl)
{
	struct stat st;
	char** endp = NULL;
	int32_t num;
	int ret = OFF;

	for (int i = 1; i < argc && ret == OFF; i++) {
		if (argv[i][0] == OPT_CHAR) {
			switch (argv[i][1]) {
			case 'i':
			case 'I':
				if (argc > i + 1 && argv[i + 1][0] != OPT_CHAR) {
					strcpy(ctrl->path[INPUT_NUM], argv[++i]);
					if ((stat(argv[i], &st)) == -1) ret = ERR_ARG;
				}
				ctrl->flag_files[INPUT_NUM] = ON;
				print_control(ctrl);
				break;
			case 'o':
			case 'O':
				if (argc > i + 1 && argv[i + 1][0] != OPT_CHAR) {
					strcpy(ctrl->path[OUTPUT_NUM], argv[++i]);
					if ((stat(argv[i], &st)) == 0) ret = ERR_ARG;
				}
				ctrl->flag_files[OUTPUT_NUM] = ON;
				print_control(ctrl);
				break;
			case 't':
			case 'T':
				ctrl->flag_files[INPUT_NUM] = FLAG_TEST;
				break;
			case 'l':
			case 'L':
				if (argc <= i + 1 || argv[i + 1][0] == OPT_CHAR)ret = ERR_ARG;
				else {
					num = strtol(argv[++i], endp, 10);
					if (num < LOOP_MIN || num > LOOP_MAX) ret = ERR_ARG;
					else
						ctrl->loop = num;
				}
				break;
			case 'h':
			case 'H':
				ret = ERR_HELP;
				break;
			default:
				ret = ERR_ARG;
				break;
			}
		}
	}
	return ret;
}

void err_msg(int num)
{
	switch (num) {
	case ERR_ARG:
		fprintf(stderr, "ARGUMENT ERROR.%s", STR_NEWLINE);
		break;
	case ERR_FOPEN:
		fprintf(stderr, "FILE OPEN ERROR.%s", STR_NEWLINE);
		break;
	case ERR_WRITE:
		fprintf(stderr, "FILE WRITE ERROR.%s", STR_NEWLINE);
		break;
	case ERR_SETKEY:
		fprintf(stderr, "SET KEY ERROR.%s", STR_NEWLINE);
		break;
	case ERR_WRITEKEY:
		fprintf(stderr, "WRITE KEY ERROR.%s", STR_NEWLINE);
		break;
	case ERR_HELP:
		break;
	}
}

void print_usage(char** argv)
{
	fprintf(stderr, "USAGE:%s", STR_NEWLINE);
	fprintf(stderr, "%s [-H] [-T] [-L] [-I <FILENAME>] [-O <FILENAME>]%s", argv[0], STR_NEWLINE);
	fprintf(stderr, "-H PRINT THIS HELP%s", STR_NEWLINE);
	fprintf(stderr, "-T USE TEST VECTOR%s", STR_NEWLINE);
	fprintf(stderr, "-L SET LOOP NUM(%d-%d)%s", LOOP_MIN, LOOP_MAX, STR_NEWLINE);
	fprintf(stderr, "-I <INPUT FILE>SET FSR REGISTER(DEFAULT:%s)%s", INPUT_FILE, STR_NEWLINE);
	fprintf(stderr, "-O <OUTPUT FILE>OUTPUT FSR REGISTER(DEFAULT:%s)%s", OUTPUT_FILE, STR_NEWLINE);
}

void print_control(_Control* ctrl)
{
	fprintf(ctrl->stream, "ctrl:%s", STR_NEWLINE);
	for(int i = 0; i < FILE_NUM; i++)
		fprintf(ctrl->stream, "fp[%d]=%p%s", i, ctrl->fp[i], STR_NEWLINE);
	fprintf(ctrl->stream, "stream=%p%s", ctrl->stream, STR_NEWLINE);
	for (int i = 0; i < FILE_NUM; i++)
		fprintf(ctrl->stream, "path[%d]=%s%s", i, ctrl->path[i], STR_NEWLINE);
	for (int i = 0; i < FILE_NUM; i++)
		fprintf(ctrl->stream, "flag_files[%d]=%d%s", i, ctrl->flag_files[i], STR_NEWLINE);
}

int open_files(_Control* ctrl)
{
	int ret = OFF;
	char mode[FILE_NUM + 1];

	for (int i = 0; i < FILE_NUM && ret == OFF; i++) {
		if (ctrl->flag_files[i] == ON) {
			sprintf(mode, "%cb", ctrl->open_mode[i]);
			if ((ctrl->fp[i] = fopen(ctrl->path[i], mode)) == NULL) ret = ERR_FOPEN;
		}
	}
	return ret;
}

void close_files(_Control* ctrl)
{
	for (int i = 0; i < FILE_NUM; i++)
		if (ctrl->fp[i] != NULL) fclose(ctrl->fp[i]);
	return;
}

int set_key(_Buf* b, _Control* ctrl)
{
	int ret = OFF;
	int read_size;
	int write_size;
	int debug = DEBUG;
	unsigned char src[FSR_A_SIZE * sizeof(int32_t)] = { 0 };
	if (ctrl->open_mode[INPUT_NUM] == 'r') {
		if ((read_size = fread(src, sizeof(char), FSR_A_SIZE * sizeof(int32_t), ctrl->fp[INPUT_NUM])) < FSR_A_SIZE * sizeof(int32_t)) ret = ERR_SETKEY;
		else {
			if (debug)fprintf(ctrl->stream, "FSR-A REGISTER:");
			for (int i = 0; i < FSR_A_SIZE; i++) {
				for (int j = 0; j < sizeof(int32_t); j++) {
					b->fsr_a[i] <<= 8;
					b->fsr_a[i] |= src[i * sizeof(int32_t) + j];
					if (debug)fprintf(ctrl->stream, "%02X", src[i * sizeof(int32_t) + j]);
				}
			}
			if (debug)fprintf(ctrl->stream, "%s", STR_NEWLINE);
		}
	}
	else {
		srand((unsigned)time(NULL));
		if (debug)fprintf(ctrl->stream, "FSR-A REGISTER:");
		for (int i = 0; i < FSR_A_SIZE * sizeof(int32_t); i++) {
			for (int j = 0; j < CHAR_BIT; j++) {
				src[i] <<= 1;
				src[i] |= (unsigned char)(rand() % 2);
			}
			if (debug)fprintf(ctrl->stream, "%02X", src[i]);
		}
		if (debug)fprintf(ctrl->stream, "%s", STR_NEWLINE);
		for (int i = 0; i < FSR_A_SIZE; i++) {
			for (int j = 0; j < sizeof(int32_t); j++) {
				b->fsr_a[i] <<= 8;
				b->fsr_a[i] |= src[i * sizeof(int32_t) + j];
			}
		}
		if ((write_size = fwrite(src, sizeof(char), FSR_A_SIZE * sizeof(int32_t), ctrl->fp[INPUT_NUM])) < (FSR_A_SIZE * sizeof(int32_t))) {
			ret = ERR_WRITEKEY;
		}
	}

	return ret;
}

int write_int32(int32_t src, _Control* ctrl)
{
	int ret = OFF;
	unsigned char buf[sizeof(int32_t)];
	int write_size = 0;
	for (int i = 0; i < sizeof(int32_t); i++) {
		buf[i] = (unsigned char)(src >> ((sizeof(int32_t) - i - 1) * CHAR_BIT) & 0xFF);
	}
	if ((write_size = fwrite(buf, sizeof(char), sizeof(int32_t), ctrl->fp[OUTPUT_NUM])) < sizeof(int32_t)) {
		ret = ERR_WRITEKEY;
	}
	return ret;
}