#ifndef ZCAS_PASSAGE_H
#define ZCAS_PASSAGE_H
#include "zcas_struct.h"

int zcas_getPart(zcas_config *config,zcas_doc *doc);
zcas_part *zcas_getParts(zcas_config *config,char* input);
zcas_part *zcas_getPartsByTitle(zcas_config *config,char* input);
zcas_part *zcas_getPartsBySize(zcas_config *config,char* input,int nbytes);
zcas_part *zcas_getPartsByPercent(zcas_config *config,char* input,double percent);
zcas_part *zcas_getPartsByPara(zcas_config *config,char* input,int count);

int zcas_TeamParts(zcas_part **partlist);

int zcas_Partlist_addtail(zcas_part **partlist,zcas_part part);
int zcas_release_parts(zcas_part *parts);

int zcas_istitle(zcas_config *config,char *text);          /* 判断该字符串是否是标题 */
int zcas_removenullchars(zcas_config *config,char *text);  /* 删除text中的无效字符 */

#endif/* _ZCAS_PASSAGE_H__ */
