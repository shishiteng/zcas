#include "zcas_vector.h"
#include "zcas_sentence.h"
#include "zcas_common.h"

/*功能：获取关键词,分层，变量命名
**参数：config-zcas句柄，parts-所有part，title-doc的标题，可为NULL
**返回值：
*/
int zcas_getPartKeywords(zcas_config *config,zcas_part *parts,char *title)
{
  zcas_part *part = NULL;
  zcas_parag *para = NULL;
  zcas_sentence *sent = NULL;
  zcas_word *word = NULL;
  UTHASH_DOUBLE *keywords = NULL;
  UTHASH_DOUBLE *wtwd = NULL;

  if(NULL == config || NULL == parts){
    DEBUG_PRINT("zcas_getPartKeywords:input invalid,(0x%x,0x%x)\n",config,parts);
    DEBUG_PRINT("exit zcas_getPartKeywords.\n");
    return -1;
  }

  DEBUG_PRINT("enter getPartKeywords.\n");
  for(part=parts;part!=NULL;part=part->next){
    part->hashwords = NULL;
    for(para=part->parags;para!=NULL;para=para->next){
      para->mold = 0;
      for (sent=para->sents; sent!=NULL; sent=sent->next){
	    int n = 1;
	    sent->mold=0;
	    //add 2013.12.17,文章是新闻时，出现“据。。。报道”的句子加权
	    if(TYPE_NEWS == config->articletype){
	      char pmatch[1024] = {'\0'};
	      strcpy(pmatch,"据_报道");
	      if(zcas_stringmatch(sent->text,pmatch)){
		n = 2;
		DEBUG_PRINT("news string matched,sent:%s match string:%s\n",sent->text,pmatch);
	      }
	    }
	    for (word=sent->words; word!=NULL; word=word->next){
	      double value = 0;
	      double weight = 0;
	      char *key = word->text;
	      if (!word->isKey){
		word->weight=0;
	      }
	      /* 根据权重词典调整权重 */
	      if(word->weight>0 && word->isKey){
		if(zcas_UTHASH_DOUBLE_find(config->weightwords,key,&value))
		  word->weight = word->weight*value;
		/* 如果该词语在标题中出现，权重加倍 */
		if(title != NULL && NULL != strstr(title,key)){ 
		  word->weight *= 2;
		} 
		/* 段落首句话加权 */
		if(config->topicsent>0 && sent == para->sents){
		  word->weight *= 2;
		}
		/* 新闻类文章，出现"据...报道"的字眼，权重加倍 */
		word->weight *= n;
		/* 主轴权重 */
		if(zcas_UTHASH_DOUBLE_find(part->hashwords,key,&value))
		  weight = value + word->weight;
		else
		  weight = word->weight;
		zcas_UTHASH_DOUBLE_add(&part->hashwords,key,weight);
	      }
	      sent->mold+=word->weight*word->weight;
	      para->mold+=word->weight*word->weight;
	    }
	    sent->mold=sqrt(sent->mold);
      }
    }
  }

  if(config->debugmode){
    DEBUG_PRINT("==========part->keywords begin:\n");
    zcas_dbg_PrintKeywords(parts);
    DEBUG_PRINT("==========part keywords end.\n");
  }

  DEBUG_PRINT("exit getPartKeywords.\n");
  return ZCAS_OK;
}

/*功能计算段落中句子的权重，获取相似度，分层
**参数：config-zcas句柄，parts-所有的part
**返回值：0-成功，其他-失败
*/
int zcas_getSimilarity(zcas_config *config,zcas_part *parts)
{
  zcas_part *part = NULL;
  zcas_parag *para = NULL;
  zcas_sentence *sent = NULL;

  DEBUG_PRINT("enter zcas_getSimilarity.\n");
  if(NULL == config || NULL == parts){
    DEBUG_PRINT("zcas_getSimilarity:input invalid,(0x%x,0x%x)\n",config,parts);
    DEBUG_PRINT("exit zcas_getSimilarity.\n");
    return -1;
  }

  for(part=parts;part!=NULL;part=part->next){
    for(para=part->parags;para!=NULL;para=para->next){
      for (sent=para->sents; sent!=NULL; sent=sent->next){
	    sent->weight=fabs(zcas_getCosine(part->hashwords,sent->words)); 
	    if(0 == sent->weight)
	      DEBUG_PRINT("cosine is 0，sent=%s\n",sent->text);
	    /* 降低含有分号、问号的句子的权重 */
	    if(NULL != strstr(sent->text,"；") || NULL != strstr(sent->text,"?") || NULL != strstr(sent->text,"？"))
	      sent->weight = sent->weight*0.7;
      }
    }
  }

  DEBUG_PRINT("exit zcas_getSimilarity.\n");
  return ZCAS_OK;
}

/* 归一化处理 */
int zcas_normalize_Weight(zcas_part *parts)
{
  double sum=0;
  doublelist *scores = NULL;
  UTHASH_MapDoubleToInt *rscores = NULL;/* weight到下标的映射 */

  zcas_part *part = NULL;
  zcas_parag *para = NULL;
  zcas_sentence *sent = NULL;

  doublelist *list = NULL;
  doublelist *tmp = NULL;
  int nsize = 0;
  double *sums;
  int i = 0;
  doublelist *it = NULL;
  UTHASH_MapDoubleToInt *tmp1 = NULL;

  DEBUG_PRINT("enter zcas_normalize_Weight.\n");
  if(NULL == parts){
    DEBUG_PRINT("zcas_normalize_Weight:input invalid.\n");
    DEBUG_PRINT("exit zcas_getPartKeywords.\n");
    return -1;
  }

  for(part=parts;part!=NULL;part=part->next){
    for(para=part->parags;para!=NULL;para=para->next){
	  for(sent=para->sents;sent!=NULL;sent=sent->next){
	    zcas_doublelist_addtail(&scores,sent->weight);
	    /* 可优化，一个数组即可 */
	  }
    }
  }

  /* 排序,升序 */
  for(list=scores;list!=NULL;list=list->next){
    for(tmp=list->next;tmp!=NULL;tmp=tmp->next){
      if(list->value > tmp->value)
	  {
	    double val = list->value;
	    list->value = tmp->value;
	    tmp->value = val;
	  }
    }
  }

  for(tmp=scores;tmp!=NULL;tmp=tmp->next)
    nsize++;

  sums = (double *)malloc(sizeof(double)*nsize);
  for(it=scores;it!=NULL;it=it->next){
	double key;
	UTHASH_MapDoubleToInt *val = NULL;
    sum=sum+it->value;
    sums[i]=sum;
    val = (UTHASH_MapDoubleToInt*)malloc(sizeof(UTHASH_MapDoubleToInt));
    val->key = it->value;
    val->value = i;
    key = val->key;
    HASH_ADD(hh,rscores,key,sizeof(double),val);
    i++;
  }
  
  /* DEBUG_PRINT("sum=%lf\n",sum); */
  for(part=parts;part!=NULL;part=part->next){
    for(para=part->parags;para!=NULL;para=para->next){
      i=0;
      for(sent=para->sents;sent!=NULL;sent=sent->next){
	    UTHASH_MapDoubleToInt *val = NULL;
	    HASH_FIND(hh,rscores,&sent->weight,sizeof(double),val);
	    if(NULL != val){
	      if(0 == sum)
		sent->weight=0;
	      else{
		sent->weight=sums[val->value]/sum;
		DEBUG_PRINT("%s[%lf]\n",sent->text,sent->weight);
	      }
	      i++;
	    }
      }
    }
  }

  free(sums);
  zcas_doublelist_release(scores);
  tmp1 = rscores;
  while(tmp1 != NULL){
    UTHASH_MapDoubleToInt *tmp22 = tmp1;
    tmp1=(UTHASH_MapDoubleToInt *)tmp1->hh.next;
    HASH_DEL(rscores,tmp22);
    free(tmp22);
  }

  DEBUG_PRINT("exit zcas_normalize_Weight.\n");
  return ZCAS_OK;
}

/*功能：计算句子与主轴的夹角
**参数：hashwords-主轴，seg-句子词语链表
**返回值：夹角
*/
double zcas_getCosine(UTHASH_DOUBLE *hashwords,zcas_word *seg)
{
	zcas_word *word = NULL;

	double mult=0;
	double ret,mold1,mold2;
	
	int m = HASH_COUNT(hashwords);
	int n = 0;
	DEBUG_PRINT("enter zcas_getCosine.\n");
	if (NULL == hashwords|| NULL == seg){
	  DEBUG_PRINT("input para is null!(0x%x,0x%x)",hashwords,seg);
	  return 0;
	}

	for (word=seg; word!=NULL; word=word->next){
	  double value = 0;
	  if(!word->isKey)
	    continue;
	  if(zcas_UTHASH_DOUBLE_find(hashwords,word->text,&value)){
	    mult = mult+word->weight*value;
	  }
	  n++;
	}

    ret=0;
    mold1=zcas_getMoldEx(hashwords);
    mold2=zcas_getMold(seg);
    if(mold1 * mold2==0)
      ret = 0;
    else{
      ret = mult/(mold1*mold2);
      /* 对于过短的句子，加大它与主轴的夹角 */
      if(m < 200){
	    double t = (double)n/m;
	    t = pow(t,0.3);
	    ret = ret*t;
      }
    }
    ret= fabs(ret);

    DEBUG_PRINT("exit zcas_getCosnie.\n");

	return ret;
}

/*功能：计算句子之间的夹角
**参数：seg1-句子1，seg-句子2
**返回值：夹角
*/
double zcas_getCosineEx(zcas_word *seg1, zcas_word *seg2)
{
	zcas_word *sga = NULL;
	zcas_word *sgb = NULL;
	double mult=0;
    double ret=0;
    double mold1=0;
    double mold2=0;

	DEBUG_PRINT("enter zcas_getCosineEx.\n");
	if (NULL == seg1|| NULL == seg2){
	  DEBUG_PRINT("input para is null!(0x%x,0x%x)",seg1,seg2);
	  DEBUG_PRINT("enter zcas_getCosineEx,cos is 0.\n");
	  return 0;
	}

	for (sga=seg1; sga!=NULL; sga=sga->next){
	  for (sgb=seg2; sgb!=NULL; sgb=sgb->next){
	    if (0 == strcmp(sga->text,sgb->text)){
	      mult=mult+sga->weight*sgb->weight;
	    }
	  }
	}

    mold1 = zcas_getMold(seg1);
    mold2 = zcas_getMold(seg2);
    if(mold1 * mold2==0)
        ret = 0;
    else{
      ret = mult/(mold1*mold2);
    }
    ret= fabs(ret);
    DEBUG_PRINT("enter zcas_getCosineEx,cos:%lf\n",ret);
	return ret;
}

/*功能：计算句子的模
**参数:seg-句子的word链表
**返回值：模
*/
double zcas_getMold(zcas_word *seg)
{
	zcas_word *sg = NULL;
	double mold=0;

	if(NULL == seg)
	  return 0;

	for (sg=seg; sg!=NULL; sg=sg->next){
	  mold+=sg->weight*sg->weight;
	}

    mold=sqrt(mold);
    return mold;
}

/*功能:计算句子的模
**参数：pwords-词语的map链表
**返回值：模
*/
double zcas_getMoldEx(UTHASH_DOUBLE *pwords)
{
  double mold=0;
  UTHASH_DOUBLE *tmp = pwords;

  if(NULL == pwords){
    return 0;
  }

  while(tmp != NULL){
	mold+=tmp->value*tmp->value;
    tmp = (UTHASH_DOUBLE *)tmp->hh.next;
  }

  mold=sqrt(mold);
  return mold;
}

/*功能：根据词频调整权重
**参数：config-zcas句柄，parts-所有part
**返回值：0-成功，其他-失败
*/
int zcas_adjustweightbyfreq(zcas_config *config,zcas_part *parts)
{
  zcas_part *part = NULL;
  zcas_parag *para = NULL;
  zcas_sentence *sent = NULL;
  zcas_word *word = NULL;
  double maxfreq = 0;
  char maxkey[] = "MAXFREQ";

  DEBUG_PRINT("enter zcas_adjustweihtbyfreq\n");
  if(NULL == config || NULL == parts){
    DEBUG_PRINT("zcas_adjustweightbyfreq:input invalid(0x%x,0x%x).\n",config,parts);
    DEBUG_PRINT("exit zcas_adjustweghtbyfreq.\n");
    return -1;
  }

  if(zcas_UTHASH_DOUBLE_find(config->freqwords,maxkey,&maxfreq)){
    DEBUG_PRINT("maxfreq=%lf\n",maxfreq);
  }
  else{
    printf("find freqword failed!\n");
    return ZCAS_ERROR;
  }
  
  for(part=parts;part!=NULL;part=part->next){
	UTHASH_DOUBLE *tmp = NULL;
    for(para=part->parags;para!=NULL;para=para->next){
      for(sent=para->sents;sent!=NULL;sent=sent->next){
	    for(word=sent->words;word!=NULL;word=word->next){
	      char *key = word->text;
	      int n;
	      double freqratio,value;
	      if(zcas_UTHASH_DOUBLE_find(config->freqwords,key,&value)){
		if(100 >= value) continue;
		n = zcas_getwordcount(key);
		freqratio = pow(maxfreq/value*n,0.1);
		word->weight = word->weight * freqratio;
	      }
	    }
      }
    }
    tmp = part->hashwords;
    while(tmp != NULL){
      int n;
      double freqratio;
      double weight = 0;
      double value = 0;
      char *key = tmp->key;
      if(zcas_UTHASH_DOUBLE_find(config->freqwords,key,&value)){
	    /* 词频小于100的，不做调整，2013.7.15 */
	    if(100 >= value){
	      tmp = (UTHASH_DOUBLE *)tmp->hh.next;
	      continue;
	    }
	    /* 调整词频加入 */
	    n = zcas_getwordcount(key);
	    freqratio =pow(maxfreq/value*n,0.1);
	    weight = tmp->value * freqratio;
	    zcas_UTHASH_DOUBLE_add(&part->hashwords,key,weight);
      }
      tmp = (UTHASH_DOUBLE *)tmp->hh.next;
    }
  }

  if(config->debugmode){
    DEBUG_PRINT("==========after zcas_adjustweightbyfreq,part->keywords begin:\n");
    zcas_dbg_PrintKeywords(parts);
    DEBUG_PRINT("==========after zcas_adjustweightbyfreq,part->keywords end.\n");
  }

  return ZCAS_OK;
}

/*功能：调试模式下，打印主轴词语
**参数：parts-所有part
**返回值：无
*/
void zcas_dbg_PrintKeywords(zcas_part *parts)
{
  UTHASH_DOUBLE *tmp = NULL;
  
  if(NULL == parts){
    return;
  }

  tmp = parts->hashwords;
  while(tmp != NULL){
    printf("%s,%.3lf_",tmp->key,tmp->value);
    tmp = (UTHASH_DOUBLE *)tmp->hh.next;
  }

  printf("\n");
  return;
}

/*功能：获取两个word2和word1的关联度
**参数：word1;word2;partconjwords-part的关联词链表
**返回值：关联度
*/
double zcas_getwordrelation(zcas_word *word1,zcas_word *word2,UTHASH_CONJWORDS *partconjwords)
{
  double relation = 0;
  conjwordlist *conjlist = NULL;
  conjwordlist *targetlist = NULL;
  
  if(NULL == word1 || NULL == word2 || NULL == partconjwords){
    DEBUG_PRINT("zcas_getwordrelation:input err(0x%x,0x%x,0x%x).\n",word1,word2,partconjwords);
    return 0;
  }

  if(!zcas_UTHASH_CONJWORDS_find(partconjwords,word1->text,&conjlist)){
    return 0;
  }

  targetlist = zcas_conjwordlist_find(conjlist,word2->text);
  if(NULL == targetlist){
    return 0;
  }

  relation = targetlist->word.value;
  if(relation > 1){
    relation = 1;
  }

  return relation;
}
