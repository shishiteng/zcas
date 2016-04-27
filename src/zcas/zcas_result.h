#ifndef ZCAS_RESULT_H
#define ZCAS_RESULT_H
#include "zcas_struct.h"

typedef struct zcas_sentcombine{
  struct zcas_sentence *sent;
  int exist;
}zcas_sentcombine;

/* 排列组合句子时的临时结构 */
typedef struct inputstruct{
  int len;
  int count;
  int m;
}inputstruct;

char* zcas_outputsummary(zcas_part* parts);
char* zcas_outputsummarybywordcount(zcas_part* parts,int wordcount);
int zcas_getThreshold(zcas_config *config,zcas_part* parts);
int zcas_getPartThreshold(zcas_config *config,zcas_part* part);

int zcas_filtrepeatsent(zcas_part *part,zcas_sentence *allsent,int count);
int zcas_getrealsentcount(zcas_sentence *sent[],int count);
int zcas_getoutcount(zcas_part *part,zcas_sentence *out[]);
double zcas_getWordsSimilarity(zcas_sentence *sent1,zcas_sentence *sent2);

int zcas_getbestoutput(zcas_config *config,zcas_part *part,int count,int outcount);
void zcas_combinesents(int n,inputstruct *input,zcas_sentcombine insent[],zcas_sentcombine sent[],zcas_sentcombine **outsent);
int zcas_adjustresultbywordcount(zcas_part *part,int sentcount,int wordcount);

#endif/* _ZCAS_RESULT_H__ */
