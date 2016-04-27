#include "zcas_struct.h"
#include "zcas_paragraph.h"
#include "zcas_common.h"
#include "zcas_sentence.h"

/*功能：获取paras
**参数：config-zcas句柄；parts-part链表
**返回值：0
*/
int zcas_getParas(zcas_config *config,zcas_part* parts)
{
  const char * split = LINE_SEP; 
  char *line = NULL;
  char *tmp = NULL;
  zcas_part *part = NULL;
  zcas_parag *paras = NULL;

  DEBUG_PRINT("[ZCAS_PARA]enter zcas_getParags\n");
  if(NULL == config || NULL == parts){
    printf("zcas_getParas:input invalid(0x%x,0x%x).\n",config,parts);
    DEBUG_PRINT("[ZCAS_PARA]exit zcas_getParas.\n");
    return -1;
  }

  for(part = parts;part != NULL;part = part->next){
    zcas_getPara(config,part);
  }   
  
  if(config->debugmode){
    for(part = parts;part!=NULL;part=part->next){
      for(paras=part->parags;paras!=NULL;paras=paras->next)
	    DEBUG_PRINT("[PARA]:%s\n",paras->text);
    }
  }

  DEBUG_PRINT("[ZCAS_PARA]exit zcas_getParags\n");

  return ZCAS_OK;
}

/*功能：获取一个part的paras
**参数：config-zcas句柄；part-单个part
**返回值：0
*/
int zcas_getPara(zcas_config *config,zcas_part* part)
{
  const char * split = LINE_SEP; 
  char *line = NULL;
  char *tmp = NULL;
  char *baktext = NULL;

  DEBUG_PRINT("[ZCAS_PARA]enter zcas_getPara\n");
  if(NULL == config || NULL == part){
    printf("zcas_getPara:input invalid(0x%x,0x%x).\n",config,part);
    DEBUG_PRINT("[ZCAS_PARA]exit zcas_getPara.\n");
    return -1;
  }

  if(NULL == part->text){
    printf("zcas_getPara:part->text is null.\n");
    return -1;
  }

  baktext = (char*)malloc(ZCAS_TEXT_MAX_LENTH);
  memset(baktext,0,ZCAS_TEXT_MAX_LENTH);
  strcpy(baktext,part->text);

  line = strtok(baktext,split);
  while(NULL != line){
    tmp = zcas_getTextofFirstSent(config,line);
    if(NULL == tmp){
      if(zcas_getwordcount(line) > 50){
	    zcas_parag para;
	    memset(&para,0,sizeof(zcas_parag));
	    para.text = (char*)malloc(strlen(line)+1);
	    memset(para.text,'\0',strlen(line)+1);
	    strcat(para.text,line);
	    zcas_paraglist_addtail(&part->parags,para);
      }
    }
    else if(tmp != NULL && strlen(line)>10){
      zcas_parag para;
      memset(&para,0,sizeof(zcas_parag));
      para.text = (char*)malloc(strlen(line)+1);
      memset(para.text,'\0',strlen(line)+1);
      strcat(para.text,line);
      zcas_paraglist_addtail(&part->parags,para);
    }
    free(tmp);
    line = strtok(NULL,split); 
  }
  free(baktext);
  DEBUG_PRINT("[ZCAS_PARA]exit zcas_getParag\n");

  return ZCAS_OK;
}

/*功能：在para链表尾部插入新元素
**参数：paraglist-para链表；para-新元素
**返回值：0
*/
int zcas_paraglist_addtail(zcas_parag **paraglist,zcas_parag para)
{
  zcas_parag *newpara = NULL;
  zcas_parag *tmp,*last;
  /* printf("[ZCAS_PARAGRAPH]enter zcas_Paraglist_pushback\n"); */
  if(NULL == *paraglist){
    newpara = (zcas_parag*)malloc(sizeof(zcas_parag));
    memset(newpara,0,sizeof(zcas_parag));
    *newpara = para;
    newpara->next = NULL;
    newpara->prev = NULL;
    *paraglist = newpara;
    /* printf("[ZCAS_PARAGRAPH]exit zcas_Paraglist_pushback.0\n"); */
    return 0;
  }

  for(tmp=*paraglist;tmp!=NULL;tmp=tmp->next){
    last = tmp;
  }

  newpara =(zcas_parag*)malloc(sizeof(zcas_parag));
  memset(newpara,0,sizeof(zcas_parag));
  *newpara = para;
  newpara->next = NULL;
  newpara->prev = last;
  last->next = newpara;
  /* printf("[ZCAS_PARAGRAPH]exit zcas_Paraglist_pushback\n"); */
  return 0;
}

/*功能：释放paras
**参数：parags-para链表
**返回值:0
*/
int zcas_release_parags(zcas_parag *parags)
{
  zcas_parag *para = NULL;
  zcas_parag *tmppara = NULL;
  for(para=parags;para!=NULL;){
    zcas_Sentlist_clear(para->sents);
    tmppara = para->next;
    free(para->text);
    free(para);
    para = tmppara;
  }

  return 0;
}
