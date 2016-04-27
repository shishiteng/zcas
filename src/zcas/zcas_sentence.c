#include "zcas_sentence.h"
#include "zcas_common.h"
#include "zcas_vector.h"
#include "zcfc.h"

/*功能：获取所有part的分句
**参数：config-zcas句柄，parts-part链表
**返回值：0
*/
int zcas_getSentences(zcas_config *config,zcas_part *parts)
{
  zcas_part *part = NULL;
  zcas_parag *para = NULL;

  DEBUG_PRINT("[ZCAS_SENTENCE]enter zcas_getSentences\n");
  if(NULL == config || NULL == parts){
    printf("zcas_getSentences:input invalid(0x%x,0x%x).\n",config,parts);
    DEBUG_PRINT("[ZCAS_SENTENCE]exit zcas_getSentences\n");
    return -1;
  }
  for(part=parts;part!=NULL;part=part->next){
    zcas_getPartSents(config,part);
  }
  DEBUG_PRINT("[ZCAS_SENTENCE]exit zcas_getSentences\n");
  return ZCAS_OK;
}

/*功能：获取单个part的分句
**参数：
**返回值：0
*/
int zcas_getPartSents(zcas_config *config,zcas_part *part)
{
  zcas_parag *para = NULL;
  DEBUG_PRINT("enter zcas_getPartSents\n");
  if(NULL == config || NULL == part){
    printf("zcas_getPartSents:input invalid(0x%x,0x%x).\n",config,part);
    DEBUG_PRINT("exit zcas_getPartSents\n");
    return -1;
  }
  for(para = part->parags;para != NULL;para = para->next){
    zcas_getParagSents(config,para);
  }
  DEBUG_PRINT("exit zcas_getPartSents\n");
  return ZCAS_OK;
}

/*功能：获取一个para下的分句
**参数：
**返回值：0
*/
int zcas_getParagSents(zcas_config *config,zcas_parag *para)
{
  char *txt = NULL;
  unsigned int len = 0;
  zcas_sentence *sent = NULL;
  char *chr = NULL;

  DEBUG_PRINT("enter zcas_getParagSents\n");
  if(NULL == config || NULL == para){
    printf("zcas_getParagSents:input invalid(0x%x,0x%x).\n",config,para);
    DEBUG_PRINT("exit zcas_getParagSents.\n");
    return -1;
  }

  txt = para->text;
  para->sents = NULL;
  while (len < strlen(para->text)){
    unsigned int spacecnt = 0;/* 空格数 */
    sent = zcas_getFirstSent(config,txt);
    /* 当一段话没有标点时把它当作一句话来处理 */
    if (NULL == sent){
      if(strlen(txt) > 0 && zcas_getwordcount(txt) > 50){
        int strsize = strlen(txt)+1;
	    sent = (zcas_sentence*)malloc(sizeof(zcas_sentence));
	    memset(sent,0,sizeof(zcas_sentence));
	    sent->text = (char*)malloc(strsize);
	    memset(sent->text,'\0',strsize);
	    memcpy(sent->text,txt,strsize-1);
	    if(strlen(sent->text) != (unsigned int)(strsize-1)){
	      DEBUG_PRINT("get sent size faled:%s,%d,%d\n",sent->text,strsize,strlen(sent->text));
	      free(sent->text);
	      free(sent);
	      break;
	    }
	    sent->weight=0;
	    sent->words = NULL;
	    sent->lenth = zcas_getwordcount(sent->text);
	    sent->isfilt = 0;
      }
      else{
	    break;
      }
    }
   
    chr = sent->text;
    while (*chr++){
      char *tmp = strchr(chr,' ');
      if(tmp != NULL){
	    chr = tmp;
	    spacecnt++;
      }
    }
    if (spacecnt+4 < strlen(sent->text)){
      zcas_Sentlist_addtail(&para->sents,*sent);/* addtail */
    }
    txt += strlen(sent->text);
    len += strlen(sent->text);
    free(sent->text);
    free(sent);
  }
  
  if(1 == config->debugmode){
    zcas_sentence *p = NULL;
    for(p=para->sents; p != NULL;p=p->next){
      DEBUG_PRINT("Sentence:%s,wt=%lf\n",p->text,p->weight);
    }
  }
  DEBUG_PRINT("exit zcas_getParagSents\n");

  return 0;
}

/*功能：判断字符前后是否为小数
**参数：chr-输入字符指针
**返回值：0-不是小数，1-是小数
** 注：该函数有越界的风险,需在调用时做合法性检查
*/
int zcas_isDecimal(char *chr)
{
  /* DEBUG_PRINT("enter zcas_isDecimal\n"); */
  char ch,prev,next;
  if(NULL == chr){
    return 0;
  }
  ch = chr[0];
  if(ch == '.'){
    prev = *(chr-1);
    next = *(chr+1);
    if((prev >= '0' && prev <= '9') && (next >= '0' && next <= '9')){
      return 1;
    }
  }
  return 0;
}

/*功能：判断引号、括号等是否匹配
**参数：text-输入字符串，punct-标点字符串
**返回值：0-不匹配，1-匹配
*/
int zcas_isPunctMatched(char *text,char *punct)
{
  int n = 0;
  char *matchpunct = NULL;
  char *lpunct = NULL;
  char *rpunct = NULL;
  int i = 0;

  if(NULL == text || NULL == punct){
    DEBUG_PRINT("zcas_isPunctMatched:input err(0x%x,0x%x).",text,punct);
    return 1;
  }

  n = zcas_getbytesnum(punct[0]);
  if(0 == n){
    return 1;
  }
  matchpunct = (char *)malloc(n+1);
  memset(matchpunct,'\0',n+1);
  if(0 == strcmp(punct,"(")){
    strcpy(matchpunct,")");
  }
  else if(0 == strcmp(punct,"（")){
    strcpy(matchpunct,"）");
  }
  else if(0 == strcmp(punct,"“")){
    strcpy(matchpunct,"”");
  }
  
  lpunct  = strstr(text,punct);
  rpunct  = strstr(text,matchpunct);

  while(NULL != lpunct){
    i++;
    lpunct = lpunct+n;
    lpunct = strstr(lpunct,punct);
  }

  while(NULL != rpunct){
    i--;
    rpunct = rpunct+n;
    rpunct = strstr(rpunct,matchpunct);
  }
  
  free(matchpunct);

  if(0 == i)
    return 1;
  else
    return 0;
}

/*功能：获取字符串中的第一个句子
**参数：config-zcas句柄，txt-输入文本
**返回值：句子指针
*/
zcas_sentence *zcas_getFirstSent(zcas_config *config,const char *txt)
{
  /* DEBUG_PRINT("enter zcas_getFirstSent\n"); */
  char *text = NULL;
  zcas_sentence *sent = NULL;

  if(NULL == config || NULL == txt){
    DEBUG_PRINT("zcas_getFirstSent:input err(0x%x,0x%x).",config,txt);
    return NULL;
  }

  text = zcas_getTextofFirstSent(config,txt);
  if(NULL == text)
    return NULL;
  sent = (zcas_sentence*)malloc(sizeof(zcas_sentence));
  memset(sent,0,sizeof(zcas_sentence));
  sent->text = (char*)malloc(strlen(text)+1);
  memset(sent->text,'\0',strlen(text)+1);
  memcpy(sent->text,text,strlen(text));
  sent->weight=0;
  sent->words = NULL;
  sent->lenth = zcas_getwordcount(sent->text);
  sent->isfilt = 0;
  free(text);
  return sent;
}

/*功能：获得文本中的第一个句子(字符串)
**参数：
**返回值：句子字符串
*/
char *zcas_getTextofFirstSent(zcas_config *config,const char *txt)
{
  int i = 0;
  char *sent = NULL;
  int n = strlen(txt);
  int foundsent = 0;

  if(NULL == config || NULL == txt){
    DEBUG_PRINT("zcas_getTextofFirstSent:input err(0x%x,0x%x).",config,txt);
    return NULL;
  }

  while(i<n){
	int j = 0;
	char *w = NULL;
    int s = zcas_getbytesnum(txt[i]);
    if(s <= 0){
      break;
    }
	
    w = (char *)malloc(s+1);
    memset(w,'\0',s+1);
    for(j=0;j<s;j++){
      w[j] = txt[i+j];
    }
	
    if(strchr(config->epuncts,txt[i])){
	  int count;
	  char *text = (char *)malloc(i+1);
	  if(i>= 1 && (i+1)<n){
	    if(zcas_isDecimal((char*)(&txt[i]))){
	      i += s;
	      free(w);
	      free(text);
	      continue;
	    }
	  }
      memset(text,'\0',i+1);
      strncpy(text,txt,i);
      if(!zcas_isPunctMatched(text,"(") || !zcas_isPunctMatched(text,"（")
	   /* || !zcas_isPunctMatched(text,"“") */){
	    count = zcas_getwordcount(text);/* 句子长度大于100时，忽略不匹配的括号 */
	    i += s;
	    if(count > 100){
	      foundsent = 1;
	      free(w);
	      free(text);
	      break;
	    }
	    DEBUG_PRINT("[punct is not matched]%s\n",text);
	    free(w);
	    free(text);
	    continue;
      }
      i += s;
	  free(w);
	  free(text);
      foundsent = 1;
      break;
    }
    if(strstr(config->cpuncts,w)){
      int count;
      char *text = (char *)malloc(i+1);
      memset(text,'\0',i+1);
      strncpy(text,txt,i);
      if(!zcas_isPunctMatched(text,"(") || !zcas_isPunctMatched(text,"（")
	 /*|| !zcas_isPunctMatched(text,"“")*/){
	    count = zcas_getwordcount(text);/* 句子长度大于100时，忽略不匹配的括号 */
	    i += s;
	    if(count > 100){
	      foundsent = 1;
	      free(w);
	      free(text);
	      break;
	    }
	    DEBUG_PRINT("[punct is not matched]%s\n",text);
	    free(w);
	    free(text);
	    continue;
      }
      i += s;
	  free(w);
	  free(text);
      foundsent = 1;
      break;
    } 
    i += s;
	free(w);
  }

  if(0 == foundsent){
    return NULL;
  }
  else{
    char *sent = (char *)malloc(i+1);
    memset(sent,'\0',i+1);
    strncpy(sent,txt,i);
    return sent;
  }
}


/*功能：获取所有part的分词
**参数：
**返回值：0
*/
int zcas_getWords(zcas_config *config,zcas_part *parts)
{
  zcas_part *part = NULL;
  DEBUG_PRINT("enter zcas_getWords\n");
  if(NULL == config || NULL == parts){
    DEBUG_PRINT("zcas_getWords:input err(0x%x,0x%x).",config,parts);
    return -1;
  }

  for(part=parts;part!=NULL;part=part->next){
    zcas_getPartWords(config,part);
  }
  DEBUG_PRINT("exit zcas_getWords\n");
  return ZCAS_OK;
}

/*功能：获取单个part下句子的分词
**参数：
**返回值：0
*/
int zcas_getPartWords(zcas_config *config,zcas_part *part)
{
  zcas_parag *para = NULL;
  DEBUG_PRINT("[ZCAS_SENTENCE]enter zcas_getPartWords\n");

  if(NULL == config || NULL == part){
    return -1;
  }
  for(para = part->parags;para != NULL;para = para->next){
    zcas_getParagWords(config,para);
  }
  return 0;
}

/*功能：获取单个段落下句子的分词
**参数：
**返回值：0
*/
int zcas_getParagWords(zcas_config *config,zcas_parag *parag)
{
  zcas_sentence *sent = NULL;

  DEBUG_PRINT("[ZCAS_SENTENCE]enter zcas_getParagWords\n");
  if(NULL == config || NULL == parag){
    return -1;
  }
  for(sent=parag->sents;sent != NULL;sent=sent->next){
    zcas_getSentWords(config,sent);
  }
  return 0;
}

/*功能：获取一个句子下的分词
**参数：
**返回值：0-成功，其他-失败
*/
int zcas_getSentWords(zcas_config *config,zcas_sentence *sent)
{
  int zcwn = 0,i=0;
  zcfc_oword *words;

  if(NULL == config || NULL == sent){
    DEBUG_PRINT("[ZCAS_SENTENCE]input is null.\n");
    return -1;
  }
  DEBUG_PRINT("[ZCAS_SENTENCE]enter zcas_getSentWords\n");
  sent->words = NULL;
  if(NULL == config->zcfchandle || NULL == sent || NULL == sent->text){
    DEBUG_PRINT("getSentWords:input paras error[0x%x,0x%x,0x%x]\n",config->zcfchandle,sent,sent->text);
    DEBUG_PRINT("[ZCAS_SENTENCE]exit zcas_getSentWords\n");
    return -1;
  }

  words = zcas_zcfc_Split(config->zcfchandle,sent->text,(int)strlen(sent->text),&zcwn);/* 结构转换 */
  DEBUG_PRINT("[ZCAS_SENTENCE]getSentWords,word count:%d\n",zcwn);
  for (i=0;i<zcwn;i++){
	int found=0;
	zcas_word *word=NULL;
    zcas_word sg;
    memset(&sg,0,sizeof(sg));
    memcpy(sg.text,words[i].word,sizeof(sg.text));
    sg.prev = NULL;
    sg.next = NULL;
    sg.weight=1;
    sg.count =1;
    sg.isKey=0;
    sg.isKey=zcas_isKeyWord_str(words[i].pos);
    if(config->debugmode == 1)
      printf("%s,%s,%d ",sg.text,words[i].pos,sg.isKey);
    for(word = sent->words;word != NULL;word = word->next){
      if (0 == strcmp(word->text,sg.text)){
	    found=1;
	    word->weight++;
	    word->count++;
	    break;
      }
    }
    if (!found){
      zcas_Wordlist_addtail(&sent->words,sg);/* 数组 */
    }
  }
  zcfc_release_words(words);

  return ZCAS_OK;
}

/*功能：获取所有part的关联词
**参数：config-zcas句柄，parts-part链表
**返回值：0
*/
int zcas_getConjWords(zcas_config *config,zcas_part *parts)
{
  double HAS=555.555;/* 没有具体含义，作为标记 */
  UTHASH_DOUBLE *AllKWords = NULL;
  zcas_part *part= NULL;
  zcas_parag *para = NULL;
  zcas_sentence *sent = NULL;
  zcas_word *word = NULL;

  if(NULL == config || NULL == parts){
    return -1;
  }

  DEBUG_PRINT("enter zcas_getConjWords\n");
  DEBUG_PRINT("--------AllKeywords begin---------\n");
  /* 获取part所有词语，加入AllKWords,并统计词频 */
  for(part= parts; part!=NULL; part=part->next){
    for (para=part->parags; para!=NULL; para=para->next){
      for (sent=para->sents; sent!=NULL; sent=sent->next){
	    for(word=sent->words; word!=NULL; word=word->next){
	      if(word->isKey){
		zcas_UTHASH_DOUBLE_add(&AllKWords,word->text,HAS);
		if(1 == config->debugmode)
		  printf("%s_",word->text);
	      }
	    }
      }
    }
    if(1 == config->debugmode)
      printf("\n");
  }
  DEBUG_PRINT("--------AllKeywords end----------\n");

  /* printf("\n"); */
  /* 建立part的关联词存储结构，partconjwords */
  for(part= parts; part!=NULL; part=part->next){
    for (para=part->parags; para!=NULL; para=para->next){
      for (sent=para->sents; sent!=NULL; sent=sent->next){
	    for(word=sent->words; word!=NULL; word=word->next){
	      conjwordlist *list = NULL;
	      conjwordlist *partlist = NULL;
	      conjwordlist *tmp = NULL;
	      CONJWORD conjwd;
	      if(!word->isKey){
		continue;
	      }
	      if(!zcas_UTHASH_CONJWORDS_find(config->conjwords,word->text,&list) || 
		 zcas_UTHASH_CONJWORDS_find(config->partconjwords,word->text,&partlist)){
		continue;
	      }
	      for(tmp=list;tmp!=NULL;tmp=tmp->next){
		double wt = 0;
		conjwd = tmp->word;
		if(zcas_UTHASH_DOUBLE_find(AllKWords,conjwd.key,&wt) && HAS == wt){
		  zcas_conjwordlist_addtail(&partlist,conjwd);
		}
	      }
	      /* printf("\n"); */
	      if(NULL != partlist){
		zcas_UTHASH_CONJWORDS_add(&config->partconjwords,word->text,partlist);
	      }
	    }
      }
    }
  }

  if(1 == config->debugmode){
    UTHASH_CONJWORDS *tmp = NULL;
    printf("--------------partconjwords begin-----------\n");
    for(tmp = config->partconjwords;tmp!=NULL;tmp=(UTHASH_CONJWORDS *)tmp->hh.next){
      conjwordlist *list = NULL;
      printf("%s:\t",tmp->key);
      for(list=tmp->value;list!=NULL;list=list->next){
	    printf("/%s,%lf",list->word.key,list->word.value);
      }
      printf("\n");
    }
    printf("--------------partconjwords end------------\n");
    printf("--------------allwords begin------------\n");
    for(part= parts; part!=NULL; part=part->next){
      for (para=part->parags; para!=NULL; para=para->next){
	    for (sent=para->sents; sent!=NULL; sent=sent->next){
	      for(word=sent->words; word!=NULL; word=word->next){
		if(!word->isKey) continue;
		printf("/%s,%.3lf",word->text,word->weight);
	      }
	    }
      }
    }
    printf("\n--------------allwords end------------\n");
  }

  /* 根据关联词计算权重,未考虑关联词出现次数   */
  DEBUG_PRINT("--------------adjust by conjwords begin-----------\n");
  for(part= parts; part!=NULL; part=part->next){
    for (para=part->parags; para!=NULL; para=para->next){
      for (sent=para->sents; sent!=NULL; sent=sent->next){
	    for(word=sent->words; word!=NULL; word=word->next){
	      zcas_word *tmp = NULL;
	      if(!word->isKey) continue;
	      /* 根据关联度调整句子中分词的权重 */
	      for(tmp=sent->words;tmp!=NULL;tmp=tmp->next){
		double rel;
		if(tmp == word || !tmp->isKey) continue;
		rel = zcas_getwordrelation(word,tmp,config->partconjwords);
		word->weight += tmp->count*rel;
	      }
	    }
      }
    }
  }
  DEBUG_PRINT("--------------ajust by conjwords end-------------\n");

  if(config->debugmode){
    printf("--------------after adjusted by conjwords,allwords begin------------\n");
    for(part= parts; part!=NULL; part=part->next){
      for (para=part->parags; para!=NULL; para=para->next){
	    for (sent=para->sents; sent!=NULL; sent=sent->next){
	      for(word=sent->words; word!=NULL; word=word->next){
		if(!word->isKey) continue;
		printf("/%s,%.3lf",word->text,word->weight);
	      }
	    }
      }
    }
    printf("\n--------------after adjusted by conjwords,allwords end------------\n");
  }
  /* 释放资源：AllWords，partconjwords */
  zcas_UTHASH_DOUBLE_release(AllKWords);
 
  DEBUG_PRINT("exit zcas_getConjwords\n");
  return ZCAS_OK;
}

int zcas_Sentlist_addtail(zcas_sentence **sentlist,zcas_sentence sent)
{
  zcas_sentence *newsent;
  zcas_sentence *tmp,*last;
  /* DEBUG_PRINT("[ZCAS_SENTENCE]enter zcas_Sentlist_addtail\n"); */
  if(NULL == *sentlist)	{
    newsent =(zcas_sentence*)malloc(sizeof(zcas_sentence));
    memset(newsent,0,sizeof(zcas_sentence));
    *newsent = sent;
    newsent->text = (char*)malloc(strlen(sent.text)+1);
    memset(newsent->text,'\0',strlen(sent.text)+1);
    strcpy(newsent->text,sent.text);
    newsent->next = NULL;
    newsent->prev = NULL;
    *sentlist = newsent;
    return 0;
  }

  for(tmp=*sentlist;tmp!=NULL;tmp=tmp->next){
    last = tmp;
  }

  newsent =(zcas_sentence*)malloc(sizeof(zcas_sentence));
  memset(newsent,0,sizeof(zcas_sentence));
  *newsent = sent;
  newsent->text = (char*)malloc(strlen(sent.text)+1);
  memset(newsent->text,'\0',strlen(sent.text)+1);
  strcpy(newsent->text,sent.text);
  newsent->next = NULL;
  newsent->prev = last;
  last->next = newsent;
  /* DEBUG_PRINT("[ZCAS_SENTENCE]exit zcas_Sentlist_addtail\n"); */
  return 0;
}


int zcas_Wordlist_addtail(zcas_word **segwordlist,zcas_word segword)
{
  zcas_word *newsegword;
  zcas_word *tmp,*last;
  /* DEBUG_PRINT("enter zcas_wordlist_addtail\n"); */
  if(NULL == *segwordlist){
    newsegword =(zcas_word*)malloc(sizeof(zcas_word));
    memset(newsegword,0,sizeof(zcas_word));
    *newsegword = segword;
    newsegword->next = NULL;
    newsegword->prev = NULL;
    *segwordlist = newsegword;
    /* DEBUG_PRINT("exit zcas_wordlist_addtail.0\n"); */
    return 0;
  }

  for(tmp=*segwordlist;tmp!=NULL;tmp=tmp->next){
    last = tmp;
  }

  newsegword =(zcas_word*)malloc(sizeof(zcas_word));
  memset(newsegword,0,sizeof(zcas_word));
  *newsegword = segword;
  newsegword->next = NULL;
  newsegword->prev = last;
  last->next = newsegword;
  /* DEBUG_PRINT("exit zcas_wordlist_addtail\n"); */
  return 0;
}

int zcas_Sentlist_clear(zcas_sentence *sents)
{
  zcas_sentence *sent = NULL;
  zcas_sentence *tmpsent = NULL;
  for(sent=sents;sent!=NULL;){
    zcas_Wordlist_clear(sent->words);
    sent->words = NULL;
    tmpsent = sent->next;
    free(sent->text);
    free(sent);
    sent = tmpsent;
  }
  return 0;
}

int zcas_Wordlist_clear(zcas_word *words)
{
  zcas_word *word = NULL;
  for(word=words;word!=NULL;){
    zcas_word *tmpwd = word->next;
    free(word);
    word = tmpwd;
  }
  return 0;
}

WORD_ATTR zcas_getAttrib(char v)
{
  switch(v)
    {
    case 'N':
      return ATTRIB_NOUN;
    case 'P':
      return ATTRIB_PRON;
    default:
      return ATTRIB_UNKNOWN;
    }
}

char zcas_getChar(WORD_ATTR attr)
{
  /* DEBUG_PRINT("enter c_getChar\n"); */
  switch(attr)
    {
    case ATTRIB_UNKNOWN:
      return '0';
    case ATTRIB_NOUN:
      return 'n';
    case ATTRIB_TIME:
      return 't';
    case ATTRIB_LOCT:
      return 's';
    case ATTRIB_DIRECT:
      return 'f';
    case ATTRIB_VERB:
      return 'v';
    case ATTRIB_ADJ:
      return 'a';
    case ATTRIB_DIST:
      return 'b';
    case ATTRIB_STAT:
      return 'z';
    case ATTRIB_PRON:
      return 'r';
    case ATTRIB_NUM:
      return 'm';
    case ATTRIB_QUAN:
      return 'q';
    case ATTRIB_ADV:
      return 'd';
    case ATTRIB_PREP:
      return 'p';
    case ATTRIB_CONJ:
      return 'c';
    case ATTRIB_AUX:
      return 'u';
    case ATTRIB_EXC:
      return 'e';
    case ATTRIB_MOD:
      return 'y';
    case ATTRIB_ONOM:
      return 'o';
    case ATTRIB_PREF:
      return 'h';
    case ATTRIB_SUFF:
      return 'k';
    case ATTRIB_ASTR:
      return 'x';
    case ATTRIB_PUNC:
      return 'w';
    default:
      return '0';
    }
}

int zcas_isKeyWord(WORD_ATTR attr)
{
  switch(attr)
    {
    case ATTRIB_NOUN:
      return 1;
    case ATTRIB_TIME:
      return 1;
    case ATTRIB_LOCT:
      return 1;
    case ATTRIB_DIRECT:
      return 1;
    case ATTRIB_DIST:
      return 1;
    case ATTRIB_ASTR:
      return 1;
    default:
      return 0;
    }
}

int zcas_isKeyWord_str(char *attr)
{
  if(NULL == attr)
    return 0;
  else if(0 == strcmp(attr,"n")){
    return 1;
  }
  else
    return 0;
}

int zcas_isKeyWord_char(char v)
{
    switch(v)
    {
    case 'n':
        return 1;
    case 't':
        return 1;
    case 's':
        return 0;
    case 'f':
        return 1;
    case 'b':
        return 1;
    case 'x':
        return 1;
    default:
        return 0;
    }
}

int zcas_GetPunctPos(zcas_config *configAttr,const char* s)
{
	/* DEBUG_PRINT("enter zcas_GetPunctPos\n"); */
	/* DEBUG_PRINT("[zcas_GetPunctPos]cpunct=%s,epunct=%s\n",configAttr->cpuncts,configAttr->epuncts); */
    char w[4]; /*  定义一个数组，存放一个中文字符 */
    int i=0;
    char *tmp=NULL;
    int n=strlen(s);
    int foundSentence = 0;
    int PuncMatch = 1; /*  判断引号是否匹配 */
	int k;
	unsigned char t;

	if(NULL == configAttr || NULL == s){
	  return -1;
	}
    while(i<n){
		if(s[i]>0){
			/*  如果发现西文字符 */
			if(strchr(configAttr->epuncts,s[i])){
				/*  如果找到西文句末标点 */
				foundSentence=1;
				i++;
				break;
			}
			else /*  如果不是西文句末标点 */
				i++;
		}
		else{
			/*  如果是中文字符 */
			w[0]=s[i];
			w[1]=s[i+1];
			t=(unsigned char)w[0];
			k=2;
			w[2] = s[i+2];
			w[3] = 0;
			k=3;
			if(strstr(configAttr->cpuncts,w)){
				/*  如果是中文句末标点 */
				tmp = (char*)(s+i); /*  取出s中当前句末标点左边的部分 */
				if(NULL!=strstr(tmp,"“") && NULL==strstr(tmp,"”"))
					PuncMatch = 0; /*  如果在串中发现左引号，但没有发现右引号，则括号不匹配 */

				foundSentence = 1;
				i+=k;
				break;
			}
			else  /*  如果不是中文句末标点 */
				i+=k;
        }
    }

    if(!foundSentence){
        return 0;
    }

    while(i<n){
		if(s[i]>0){
			if(strchr(configAttr->epuncts,s[i]) || strchr(configAttr->erends,s[i]))
				i++;
			else
				return i;
		}
		else{
			w[0]=s[i];
			w[1]=s[i+1];
			t=(unsigned char)w[0];
			k=2;
			w[2] = s[i+2];
			w[3] = 0;
			k=3;

			if (strcmp(w,"”")==0)
				PuncMatch = 1;

			if(strstr(configAttr->cpuncts,w) || strstr(configAttr->crends,w))
				i+=k;
			else{
				if (PuncMatch){
					return i;
				}
				else
					i+=k;
			}
		}
    }
	if (PuncMatch){
		return n;
	}

	else{
		return 0;
	}
}

zcfc_oword* zcas_zcfc_Split(zcfc_handle* uhandle,char* input,int input_size, int* return_count)
{
  zcfc_oword* swords = NULL;
  if(NULL == uhandle || NULL == input || input_size <= 0 || NULL == return_count){
    return NULL;
  }
  swords = zcfc_run(uhandle,input,input_size,return_count);

  return swords;
#if 0
  /* 单字分词 */
  zcfc_word *swords;
  int count = 0;
  int i = 0;
  int len = strlen(input);
  while(i < len){
    int n = zcas_getbytesnum(input[i]);
    if(n >=3){
      count ++;
    }
    i += n;
  }
  swords = (zcfc_word*)malloc(sizeof(zcfc_word)*(count));
  i = 0;
  count = 0;
  while(i < len){
    int n = zcas_getbytesnum(input[i]);
    if(n >=3){
      char *w = (char *)malloc(n+1);
      memset(w,'\0',n+1);
      memcpy(w,input+i,n);
      memcpy(swords[count].ctype,"n",10);
      memcpy(swords[count].word,w,WORD_MAX_LENTH_AC);
      //swords[count].lenth = 3;
      //swords[count].type = ATTRIB_NOUN;/* swd->type; */
      swords[count].prev = NULL;
      swords[count].next = NULL;
      count++;
      free(w);
    }
    i += n;
  }

  *return_count = count;
  return swords;
#endif
}
