#include "zcas_part.h"
#include "zcas_paragraph.h"
#include "zcas_sentence.h"
#include "zcas_common.h"

/*功能：对文章进行分割，方式：单主题、多主题
 *参数：
 *返回值：0-正确，其他-错误
 */
int zcas_getPart(zcas_config *config,zcas_doc *doc)
{
  DEBUG_PRINT("[ZCAS_PART]enter zcas_getPart\n");
  if(NULL == config || NULL == doc){
    printf("zcas_getPart:input invalid(0x%x,0x%x).\n",config,doc);
    DEBUG_PRINT("[ZCAS_PART]exit zcas_getPart.\n");
    return -1;
  }

  switch(config->thememode){
  case 0:
    doc->parts = zcas_getParts(config,doc->text);
    break;
  case 1:
    doc->parts = zcas_getPartsByTitle(config,doc->text);
    break;
  case 2:
    doc->parts = zcas_getPartsBySize(config,doc->text,config->partbytes);
    break;
  case 3:
    doc->parts = zcas_getPartsByPara(config,doc->text,config->paracount);
    break;
  default:
    break;
  }

  if(NULL == doc->parts){
    DEBUG_PRINT("getParts failed!\n");
    return ZCAS_ERROR;
  }

#if 0
  if(ZCAS_OK != zcas_TeamParts(&doc->parts)){
    DEBUG_PRINT("TeamParts failed\n");
    return ZCAS_ERROR;
  }
#endif

  DEBUG_PRINT("[ZCAS_PART]exit zcas_getPart\n");
  return ZCAS_OK;
}

/*功能：获取parts，单part
**参数：config-zcas句柄，input-照耀字符串
**返回值：part链表
*/
zcas_part *zcas_getParts(zcas_config *config,char* input)
{
  zcas_part *partlist = NULL;
  zcas_part tpart;

  const char * split = LINE_SEP; 
  char *line = NULL;
  char *buf = NULL;
  int text_max_size = ZCAS_TEXT_MAX_LENTH;
  int titleFlag = 0;
  int nflag = 0;

  DEBUG_PRINT("[ZCAS_PART]enter zcas_getParts\n");
  if(NULL == config || NULL == input){
    printf("zcas_getParts:input invalid(0x%x,0x%x).\n",config,input);
    return NULL;
  }

  memset(&tpart,0,sizeof(zcas_part));
  tpart.text = (char*)malloc(text_max_size);
  memset(tpart.text,'\0',text_max_size);
  memset(tpart.title,'\0',sizeof(tpart.title));

  buf = (char*)malloc(strlen(input)+1);
  memset(buf,0,strlen(input)+1);
  strcpy(buf,input);
  
  /* 一个标题 */
  line = strtok(buf,split);
  while(line != NULL) { 
    char *tmp = NULL;
    int len = strlen(line);
    tmp = zcas_getTextofFirstSent(config,line);
    if(0 == nflag && tmp == NULL && len<=title_Max_Len && len>2 && 0==titleFlag){
      strcpy(tpart.title,line);
      titleFlag = 1;
      DEBUG_PRINT("title:%s\n",tpart.title);
    }
    else if(len > 10){
	  strcat(tpart.text,line);
	  strcat(tpart.text,LINE_SEP);
    }
    free(tmp);
    nflag++;
    line = strtok(NULL,split); 
  } 

  zcas_Partlist_addtail(&partlist,tpart);
  free(tpart.text);
  free(buf);

  if(config->debugmode){
    zcas_part *parts = NULL;
    for(parts = partlist;parts!=NULL;parts=parts->next){
      DEBUG_PRINT("[parts TITLE]:%s\n",parts->title);
      DEBUG_PRINT("[parts TEXT]:%s\n",parts->text);
    }
  }
  DEBUG_PRINT("[ZCAS_PART]exit zcas_getParts\n");

  return partlist;
}

/*功能：根据标题划分文章
**参数：config-zcas句柄，input-输入文章
**返回值：part指针
*/
zcas_part *zcas_getPartsByTitle(zcas_config *config,char* input)
{
  zcas_part *partlist = NULL;
  const char * split = LINE_SEP; 
  char *line = NULL;
  char *buf = NULL;
  int text_max_size = 1024*100;
  int inlen = 0;

  DEBUG_PRINT("[ZCAS_PART]enter zcas_getPartsByTitle\n");
  if(NULL == config || NULL == input){
    printf("zcas_getPartsByTitle:input invalid(0x%x,0x%x).\n",config,input);
    DEBUG_PRINT("[ZCAS_PART]exit zcas_getPartsByTitle\n");
    return NULL;
  }

  inlen = strlen(input);
  buf = (char*)malloc(inlen+1);
  memset(buf,0,inlen+1);
  strcpy(buf,input);

  line = strtok(buf,split);
  while(line != NULL){
    if(zcas_istitle(config,line)){
      //取标题,每遇一个标题，就当是一个part，直接加入链表
      zcas_part tpart;
      tpart.parags = NULL;
      tpart.hashwords = NULL;
      tpart.text = (char*)malloc(text_max_size);
      memset(tpart.text,'\0',text_max_size);
      memset(tpart.title,'\0',sizeof(tpart.title));

      strcpy(tpart.title,line);
      line = strtok(NULL,split);
      while(line != NULL){
	    if(!zcas_istitle(config,line)){
	      strcat(tpart.text,line);
	      strcat(tpart.text,LINE_SEP);
	    }
	    else
	      break;
	    line = strtok(NULL,split);
      }
      zcas_Partlist_addtail(&partlist,tpart);
      tpart.parags = NULL;
      tpart.hashwords = NULL;
      free(tpart.text);
    }
    else{
      //取正文
      zcas_part tpart;
      memset(&tpart,0,sizeof(zcas_part));
      tpart.text = (char*)malloc(text_max_size);
      memset(tpart.text,'\0',text_max_size);
      memset(tpart.title,'\0',sizeof(tpart.title));
      strcat(tpart.text,line);
      line = strtok(NULL,split);
      while(line != NULL){
	    if(!zcas_istitle(config,line)){
	      strcat(tpart.text,line);
	      strcat(tpart.text,LINE_SEP);
	    }
	    else
	      break;
	    line = strtok(NULL,split);
      }
      zcas_Partlist_addtail(&partlist,tpart);
      free(tpart.text);
    }
    //line = strtok(NULL,split);
  }

  free(buf);

  if(config->debugmode){
    zcas_part *parts = NULL;
    for(parts = partlist;parts!=NULL;parts=parts->next){
      DEBUG_PRINT("[parts TITLE]:%s\n",parts->title);
      DEBUG_PRINT("[parts TEXT]:%s\n",parts->text);
    }
  }
  DEBUG_PRINT("[ZCAS_PART]exit zcas_getPartsByTitle\n");
  
  return partlist;
}

/*功能：根据字节数划分文章
pp**参数：config-zcas句柄，input-输入文章，nbytes-每个part字节数
**返回值：part指针
*/
zcas_part *zcas_getPartsBySize(zcas_config *config,char* input,int nbytes)
{
  zcas_part *partlist = NULL;
  char *text = NULL;
  char *str = NULL;
  char *tmp = NULL;
  int inlen = 0;
  int n = 0;
  int size = 0;

  DEBUG_PRINT("[ZCAS_PART]enter zcas_getPartsBySize\n");
  if(NULL == config || NULL == input){
    printf("zcas_getPartsBySize:input invalid(0x%x,0x%x).\n",config,input);
    DEBUG_PRINT("[ZCAS_PART]exit zcas_getPartsBySize\n");
    return NULL;
  }

  str = (char*)malloc(nbytes);
  memset(str,0,nbytes);

  inlen = strlen(input);
  text = input;
  //inlen-输入字符串总长度；n-input的指针偏移量；size-每个part的text长度
  while(n < inlen){
    int lenth = 0;
    tmp = zcas_getTextofFirstSent(config,text);
    if(NULL == tmp){
      DEBUG_PRINT("get sentence failed.\n");
      DEBUG_PRINT("%s\n",text);
      break;
    }
    lenth = strlen(tmp);
    if(lenth > nbytes){
      DEBUG_PRINT("this sentence is too large.\n");//超出长度
      text += lenth;
      n += lenth;
    }
    else if((size+lenth)>nbytes){
      //printf("add to list.\n");//加入链表
      zcas_part tpart;
      memset(&tpart,0,sizeof(zcas_part));
      tpart.text = (char*)malloc(size+1);
      memset(tpart.text,0,size+1);
      memcpy(tpart.text,str,size);
      zcas_Partlist_addtail(&partlist,tpart);
      free(tpart.text);
      memset(str,0,nbytes);
      size = 0;
    }
    else if((n+nbytes) >= inlen){
      int size = inlen-n;
      zcas_part tpart;
      memset(&tpart,0,sizeof(zcas_part));
      tpart.text = (char*)malloc(size+1);
      memset(tpart.text,0,size+1);
      memcpy(tpart.text,text,size);
      zcas_Partlist_addtail(&partlist,tpart);
      free(tpart.text);
      free(tmp);
      break;
    }
    else{
      strcat(str,tmp);
      size += lenth;
      text += lenth;
      n += lenth;
    }
    free(tmp);
  }
  
  free(str);
  if(config->debugmode){
    zcas_part *parts = NULL;
    for(parts = partlist;parts!=NULL;parts=parts->next){
      DEBUG_PRINT("[parts TITLE]:%s\n",parts->title);
      DEBUG_PRINT("[parts TEXT]:%s\n",parts->text);
    }
  }
  DEBUG_PRINT("[ZCAS_PART]exit zcas_getPartsBySize\n");
  return partlist;
}

/*功能：根据百分比划分文章
**参数：config-zcas句柄，input-输入文章，percent-百分比
**返回值：part指针
*/
zcas_part *zcas_getPartsByPercent(zcas_config *config,char* input,double percent)
{
  zcas_part *partlist = NULL;
  int nbytes = 0;

  DEBUG_PRINT("[ZCAS_PART]exit zcas_getPartsByPercent\n");
  if(NULL == config || NULL == input){
    printf("zcas_getPartsByPercent:input invalid(0x%x,0x%x).\n",config,input);
    DEBUG_PRINT("[ZCAS_PART]exit zcas_getPartsByPercent\n");
    return NULL;
  }

  nbytes = (int)strlen(input)*percent;
  partlist = zcas_getPartsBySize(config,input,nbytes);

  DEBUG_PRINT("[ZCAS_PART]exit zcas_getPartsByPercent\n");
  return partlist;
}

/*功能：根据段落划分文章
**参数：config-zcas句柄，input-输入文章，count-每个part段落数
**返回值：part指针
*/
zcas_part *zcas_getPartsByPara(zcas_config *config,char* input,int count)
{
  zcas_part *partlist = NULL;
  const char *split = LINE_SEP;
  char *text = NULL;
  char *buf = NULL;
  int n = 0;
  int inlen = 0;

  DEBUG_PRINT("[ZCAS_PART]enter zcas_getPartsByPara\n");
  if(NULL == config || NULL == input){
    printf("zcas_getPartsByPara:input invalid(0x%x,0x%x).\n",config,input);
    DEBUG_PRINT("[ZCAS_PART]exit zcas_getPartsByPara\n");
    return NULL;
  }

  inlen = strlen(input);
  buf = (char*)malloc(inlen+1);
  memset(buf,0,inlen+1);
  strcpy(buf,input);

  text = strtok(buf,split);
  while(text != NULL){
    zcas_part tpart;
    memset(&tpart,0,sizeof(zcas_part));
    tpart.text = (char*)malloc(inlen+1);
    memset(tpart.text,'\0',inlen+1);
    while(n++ < count){
      strcat(tpart.text,text);
      text = strtok(NULL,split);
      if(NULL == text){
	    break;
      }
    }
    zcas_Partlist_addtail(&partlist,tpart);
    free(tpart.text);
    n = 0;
  }

  free(buf);

  if(config->debugmode){
    zcas_part *parts = NULL;
    for(parts = partlist;parts!=NULL;parts=parts->next){
      DEBUG_PRINT("[parts TITLE]:%s\n",parts->title);
      DEBUG_PRINT("[parts TEXT]:%s\n",parts->text);
    }
  }
  DEBUG_PRINT("[ZCAS_PART]exit zcas_getPartsByPara\n");
  return partlist;
}

/*功能：
**参数：
**返回值：
*/
int zcas_TeamParts(zcas_part **partlist)
{
  zcas_part *p= NULL;
  zcas_part *tmppart = NULL;
  zcas_part *part= NULL;
  zcas_part tpart;

  int n;
  zcas_parag *paraglist = NULL;
  zcas_word *words = NULL;
  zcas_part *tmppas = NULL;
  zcas_part *parts = NULL;

  DEBUG_PRINT("[ZCAS_PART]enter zcas_TeamParts\n");

  if(NULL == partlist || NULL == *partlist){
    DEBUG_PRINT("[ZCAS_PART]exit zcas_TeamParts.\n");
    return -1;
  }
  p = *partlist;
  tmppart = NULL;
  part= *partlist;
  tpart = *part;
  part = part->next;
  for(;part!=NULL;part=part->next){
    n = strlen(part->text);
    if(strlen(part->text)>pas_Min_Len || strlen(part->title) > 0){
      zcas_Partlist_addtail(&tmppart,tpart);
      tpart = *part;
    }
    else{
      strcat(tpart.title,part->title);
      strcat(tpart.text,part->text);
    }
  }

  zcas_Partlist_addtail(&tmppart,tpart);
  *partlist = tmppart;

  /* release part封装简化结构 */
  while(p!=NULL){
    tmppas = p->next;
    free(p->text);
    free(p);
    p = tmppas;
  }

  if(1 == gprintlevel){
    for(parts = *partlist;parts!=NULL;parts=parts->next){
      DEBUG_PRINT("[2TITLE]:\n",parts->title);
      DEBUG_PRINT("[2TEXT]:\n",parts->text);
    }
  }

  DEBUG_PRINT("[ZCAS_PART]exit zcas_TeamParts\n");
  return 0;
}

/*功能：在part链表尾部插入新元素
**参数：partlist-part链表；part-插入元素
**返回值：0
*/
int zcas_Partlist_addtail(zcas_part **partlist,zcas_part part)
{
  zcas_part *newpart = NULL;
  zcas_part *tmp = NULL;
  zcas_part *last = NULL;

  if(NULL == *partlist){
    newpart =(zcas_part*)malloc(sizeof(zcas_part));
    memset(newpart,0,sizeof(zcas_part));
    *newpart = part;
    newpart->text = (char *)malloc(strlen(part.text)+1);
    memset(newpart->title,'\0',sizeof(newpart->title));
    memset(newpart->text,'\0',strlen(part.text)+1);
    strcpy(newpart->title,part.title);
    strcpy(newpart->text,part.text);
    newpart->next = NULL;
    newpart->prev = NULL;
    *partlist = newpart;
    /* DEBUG_PRINT("[ZCAS_PART]exit zcas_Partlist_addtail.0\n"); */
    return 0;
  }
  
  for(tmp=*partlist;tmp!=NULL;tmp=tmp->next){
    last = tmp;
  }

  newpart =(zcas_part*)malloc(sizeof(zcas_part));
  memset(newpart,0,sizeof(zcas_part));
  *newpart = part;
  newpart->text = (char *)malloc(strlen(part.text)+1);
  memset(newpart->title,'\0',sizeof(part.title));
  memset(newpart->text,'\0',strlen(part.text)+1);
  strcpy(newpart->title,part.title);
  strcpy(newpart->text,part.text);
  newpart->next = NULL;
  newpart->prev = last;
  last->next = newpart;
  /* DEBUG_PRINT("[DEBUG]zcas_Partlist_addtail.2:%s\n",newpart->text); */
  /* DEBUG_PRINT("[ZCAS_PART]exit zcas_Partlist_addtail\n"); */
  return 0;
}

/*功能:释放parts
**参数：parts-part链表
**返回值：0
*/
int zcas_release_parts(zcas_part *parts)
{
  zcas_part *part = NULL;
  zcas_word *keyword = NULL;
  zcas_part *tmppas = NULL;

  for(part=parts;part!=NULL;){
    zcas_release_parags(part->parags);
    zcas_UTHASH_DOUBLE_release(part->hashwords);
    tmppas = part->next;
    free(part->text);
    free(part);
    part = tmppas;
  }

  return 0;
}

/*功能：判断字符串是不是标题
**参数：config-zcas句柄，text-字符串
**返回值：0-不是标题；1-是标题
*/
int zcas_istitle(zcas_config *config,char *text)
{
  unsigned int len = 0;
  char *tmp = NULL;

  len = strlen(text);  
  tmp = zcas_getTextofFirstSent(config,text);
  if(tmp == NULL && len<=title_Max_Len && len>2)
    return 1;
  else{
    free(tmp);
    return 0;
  }
}

/*功能：删除字符串中的无效字符，如：'\t'、' '等
**参数：
**返回值：ZCAS_OK
*/
int zcas_removenullchars(zcas_config *config,char *text)
{
  int i =0;
  int count = 0;
  if(NULL == config || NULL == text){
    DEBUG_PRINT("zcas_removenullchars:input invalid,(0x%x,0x%x)\n",config,text);
    return ZCAS_OK;
  }
  DEBUG_PRINT("enter zcas_removenullchars.\n");
  for(i=0;i<2;i++){
    char *p = NULL;
    p = strchr(text,config->NullChrs[i]);
    while(p != NULL){

      int size = strlen(p);
      char *tmp = (char*)malloc(size);
      memset(tmp,'\0',size);
      strcpy(tmp,p+1);
      memset(p,'\0',size);
      strcpy(p,tmp);
      free(tmp);
      p = strchr(text,config->NullChrs[i]);
      count++;
    }
  }

  DEBUG_PRINT("null chars count:%d\n",count);
  DEBUG_PRINT("exit zcas_removenullchars.\n");
  return ZCAS_OK;
}
