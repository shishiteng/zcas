#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef OS_LINUX
#include<sys/time.h>//测试执行时间
#endif/* OS_LINUX*/
#include <string.h>
#include "md5.h"

typedef struct zcas_license{
  int year;
  int day;
  int workdays;
  char usrinfo[100];
  char crc[33];
  char desc[1024];
}zcas_license;

char* tool_md5(char *instr,int n);
char* tool_encrypt(char *in);
char* tool_decrypt(char *in);
//int tool_createlicense(char *filename,char *input,int workdays);
int tool_verifylicense(char *filename);
