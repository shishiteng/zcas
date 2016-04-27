#ifndef ZCAS_VECTOR_H
#define ZCAS_VECTOR_H
#include "zcas_struct.h"

int zcas_getPartKeywords(zcas_config *config,zcas_part *parts,char *title);

int zcas_getSimilarity(zcas_config *config,zcas_part *parts);
int zcas_normalize_Weight(zcas_part *parts);
double zcas_getCosine(UTHASH_DOUBLE *hashwords,zcas_word *seg);
double zcas_getCosineEx(zcas_word *seg1, zcas_word *seg2);
double zcas_getMold(zcas_word*seg);
double zcas_getMoldEx(UTHASH_DOUBLE *hashwords);
double zcas_getwordrelation(zcas_word *word1,zcas_word *word2,UTHASH_CONJWORDS *partconjwords);
int zcas_adjustweightbyfreq(zcas_config *config,zcas_part *parts);

void zcas_dbg_PrintKeywords(zcas_part *parts);
#endif/* _ZCAS_VECTOR_H__ */
