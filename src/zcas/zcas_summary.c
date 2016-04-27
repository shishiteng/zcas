#include "zcas_head.h"
#include "zcfc.h"

/*功能：获取摘要
**参数：config-zcas句柄，instr-输入字符串
**返回值：摘要结果
**注意：获取结果成功后，需要释放内存
*/
char *zcas_getsummary(zcas_config *config,char* instr)
{
	zcas_doc doc;
	char *outstr = NULL;
	char *tmp = NULL;
	unsigned int len = 0;/* windows error */
	double threld = 0;

	DEBUG_PRINT("[ZCAS_SUMMARY]enter zcas_getsummary.\n");
	if(NULL == config || NULL == instr){
	  printf("[ZCAS_SUMMARY]input para err!(0x%x,0x%x)",config,instr);
	  DEBUG_PRINT("[ZCAS_SUMMARY]exit zcas_getsummary.\n");
	  return NULL;
	}

	/* 过滤无效字符 */
	if(ZCAS_OK != zcas_removenullchars(config,instr)){
	  printf("[ZCAS_SUMMARY]过滤无效字符失败!\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_removenullchars end.\n");

	/* 获取doc结构 */
	memset(&doc,0,sizeof(zcas_doc));
	doc.text = (char*)malloc(strlen(instr)+1);
	memset(doc.text,'\0',strlen(instr)+1);
	strcpy(doc.text,instr);
	
	tmp = strstr(instr,"\n");
	len = tmp - instr;
	if(NULL != tmp && len <= title_Max_Len){
	  doc.title = (char *)malloc(len+1);
	  memset(doc.title,'\0',len+1);
	  strncpy(doc.title,instr,len);
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]get doc end.\n");

	/* 分part */
	if(ZCAS_OK != zcas_getPart(config,&doc)){
	  printf("分part失败！\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getPart end.\n");

	/* 分para */
	if(ZCAS_OK != zcas_getParas(config,doc.parts)){
	  printf("分段失败！\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getPara end.\n");

	/* 分句 */
    if(ZCAS_OK != zcas_getSentences(config,doc.parts)){
      printf("分句失败!\n");
    }
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getSentences end.\n");

	/* 分词 */
	if(ZCAS_OK != zcas_getWords(config,doc.parts)){
	  printf("分词失败！\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getWords end.\n");

	/* 获取关联词，权重加入关联度影响 */
	if(config->conjword){
	  if(ZCAS_OK != zcas_getConjWords(config,doc.parts)){
	    printf("获取关联词失败！\n");
	  }
    DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getConjWords end.\n");
	}

    /* 统计词频，初始化句子权重，获取主轴 */
	if(ZCAS_OK != zcas_getPartKeywords(config,doc.parts,doc.title)){
	  printf("获取part关键词失败！\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getPartKeywords end.\n");

	/* 根据词频调整权重 */
	if(config->freqword){
	  if(ZCAS_OK != zcas_adjustweightbyfreq(config,doc.parts)){
	    printf("[ZCAS_SUMMARY]adjustweightbyfreq error.\n");
	  }
	  DEBUG_PRINT("[ZCAS_SUMMARY]zcas_adjustweightbyfreq end.\n");
	}

	/* 计算相似度 */
	if(ZCAS_OK != zcas_getSimilarity(config,doc.parts)){
	  printf("[ZCAS_SUMMARY]计算相似度失败!\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getSimilarity end.\n");

	/* 归一化处理 */
	if(ZCAS_OK != zcas_normalize_Weight(doc.parts)){ 
	  printf("[ZCAS_SUMMARY]权重归一化失败!\n"); 
	} 
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_normalize_Weight end.\n");

	/* 获取输出阈 */
	if(ZCAS_OK != zcas_getThreshold(config,doc.parts)){
	  printf("[ZCAS_SUMMARY]get threshold failed!\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getThreshold end.\n");
	
	/* 输出结果 */
	outstr = zcas_outputsummary(doc.parts);/* config配置参数 */
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_outputsummary end.\n");
	if(NULL == outstr){
	  /* 返回wordcount */
	  DEBUG_PRINT("[ZCAS_SUMMARY]摘要失败，将输出一个字数少于%d的句子。\n",config->wordcount);
	  outstr = zcas_outputsummarybywordcount(doc.parts,config->wordcount);
	  if (NULL == outstr){
	    printf("[ZCAS_SUMMARY]获取摘要结果失败!\n");
	  }
	  DEBUG_PRINT("[ZCAS_SUMMARY]zcas_outputsummarybywordcount end.\n");
	}
	
	/* 释放内存 */
	if(ZCAS_OK != zcas_release_parts(doc.parts)){
	  printf("[ZCAS_SUMMARY]释放资源失败!\n");
	}
	free(doc.text);
	if(doc.title != NULL)
	  free(doc.title);
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_release_parts end.\n");

	DEBUG_PRINT("[ZCAS_SUMMARY]exit zcas_getsummary.\n");
	return outstr;
}

/*功能：获取摘要,参数传入标题
**参数：config-zcas句柄，instr-输入字符串
**返回值：摘要结果
**注意：获取结果成功后，需要释放内存
*/
char *zcas_getsummaryEx(zcas_config *config,char* instr,char *title)
{
	zcas_doc doc;
	char *outstr = NULL;
	double threld = 0;
	unsigned int len = 0;

	DEBUG_PRINT("[ZCAS_SUMMARY]enter zcas_getsummary.\n");
	if(NULL == config || NULL == instr){
	  printf("[ZCAS_SUMMARY]input para err!(0x%x,0x%x)",config,instr);
	  DEBUG_PRINT("[ZCAS_SUMMARY]exit zcas_getsummary.\n");
	  return NULL;
	}

	/* 获取doc结构 */
	memset(&doc,0,sizeof(zcas_doc));
	doc.text = (char*)malloc(strlen(instr)+1);
	memset(doc.text,'\0',strlen(instr)+1);
	strcpy(doc.text,instr);
	
	if(title != NULL){
	  len = strlen(title)+1;
	  doc.title = (char*)malloc(len);
	  strncpy(doc.title,title,len-1);
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]get doc end.\n");

	/* 分part */
	if(ZCAS_OK != zcas_getPart(config,&doc)){
	  printf("分part失败！\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getPart end.\n");

	/* 分para */
	if(ZCAS_OK != zcas_getParas(config,doc.parts)){
	  printf("分段失败！\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getPara end.\n");

	/* 分句 */
    if(ZCAS_OK != zcas_getSentences(config,doc.parts)){
      printf("分句失败!\n");
    }
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getSentences end.\n");

	/* 分词 */
	if(ZCAS_OK != zcas_getWords(config,doc.parts)){
	  printf("分词失败！\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getWords end.\n");

	/* 获取关联词，权重加入关联度影响 */
	if(config->conjword){
	  if(ZCAS_OK != zcas_getConjWords(config,doc.parts)){
	    printf("获取关联词失败！\n");
	  }
    DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getConjWords end.\n");
	}

    /* 统计词频，初始化句子权重，获取主轴 */
	if(ZCAS_OK != zcas_getPartKeywords(config,doc.parts,doc.title)){
	  printf("获取part关键词失败！\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getPartKeywords end.\n");

	/* 根据词频调整权重 */
	if(config->freqword){
	  if(ZCAS_OK != zcas_adjustweightbyfreq(config,doc.parts)){
	    printf("[ZCAS_SUMMARY]adjustweightbyfreq error.\n");
	  }
	  DEBUG_PRINT("[ZCAS_SUMMARY]zcas_adjustweightbyfreq end.\n");
	}

	/* 计算相似度 */
	if(ZCAS_OK != zcas_getSimilarity(config,doc.parts)){
	  printf("[ZCAS_SUMMARY]计算相似度失败!\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getSimilarity end.\n");

	/* 归一化处理 */
	if(ZCAS_OK != zcas_normalize_Weight(doc.parts)){ 
	  printf("[ZCAS_SUMMARY]权重归一化失败!\n"); 
	} 
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_normalize_Weight end.\n");

	/* 获取输出阈 */
	if(ZCAS_OK != zcas_getThreshold(config,doc.parts)){
	  printf("[ZCAS_SUMMARY]get threshold failed!\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getThreshold end.\n");
	
	/* 输出结果 */
	outstr = zcas_outputsummary(doc.parts);/* config配置参数 */
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_outputsummary end.\n");
	if(NULL == outstr){
	  /* 返回wordcount */
	  DEBUG_PRINT("[ZCAS_SUMMARY]摘要失败，将输出一个字数少于%d的句子。\n",config->wordcount);
	  outstr = zcas_outputsummarybywordcount(doc.parts,config->wordcount);
	  if (NULL == outstr){
	    printf("[ZCAS_SUMMARY]获取摘要结果失败!\n");
	  }
	  DEBUG_PRINT("[ZCAS_SUMMARY]zcas_outputsummarybywordcount end.\n");
	}

	/* 过滤无效字符 */
	if(ZCAS_OK != zcas_removenullchars(config,outstr)){
	  printf("[ZCAS_SUMMARY]过滤无效字符失败!\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_removenullchars end.\n");
	
	/* 释放内存 */
	if(ZCAS_OK != zcas_release_parts(doc.parts)){
	  printf("[ZCAS_SUMMARY]释放资源失败!\n");
	}
	free(doc.text);
	if(doc.title != NULL)
	  free(doc.title);
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_release_parts end.\n");

	DEBUG_PRINT("[ZCAS_SUMMARY]exit zcas_getsummary.\n");
	return outstr;
}


/*功能:获取分词
 *参数：
 *返回值：分词结果字符串
 */
char *zcas_getfc(zcas_config *config,char* instr)
{
  char *outstr = NULL;
  int zcwn = 0,i=0;
  zcfc_oword *words = NULL;
  long size = 0;

  DEBUG_PRINT("[ZCAS_SUMMARY]enter zcas_getfc\n");
  if(NULL == config || NULL == instr){
    printf("[ZCAS_SUMMARY]input para err!(0x%x,0x%x)",config,instr);
    DEBUG_PRINT("[ZCAS_SUMMARY]exit zcas_getfc.\n");
    return NULL;
  }

  outstr = (char*)malloc(ZCAS_TEXT_MAX_LENTH);
  memset(outstr,'\0',(ZCAS_TEXT_MAX_LENTH));
  DEBUG_PRINT("instr:%s\n",instr);
  words = zcfc_run(config->zcfchandle,instr,(int)strlen(instr),&zcwn);/* 结构转换 */
  for (i=0;i<zcwn;i++){
    char str[124];
    if(size >= ZCAS_TEXT_MAX_LENTH)
      break;
    memset(str,'\0',sizeof(str));
    sprintf(str,"%s/%s(0,0)|",words[i].word,words[i].pos);
    strcat(outstr,str);
    size += strlen(str);
  }
  zcfc_release_words(words);
  DEBUG_PRINT("[ZCAS_SUMMARY]exit zcas_getfc\n");
  
  if(NULL != outstr && 0 == strlen(outstr)){
    printf("[ZCAS_SUMMARY]分词结果为空,请检查您的输入。\n");
  }
  return outstr;
}

/*功能：释放zcas内存
**参数：config-zcas句柄
**返回值：ZCAS_Ok-成功，其他-失败
*/
int zcas_releasesummary(zcas_config *config)
{
  DEBUG_PRINT("[ZCAS_RELEASE]enter zcas_releasesummary\n");

  if(NULL == config){
    printf("pconfig is null.\n");
    DEBUG_PRINT("[ZCAS_RELEASE]exit zcas_releasesummary\n");
    return -1;
  }
  zcfc_env_release(config->zcfchandle);

  zcas_UTHASH_CONJWORDS_release(config->partconjwords);
  zcas_UTHASH_CONJWORDS_release(config->conjwords);
  zcas_UTHASH_DOUBLE_release(config->freqwords);
  zcas_UTHASH_DOUBLE_release(config->weightwords);

  free(config);

  DEBUG_PRINT("[ZCAS_RELEASE]exit zcas_releasesummary\n");
  return ZCAS_OK;
}

#if 1
//获取主轴
zcas_part *zcas_getPartKeywordsAB(zcas_config *config,char* instr)
{
	zcas_doc doc;
	char *outstr = NULL;
	char *tmp = NULL;
	unsigned int len = 0;/* windows error */
	double threld = 0;

	DEBUG_PRINT("[ZCAS_SUMMARY]enter zcas_getsummary.\n");
	if(NULL == config || NULL == instr){
	  printf("[ZCAS_SUMMARY]input para err!(0x%x,0x%x)",config,instr);
	  DEBUG_PRINT("[ZCAS_SUMMARY]exit zcas_getsummary.\n");
	  return NULL;
	}

	/* 过滤无效字符 */
	if(ZCAS_OK != zcas_removenullchars(config,instr)){
	  printf("[ZCAS_SUMMARY]过滤无效字符失败!\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_removenullchars end.\n");

	/* 获取doc结构 */
	memset(&doc,0,sizeof(zcas_doc));
	doc.text = (char*)malloc(strlen(instr)+1);
	memset(doc.text,'\0',strlen(instr)+1);
	strcpy(doc.text,instr);
	
	tmp = strstr(instr,"\n");
	len = tmp - instr;
	if(NULL != tmp && len <= title_Max_Len){
	  doc.title = (char *)malloc(len+1);
	  memset(doc.title,'\0',len+1);
	  strncpy(doc.title,instr,len);
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]get doc end.\n");

	/* 分part */
	if(ZCAS_OK != zcas_getPart(config,&doc)){
	  printf("分part失败！\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getPart end.\n");

	/* 分para */
	if(ZCAS_OK != zcas_getParas(config,doc.parts)){
	  printf("分段失败！\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getPara end.\n");

	/* 分句 */
    if(ZCAS_OK != zcas_getSentences(config,doc.parts)){
      printf("分句失败!\n");
    }
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getSentences end.\n");

	/* 分词 */
	if(ZCAS_OK != zcas_getWords(config,doc.parts)){
	  printf("分词失败！\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getWords end.\n");

	/* 获取关联词，权重加入关联度影响 */
	if(config->conjword){
	  if(ZCAS_OK != zcas_getConjWords(config,doc.parts)){
	    printf("获取关联词失败！\n");
	  }
    DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getConjWords end.\n");
	}

    /* 统计词频，初始化句子权重，获取主轴 */
	if(ZCAS_OK != zcas_getPartKeywords(config,doc.parts,doc.title)){
	  printf("获取part关键词失败！\n");
	}
	DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getPartKeywords end.\n");

	/* 根据词频调整权重 */
	if(config->freqword){
	  if(ZCAS_OK != zcas_adjustweightbyfreq(config,doc.parts)){
	    printf("[ZCAS_SUMMARY]adjustweightbyfreq error.\n");
	  }
	  DEBUG_PRINT("[ZCAS_SUMMARY]zcas_adjustweightbyfreq end.\n");
	}

	return doc.parts;
}

double zcas_get2PartsSimilarityAB(zcas_part *part1,zcas_part *part2)
{
  double weight = 0;

  weight = fabs(zcas_getCosineAB(part1->hashwords,part2->hashwords));

  return weight;
}

double zcas_getCosineAB(UTHASH_DOUBLE *hashwords1,UTHASH_DOUBLE *hashwords2)
{
	double mult=0;
	double ret,mold1,mold2;
	
	int m1 = HASH_COUNT(hashwords1);
	int m2 = HASH_COUNT(hashwords2);
	int n = 0;
	//printf("enter zcas_getCosineAB.\n");
	if (NULL == hashwords1 || NULL == hashwords2){
	  printf("input para is null!(0x%x,0x%x)",hashwords1,hashwords2);
	  return 0;
	}

	//
	UTHASH_DOUBLE *tmp = NULL;
	for (tmp=hashwords2;tmp!=NULL;tmp=(UTHASH_DOUBLE*)tmp->hh.next){
	  double value = 0;
	  if(zcas_UTHASH_DOUBLE_find(hashwords1,tmp->key,&value)){
	    mult = mult+tmp->value*value;
	  }
	  n++;
	}
	//

    ret=0;
    mold1=zcas_getMoldEx(hashwords1);
    mold2=zcas_getMoldEx(hashwords2);
    if(mold1 * mold2==0)
      ret = 0;
    else{
      ret = mult/(mold1*mold2);
#if 0
      /* 对于过短的句子，加大它与主轴的夹角 */
      if(m < 200){
	    double t = (double)n/m;
	    t = pow(t,0.3);
	    ret = ret*t;
      }
#endif
    }
    ret= fabs(ret);

    //printf("exit zcas_getCosnieAB.\n");

	return ret;
}

#endif
