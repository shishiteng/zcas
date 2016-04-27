#include "zcas_common.h"
#ifdef OS_LINUX
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <regex.h>
#endif

int zcas_UTHASH_DOUBLE_add(UTHASH_DOUBLE **pwords,char *key,double value)
{
  /* printf("enter zcas_UTHASH_AddDouble\n"); */
  UTHASH_DOUBLE *pnew = NULL;
  HASH_FIND(hh,*pwords,key,strlen(key)+1,pnew);
  if(NULL == pnew){
    pnew = (UTHASH_DOUBLE*)malloc(sizeof(UTHASH_DOUBLE));
    memset(pnew,0,sizeof(UTHASH_DOUBLE));
    memcpy(pnew->key,key,strlen(key)+1);
    HASH_ADD(hh,*pwords,key,strlen(key)+1,pnew);
  }
  pnew->value = value;
  /* printf("exit zcas_UTHASH_AddDouble\n"); */
  return 0;
}

int zcas_UTHASH_DOUBLE_find(UTHASH_DOUBLE *pwords,char *key,double *value)
{
  UTHASH_DOUBLE *pwd = NULL;
  HASH_FIND(hh,pwords,key,strlen(key)+1,pwd);
  if(NULL != pwd){
    *value = pwd->value;
    return 1;
  }
  else{
    return 0;
  }
}

int hashdouble_sort(struct UTHASH_DOUBLE *p1,struct UTHASH_DOUBLE *p2)
{
  double d = 0;
  if(NULL == p1 || NULL == p2)
    return 0;
  d = p1->value - p2->value;
  if(d > 0)
    return 1;
  else if(d < 0)
    return -1;
  else
    return 0;
}
void zcas_UTHASH_DOUBLE_sort(UTHASH_DOUBLE *pwords)
{
  if(NULL != pwords)
    HASH_SORT(pwords,hashdouble_sort);
}

int zcas_UTHASH_DOUBLE_release(UTHASH_DOUBLE *map)
{
  UTHASH_DOUBLE *tmp = map;
  while(tmp != NULL){
    UTHASH_DOUBLE *tmp1 = tmp;
    tmp = (UTHASH_DOUBLE *)tmp->hh.next;
    HASH_DEL(map,tmp1);
    free(tmp1);
  }
  return 0;
}

int zcas_UTHASH_CONJWORDS_add(UTHASH_CONJWORDS **pconwords,char *key,conjwordlist *value)
{
  UTHASH_CONJWORDS *pconjwords = NULL;
  HASH_FIND(hh,*pconwords,key,strlen(key)+1,pconjwords);
  if(NULL == pconjwords){
    pconjwords = (UTHASH_CONJWORDS*)malloc(sizeof(UTHASH_CONJWORDS));
    memset(pconjwords,0,sizeof(UTHASH_CONJWORDS));
    memcpy(pconjwords->key,key,strlen(key)+1);
    HASH_ADD(hh,*pconwords,key,strlen(key)+1,pconjwords);
  }
  pconjwords->value = value;

  return 0;
}

int zcas_UTHASH_CONJWORDS_find(UTHASH_CONJWORDS *pconwords,char *key,conjwordlist **pvalue)
{
  UTHASH_CONJWORDS *pwd = NULL;
  HASH_FIND(hh,pconwords,key,strlen(key)+1,pwd);
  if(NULL != pwd){
    *pvalue = pwd->value;
    return 1;
  }
  else{
    return 0;
  }
}

int zcas_UTHASH_CONJWORDS_release(UTHASH_CONJWORDS *map)
{
  UTHASH_CONJWORDS *tmp = map;
  while(tmp != NULL){
    UTHASH_CONJWORDS *tmp1 = tmp;
    tmp = (UTHASH_CONJWORDS *)tmp->hh.next;
    zcas_conjwordlist_release(tmp1->value);
    HASH_DEL(map,tmp1);
    free(tmp1);
  }
  return 0;
}

char* zcas_ReadAllFromFile(char* filename)
{
  FILE *fp = NULL;
  int Max_Buf=ZCAS_TEXT_MAX_LENTH;
  int size = 0;
  char *outstr=(char*)malloc(Max_Buf);

  DEBUG_PRINT("enter zcas_ReadAllFromFile,readfile[%s]\n",filename);
  memset(outstr,'\0',Max_Buf);

  fp = fopen(filename,"r");
  if(NULL == fp){
    printf("open file [%s] failed!\n",filename);
    return NULL;
  }

  while(!feof(fp)){
    char str[1024];
    memset(str,'\0',sizeof(str));
    if(NULL == fgets(str,sizeof(str),fp))
      break ;
    size += strlen(str);
    if(size > Max_Buf)
      break;
    strcat(outstr,str);
  }

  fclose(fp);
  DEBUG_PRINT("exit zcas_ReadAllFromFile,%d\n",size);
  return outstr;
}

int zcas_stringlist_addtail(stringList **list,char *str)
{
  /* printf("[DEBUG]enter zcas_stringlist_addtail\n"); */
  stringList *tmp= NULL;
  stringList *last = NULL;
  stringList *newelement = NULL;
  if (NULL == *list){
    newelement = (stringList *)malloc(sizeof(stringList));
    newelement->string = (char *)malloc(strlen(str)+1);
    memset(newelement->string,'\0',strlen(str)+1);
    strcpy(newelement->string,str);
    newelement->next = NULL;
    newelement->prev = NULL;
    *list = newelement;
    /* printf("[DEBUG]exit zcas_stringlist_addtail.0\n"); */
    return 0;
  }

  for(tmp=*list;tmp!=NULL;tmp=tmp->next){
    last = tmp;
  }

  newelement = (stringList *)malloc(sizeof(stringList));
  newelement->string = (char *)malloc(strlen(str)+1);
  memset(newelement->string,'\0',strlen(str)+1);
  strcpy(newelement->string,str);
  newelement->next = NULL;
  newelement->prev = last;
  last->next = newelement;
  /* printf("[DEBUG]exit zcas_stringlist_addtail\n"); */
  return 0;

}

int zcas_doublelist_addtail(doublelist **list,double element)
{
  doublelist *tmp= NULL;
  doublelist *last = NULL;
  doublelist *newelement;
  /* printf("[DEBUG]enter doublelist_pushback\n"); */
  if (NULL == *list){
    newelement = (doublelist *)malloc(sizeof(doublelist));
    memset(newelement,0,sizeof(doublelist));
    newelement->value = element;
    newelement->next = NULL;
    newelement->prev = NULL;
    *list = newelement;
    /* printf("[DEBUG]exit doublelist_pushback.0\n"); */
    return 0;
  }

  for(tmp=*list;tmp!=NULL;tmp=tmp->next){
    last = tmp;
  }

  newelement = (doublelist *)malloc(sizeof(doublelist));
  memset(newelement,0,sizeof(doublelist));
  newelement->value = element;
  newelement->next = NULL;
  newelement->prev = last;
  last->next = newelement;
  /* printf("[DEBUG]exit doublelist_pushback\n"); */
  return 0;
}

int zcas_conjwordlist_addtail(conjwordlist **list,CONJWORD element)
{
  /* printf("[DEBUG]enter zcas_UTHASConword_pushback\n"); */
  conjwordlist *tmp= NULL;
  conjwordlist *last = NULL;
  conjwordlist *newelement = NULL;

  newelement = (conjwordlist *)malloc(sizeof(conjwordlist));
  memset(newelement,0,sizeof(conjwordlist));
  memset(newelement->word.key,'\0',sizeof(newelement->word.key));
  strcpy(newelement->word.key,element.key);
  newelement->word.value = element.value;

  if (NULL == *list){
    newelement->next = NULL;
    *list = newelement;
  }
  else{
	for(tmp=*list;tmp!=NULL;tmp=tmp->next){
	 last = tmp;
	}
	newelement->next = NULL;
	last->next = newelement;
  }
  return 0;
}

int zcas_stringlist_release(stringList *list)
{
  while(list != NULL){
    stringList *tmp = list->next;
    free(list->string);
    free(list);
    list = tmp;
  }
  return 0;
}

int zcas_doublelist_release(doublelist *list)
{
  while(list != NULL){
    doublelist *tmp = list->next;
    free(list);
    list = tmp;
  }
  return 0;
}

int zcas_conjwordlist_release(conjwordlist *list)
{
  while(list != NULL){
    conjwordlist *tmp = list->next;
    free(list);
    list = tmp;
  }
  return 0;
}

conjwordlist* zcas_conjwordlist_find(conjwordlist *list,char *key)
{
  while(list != NULL){
    if(0 == strcmp(list->word.key,key))
      return list;
    list = list->next;
  }
  return NULL;
}

/* 计算排列组合的值C[n][m] */
int zcas_getcombineval(int n,int m)
{
  int a = 0;
  int b = 0;
  int c = 0;
  int ret = -1;
  DEBUG_PRINT("enter zcas_getcombineval:C[%d,%d]\n",n,m);
  if(0 == n || 0 == m || n < m){
    DEBUG_PRINT("zcas_getcombineval:C[%d,%d]=%d\n",n,m,0);
    return 0;
  }
  
  a = zcas_getfactorial(n);
  b = zcas_getfactorial(m);
  c = zcas_getfactorial(n-m);
  if(0 == b*c){
    DEBUG_PRINT("zcas_getcombineval exception:C[%d,%d]=%d\n",n,m,0);
    return 0;
  }
  ret = a/(b*c);
  DEBUG_PRINT("zcas_getcombineval:C[%d,%d]=%d\n",n,m,ret);
  return ret;
}

/* 计算n的阶乘，只支持计算0-10的阶乘 */
int zcas_getfactorial(int n)
{
  int tmp = n;
  int m = 1;
  if(n <= 0 || n > 10){
    DEBUG_PRINT("zcas_getfactorial:!%d=0\n",tmp);
    return 0;
  }

  while(n > 0){
    m *= n;
    n--;
  }
  
  DEBUG_PRINT("zcas_getfactorial:!%d=%d\n",tmp,m);
  return m;
}

/* 获取utf-8下字符所占的字节数 */
int zcas_getbytesnum(char ch)
{
  unsigned char n = ch;
  int num = 0;
  if(0 ==  n>>7<<7){
    num = 1;
  }
  else if(0xf0 == n>>3<<3){
    num = 4;
  }
  else if(0xe0 == n>>4<<4){
    num = 3;
  }
  else if(0xc0 == n>>5<<5){
    num = 2;
  }
  return num;
}

/* 获得中文字数 */
int zcas_getwordcount(char *str)
{
  int count = 0;
  int i = 0;
  int len = strlen(str);
  while(i < len){
    int n = zcas_getbytesnum(str[i]);
    if(n <= 0){
      DEBUG_PRINT("getwordcount:get word bytes failed.[pos=%d,len=%d]\n",i,len);
      DEBUG_PRINT("getwordcount:%s\n",str);
      break;
    }
    else if(n >=3){
      count ++;
    }
    i += n;
  }

  return count;
}

#ifndef OS_LINUX
/* windows上unicode与utf8之间的转换 */
wchar_t* zcas_TransformUTF8ToUnicodeM(const char* _str)
{
	int textlen =0;
	wchar_t * result = NULL;

	if (_str){
		textlen = MultiByteToWideChar(CP_UTF8,0,_str,-1,NULL,0);
		result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));
		memset(result,0,(textlen+1)*sizeof(wchar_t));
		MultiByteToWideChar(CP_UTF8,0,_str,-1,(LPWSTR)result,textlen);
	}
	return result;
}

char* zcas_TransformUnicodeToUTF8M(const wchar_t* _str)
{
	char* result = NULL;
	int textlen = 0;

	if (_str){
		textlen = WideCharToMultiByte(CP_UTF8,0,_str,-1,NULL,0,NULL,NULL);
		result =(char *)malloc((textlen+1)*sizeof(char));
		memset(result,0,sizeof(char)*(textlen+1));
		WideCharToMultiByte(CP_UTF8,0,_str,-1,result,textlen,NULL,NULL );
	}
	return result;
}
#endif

#ifndef OS_LINUX
/* 功能：检测字符串编码格式 ICU
 * 参数：
 * data：需要探测的字符串 
 * len：探测字符串长度 
 * detected ：探测的最有可能的字符编码名称, 调用者需要释放该字段 
 * 返回值：0-成功，其他-失败
**/  
int zcas_detectTextEncoding(const char *data, int32_t len, char **detected)  
{  
    UCharsetDetector* csd;  
    const UCharsetMatch **csm;  
    int32_t match = 0;
	int32_t matchCount = 0;  
  
    UErrorCode status = U_ZERO_ERROR;  
  
    csd = ucsdet_open(&status);  
    if(status != U_ZERO_ERROR)  
        return -1;  
   
    ucsdet_setText(csd, data, len, &status);  
    if(status != U_ZERO_ERROR)  
        return -1;  
  
    csm = ucsdet_detectAll(csd, &matchCount, &status);  
    if(status != U_ZERO_ERROR)  
        return -1;  
  
	//打印出探测的可能的编码  
	if(gprintlevel){
        for(match = 0; match < matchCount; match += 1)   
        {  
            const char *name = ucsdet_getName(csm[match], &status);  
            const char *lang = ucsdet_getLanguage(csm[match], &status);  
            int32_t confidence = ucsdet_getConfidence(csm[match], &status);  
        
            if (lang == NULL || strlen(lang) == 0)  
                    lang = "**";  
        
            printf("%s (%s) %d\n", name, lang, confidence);  
        }
	}
  
  
    if(matchCount > 0){  
        *detected = strdup(ucsdet_getName(csm[0], &status)); //分配了内存， 需要释放  
		//free(*detected);
        if(status != U_ZERO_ERROR)  
            return -1;  
    }  
  
    DEBUG_PRINT("charset = %s\n", *detected);
 
    ucsdet_close(csd);
    return 0;
}  
  
/* 功能:编码转换
 * 参数：
 * toConverterName      转换后的字符编码 
 * fromConverterName    转换前的字符编码 
 * target               存储转换后的字符串， 传出参数 
 * targetCapacity       存储容量，target的大小 
 * source               需要转换的字符串 
 * sourceLength         source的大小 
 * 返回值：错误码
**/  
int zcas_convert(const char *toConverterName,
	const char *fromConverterName,  
    char *target, int32_t targetCapacity,
	const char *source,
	int32_t sourceLength)  
{  
    UErrorCode error = U_ZERO_ERROR;  
    ucnv_convert(toConverterName, fromConverterName, target, targetCapacity, source, sourceLength, &error);  
    return error;  
}  
#endif

void zcas_printmemory(void *pt,int nsize)
{
  char *buf = pt;
  int i = 0;

  for(i=0;i<nsize;i++){
    printf("%2x",buf[i]);
  }
  printf("\n");

  return;
}

void zcas_gettime(char *s)
{
#ifdef OS_LINUX
  struct timeval tv;
  time_t now;
  struct tm *timenow;
  if(gettimeofday(&tv,NULL) < 0)
    return;

  time(&now);
  timenow = localtime(&now);
  sprintf(s,"%d-%d-%d %d:%d:%d.%d",
	  (timenow->tm_year+1900),
	  timenow->tm_mon+1,
	  timenow->tm_mday,
	  timenow->tm_hour,
	  timenow->tm_min,
	  timenow->tm_sec,
	  tv.tv_usec/1000);
#else
#endif
}

/*功能：计算buf是否匹配正则表达式pattern
**参数：buf-计算字符串；pattern-正则表达式
**返回值：0-不匹配，1-匹配
*/
int zcas_regmatch(char *buf,char *pattern)
{
#ifdef OS_LINUX
  int status,i;
  int cflags = REG_EXTENDED;
  size_t nmatch = 1;
  regmatch_t pmatch[1];
  regex_t reg;
  int bmatch = -1;

  regcomp(&reg,pattern,cflags);
  status = regexec(&reg,buf,nmatch,pmatch,0);
  if(REG_NOMATCH == status){
    bmatch = 0;
    printf("No Match:\n");
  }
  else if(0 == status){
    bmatch =1;
    printf("Match:\n");
    for(i=pmatch[0].rm_so;i<pmatch[0].rm_eo;++i)
      putchar(buf[i]);
    printf("\n");
  }
    
  regfree(&reg);
  return bmatch;
#else
	return 0;
#endif
}


/*功能：判断buf是否与pmatch格式匹配
**参数：buf-输入字符串，pmatch-格式字符串
**返回值：0-不匹配，1-匹配
*/
int zcas_stringmatch(char *buf,char *pmatch)
{
  int bmatch = 1;
  const char *sepchr = "_";
  char *str = NULL;
  char *tmp = NULL;
  char *tbuf = NULL;
  char pmatchbuf[1024] = {'\0'};
  int lenth = 0;
  int pos = 0;

  if(NULL == buf || NULL == pmatch){
    printf("zcas_stringmatch:para is null[0x%x,0x%x].\n",buf,pmatch);
    return 0;
  }

  strcpy(pmatchbuf,pmatch);
  lenth = strlen(buf);
  tbuf = buf;
  str = strtok(pmatchbuf,sepchr);
  while(NULL != str){
    tmp = strstr(tbuf,str);
    if(NULL == tmp){
      bmatch = 0;
      break;
    }
    pos = tmp - tbuf;
    if(0 == pos && (tbuf - buf)>0){
      tbuf++;
      continue;
    }
    tbuf = tmp;
    str = strtok(NULL,sepchr);
  }
  return bmatch;
}






