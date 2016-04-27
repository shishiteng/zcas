#ifndef ZCAS_SUMMARY_H
#define ZCAS_SUMMARY_H
#include "zcas_struct.h"

DLLEXPORT char *zcas_getsummary(zcas_config *config,char* instr);
DLLEXPORT char *zcas_getsummaryEx(zcas_config *config,char* instr,char *title);
DLLEXPORT int zcas_releasesummary(zcas_config *config);
DLLEXPORT char *zcas_getfc(zcas_config *config,char* instr);

#if 1
zcas_part *zcas_getPartKeywordsAB(zcas_config *config,char* instr);
double zcas_get2PartsSimilarityAB(zcas_part *part1,zcas_part *part2);
double zcas_getCosineAB(UTHASH_DOUBLE *hashwords1,UTHASH_DOUBLE *hashwords2);
#endif

#endif
