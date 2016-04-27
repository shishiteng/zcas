#ifndef ZCAS_PARAGRAPH_H
#define ZCAS_PARAGRAPH_H
#include "zcas_struct.h"

int zcas_getParas(zcas_config *config,zcas_part* parts);
int zcas_getPara(zcas_config *config,zcas_part* part);

int zcas_paraglist_addtail(zcas_parag **paraglist,zcas_parag para);
int zcas_release_parags(zcas_parag *parags);

#endif/* _ZCAS_PARAGRAPH_H__ */
