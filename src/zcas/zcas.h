#ifndef ZCAS_H
#define ZCAS_H

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

int zcas_init(char *confpath,zcas_config **pconfig);

char *zcas_getsummary(zcas_config *config,char* instr);
char *zcas_getsummaryEx(zcas_config *config,char* instr,char *title);
char *zcas_getfc(zcas_config *config,char* instr);

int zcas_releasesummary(zcas_config *config);

int zcas_reload(zcas_config *config,char *confpath);
void zcas_free(void *p);
void zcas_setdebugmode(int mode);
void zcas_setoption(zcas_config *config,char *opt,char *val);
void zcas_setopt_threldmode(zcas_config *config,int n);
void zcas_setopt_outpercent(zcas_config *config,double d);
void zcas_setopt_outcount(zcas_config *config,int n);
void zcas_setopt_wordcount(zcas_config *config,int n);
void zcas_setopt_thememode(zcas_config *config,int n);
void zcas_setopt_partbytes(zcas_config *config,int n);
void zcas_setopt_paracount(zcas_config *config,int n);
void zcas_setopt_topicsent(zcas_config *config,int n);
void zcas_setopt_bestout(zcas_config *config,int n);
void zcas_setopt_conjword(zcas_config *config,int n);
void zcas_setopt_freqword(zcas_config *config,int n);
void zcas_setopt_articletype(zcas_config *config,int type);
/* ***************接口函数 end*************** */


#endif
