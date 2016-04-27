#ifndef ZCAS_INTERFACE_H
#define ZCAS_INTERFACE_H

#include "zcas_struct.h"
/* ***************接口函数 begin*************** */
#ifndef OS_LINUX
wchar_t* zcas_TransformUTF8ToUnicodeM(const char* _str);
char* zcas_TransformUnicodeToUTF8M(const wchar_t* _str);
int zcas_detectTextEncoding(const char *data, unsigned int len, char **detected);
int zcas_convert(const char *toConverterName,
	const char *fromConverterName,  
    char *target, unsigned int targetCapacity,
	const char *source,
	unsigned int sourceLength);
#endif

char* zcas_ReadAllFromFile(char* filename);

zcas_config *zcas_init(char *confpath);
char *zcas_getsummary(zcas_config *config,char* instr);
char *zcas_getfc(zcas_config *config,char* instr);
int zcas_releasesummary(zcas_config *config);

int zcas_reload(zcas_config *config,char *confpath);
void zcas_freestr(char *str);
void zcas_setoption(zcas_config *config,int mode,double percent,int sentcount,int maxwordcount);
/* ***************接口函数 end*************** */

#endif
