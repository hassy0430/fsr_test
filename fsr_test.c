/******************************************************************************/
/* fsr_test.c                                                                 */
/* Simulate KCipher-2's FSR-A register                                        */
/* In the description, alpha look up tables have had already calucrated.      */
/* I used the primitive polynominal beta(=x^8+x^7+x^6+x+1) and calucrated     */
/* alpha_0 look up table. then I simulated FSR-A.                             */
/* Reference: S. Kiyomoto, W. Shin KDDI R&D Laboratories, Inc                 */
/* "A Description of the KCipher-2 Encryption Algorithm" ISSN: 2070-1721      */
/* Takahiro Hashimoto                                                         */
/*                                                              March 8, 2020 */
/******************************************************************************/
#include <stdio.h>

#include "k2.h"
#include "k2_key.h"

int main(int argc, char** argv)
{
	_Buf b;
	_Control ctrl;
	int debug = DEBUG;
	int ret = 0;
	uint32_t fsr_tv[FSR_A_SIZE] = { 0xBE3CA984, 0x974E6719, 0x86916EFF, 0xF52DACF9, 0x960329B5 };

	//Initialize memory
	memset(&b, 0, sizeof(_Buf));
	memset(&ctrl, 0, sizeof(_Control));
	memcpy(b.beta_multi, beta_init, sizeof(uint32_t) * ALPHA_SIZE);
	ctrl.loop = LOOP_A;
	ctrl.stream = STREAM;

	//Default settings
	strcpy(ctrl.path[INPUT_NUM], INPUT_FILE);
	strcpy(ctrl.path[OUTPUT_NUM], OUTPUT_FILE);

	//Get argument options
	if ((ret = get_arg(argc, argv, &ctrl)) != OFF) goto LABEL_END;
	if (ctrl.flag_files[INPUT_NUM] == ON){
		ctrl.open_mode[INPUT_NUM]  = 'r';
		ctrl.open_mode[OUTPUT_NUM] = 'w';
	}
	else {
		ctrl.open_mode[INPUT_NUM]  = 'w';
		ctrl.open_mode[OUTPUT_NUM] = 'w';
		if (ctrl.flag_files[INPUT_NUM] != FLAG_TEST)
			ctrl.flag_files[INPUT_NUM] = ON;
	}
	//Open files
	if ((ret = open_files(&ctrl)) != OFF) goto LABEL_END;
	if (debug)print_control(&ctrl);
	//Set key
	if (ctrl.flag_files[INPUT_NUM] == FLAG_TEST) {
		memcpy(b.fsr_a, fsr_tv, sizeof(uint32_t) * FSR_A_SIZE);
	}
	else{
		if ((ret = set_key(&b, &ctrl))) goto LABEL_END;
	}

	/*  Make coefficients of a monic polynomial */
	debug = 1;
	make_coefficients(b.beta_multi, beta_index, BETA_GF);
	for (int i = 0; i < ALPHA_SIZE; i++) {
		if (debug)fprintf(ctrl.stream, "beta^%-2d = %02X%s", beta_index[i], b.beta_multi[i], STR_NEWLINE);
	}
	debug = 1;
	/* Make alpha look up table */
	if (debug)fprintf(ctrl.stream, "alpha_0[256]={%s", STR_NEWLINE);
	make_alpha_table(b.alpha_0, b.beta_multi, BETA_GF);
	for (int i = 0; i < TABLE_SIZE; i++) {
		if (debug)fprintf(ctrl.stream, "0x%08X%s%s", b.alpha_0[i], (i < TABLE_SIZE - 1) ? "," : "", (i + 1) % 4 ? "" : STR_NEWLINE);
	}
	if (debug)fprintf(ctrl.stream, "};%s", STR_NEWLINE);

	/* FSR-A */
	if (debug)printf_fsr(b.fsr_a, 0, ctrl.stream);
	for (int i = 0; i < ctrl.loop; i++) {
		update_fsr(b.fsr_a, b.alpha_0);
		if (debug)printf_fsr(b.fsr_a, i + 1, ctrl.stream);
		if (ctrl.flag_files[OUTPUT_NUM] == ON) {
			if((ret = write_int32(b.fsr_a[0], &ctrl)))goto LABEL_END;
		}
	}
	if (debug)fprintf(ctrl.stream, "%s%s", HORIZONTAL_LINE, STR_NEWLINE);

LABEL_END:
	close_files(&ctrl);

	if (ret) {
		err_msg(ret);
		print_usage(argv);
	}

	fprintf(ctrl.stream, "PROGRAM END.%s", STR_NEWLINE);

	//system("pause");

	return 0;
}
