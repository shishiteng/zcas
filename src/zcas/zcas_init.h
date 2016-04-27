#ifndef ZCAS_INIT_H
#define ZCAS_INIT_H
#include <stdio.h>
#include "zcas_struct.h"

DLLEXPORT int zcas_init(char *confpath,zcas_config **pconfig);
DLLEXPORT int zcas_reload(zcas_config *config,char *confpath);
DLLEXPORT void zcas_free(void *p);
DLLEXPORT void zcas_setdebugmode(int mode);
DLLEXPORT void zcas_setoption(zcas_config *config,char *opt,char *val);
DLLEXPORT void zcas_setopt_threldmode(zcas_config *config,int n);
DLLEXPORT void zcas_setopt_outpercent(zcas_config *config,double d);
DLLEXPORT void zcas_setopt_outcount(zcas_config *config,int n);
DLLEXPORT void zcas_setopt_wordcount(zcas_config *config,int n);
DLLEXPORT void zcas_setopt_thememode(zcas_config *config,int n);
DLLEXPORT void zcas_setopt_partbytes(zcas_config *config,int n);
DLLEXPORT void zcas_setopt_paracount(zcas_config *config,int n);
DLLEXPORT void zcas_setopt_topicsent(zcas_config *config,int n);
DLLEXPORT void zcas_setopt_bestout(zcas_config *config,int n);
DLLEXPORT void zcas_setopt_conjword(zcas_config *config,int n);
DLLEXPORT void zcas_setopt_freqword(zcas_config *config,int n);
DLLEXPORT void zcas_setopt_articletype(zcas_config *config,int type);

int zcas_getconfig(zcas_config *config,char *confpath);
int zcas_loadconfig(char *filename,zcas_config *config);

int zcas_islimited();
int zcas_verifylicense(char *path);

zcfc_handle* zcas_loadzcfcdict(const char* dir);
int zcas_loadzcasdict(zcas_config *config,char* filename);
int zcas_loadbinarydict(zcas_config *config,char *filename);
int zcas_loadPuncts(FILE* fp,zcas_dicthead *head,char*epuncts,char*cpuncts,char*erends,char*crends);
int zcas_loadConjwordDict(FILE *fp,zcas_dicthead *head,UTHASH_CONJWORDS  **conjwords,UTHASH_DOUBLE **freqword);
int zcas_loadWeightWords(FILE *fp,zcas_dicthead *head,UTHASH_DOUBLE **wtwds);

stringList *zcas_split(char* s,const char* delim,int includeDelim);

int gprintlevel;

#endif
