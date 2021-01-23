#ifndef _BUILD_TIM_H
#define _BUILD_TIM_H
#include "core_type.h"

//Read or Update the files in [blockfiles_dir], 
//and output the data files into [output_dir]
void build_TIM(char *blockfiles_dir, char *output_dir,ERROR_CODE *err);
void update_TIM(char *blockfiles_dir, char *output_dir, ERROR_CODE *err);
#endif