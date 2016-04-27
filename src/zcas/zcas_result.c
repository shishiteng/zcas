#include "zcas_result.h"
#include "zcas_sentence.h"
#include "zcas_common.h"
#include "zcas_vector.h"

/*功能:输出结果,不需要换行符和TAB
**参数：parts-所有part
**返回值：结果字符串
*/
char* zcas_outputsummary(zcas_part* parts)
{
	zcas_part *part=NULL;
	zcas_parag *para=NULL;
	zcas_sentence *sent=NULL;
	zcas_word *wd=NULL;
	char stmp[1024] = {'\0'};
	char *outstr = NULL;

	DEBUG_PRINT("[ZCAS_RESULT]enter zcas_outputsummary\n");
	if(NULL == parts){
	  DEBUG_PRINT("zcas_outputsummary:input err(0x%x)\n",parts);
	  return NULL;
	}

	outstr = (char*)malloc(ZCAS_TEXT_MAX_LENTH);
	memset(outstr,'\0',ZCAS_TEXT_MAX_LENTH);

	/* 存在重复的句子时，输出有问题 */
	for (part=parts; part!=NULL; part=part->next){
	  DEBUG_PRINT("threshold=%lf\n",part->threshold);
	  if(strlen(part->title) > 0){
	    DEBUG_PRINT("--============================%d,%s\n",strlen(part->title),part->title);
	    sprintf(stmp,"%s%s",part->title,LINE_SEP);
	    //strcat(outstr,stmp);
	  }
	  memset(stmp,'\0',sizeof(stmp));
	  for (para=part->parags; para!=NULL; para=para->next){
	    int isFirst=1;
	    for (sent=para->sents; sent!=NULL; sent=sent->next){
	      if(sent->weight < part->threshold)
		continue;
	      if(1 == sent->isfilt)
		continue;
	      strcat(outstr,sent->text);
	      isFirst=0;
	    }
	  }
	  if(part->next != NULL){
	    strcat(outstr,LINE_SEP);
	  }
	}

	if(0 == strlen(outstr)){
	  free(outstr);
	  outstr = NULL;
	}
	DEBUG_PRINT("[ZCAS_RESULT]exit zcas_outputsummary\n");
	return outstr;
}

/*功能:输出结果,不需要换行符和TAB
**参数：parts-所有part,wordcount-词语数
**返回值：结果字符串
*/
char* zcas_outputsummarybywordcount(zcas_part* parts,int wordcount)
{
	zcas_part *part=NULL;
	zcas_parag *para=NULL;
	zcas_sentence *sent=NULL;
	char stmp[1024] = {'\0'};
	int count = 0;
	char *outstr = NULL;

	DEBUG_PRINT("[ZCAS_RESULT]enter zcas_outputsummarybywordcount\n");
	if(NULL == parts || wordcount <= 0){
	  printf("[ZCAS_RESULT]zcas_outputsummarybywordcount:input paras err(0x%x,%d)\n.",parts,wordcount);
	  return NULL;
	}

	outstr = (char*)malloc(ZCAS_TEXT_MAX_LENTH);
	memset(outstr,'\0',ZCAS_TEXT_MAX_LENTH);

	for (part=parts; part!=NULL; part=part->next){
	  memset(stmp,'\0',sizeof(stmp));
	  for (para=part->parags; para!=NULL; para=para->next){
	    for (sent=para->sents; sent!=NULL; sent=sent->next){
	      int n = zcas_getwordcount(sent->text);
	      if(n > wordcount)
		continue;
	      count += n;
	      if(count > wordcount)
		break;
	      strcat(outstr,sent->text);
	    }
	  }
	}

	/* 分段、分句都失败的情况下,输出wordcount个汉字,单个part */
	if(0 == strlen(outstr)){
	  for (part=parts; part!=NULL; part=part->next){
	    if(NULL != part->text) {
	      char *str = part->text;
	      int count = wordcount;
	      int n = zcas_getwordcount(part->text);
	      if(n <= count){
	        strcat(outstr,part->text);
	        break;
	      }
	      while(count-- > 0){
	        int m = zcas_getbytesnum(str[0]);
	        strncat(outstr,str,m);
	        str += m;
	      }
	    }	  
	  }
	}

	DEBUG_PRINT("real wordcount=%d\n",zcas_getwordcount(outstr));
	if(0 == strlen(outstr)){
	  free(outstr);
	  outstr = NULL;
	}
	DEBUG_PRINT("[ZCAS_RESULT]exit zcas_outputsummarybywordcount\n");
	return outstr;
}

/*功能：设置part输出阈
**参数：
**返回值：0-成功
*/
int zcas_getThreshold(zcas_config* config,zcas_part *parts)
{
  zcas_part *part = NULL;
  for(part=parts;part!=NULL;part=part->next){
    zcas_getPartThreshold(config,part);
  }
  return ZCAS_OK;
}

/*功能：设置单个part的输出阈
**参数：config-zcas句柄，part-单个part
**返回值：0-成功，其他-失败
*/
int zcas_getPartThreshold(zcas_config *config,zcas_part* part)
{
  double len = 0;
  double sum = 0;
  int count,ratio;
  int sentcount = 0;
  int realcount = 0;
  zcas_sentence *allsent = NULL;/* 效率，中间结构 */
  zcas_parag *para = NULL;
  zcas_sentence *sent = NULL;
  zcas_sentence *tmp = NULL;

  DEBUG_PRINT("[ZCAS_RESULT]enter zcas_getPartThreshold.\n");
  if(NULL == config || NULL == part){
    DEBUG_PRINT("zcas_getPartThreshold:input err(0x%x,0x%x).\n",config,part);
    return -1;
  }

  /* 获取全部句子 */
  DEBUG_PRINT("get all sentences.\n");
  for(para=part->parags;para!=NULL;para=para->next){
    for(sent=para->sents;sent!=NULL;sent=sent->next){
      zcas_sentence sentn;
      memcpy(&sentn,sent,sizeof(zcas_sentence));
      sentn.lenth = zcas_getwordcount(sentn.text);
      sentn.words = NULL;
      len += sentn.lenth;
      zcas_Sentlist_addtail(&allsent,sentn);
    }
  }

  /* 根据weight对句子进行排序(降序) */
  DEBUG_PRINT("sort all sentences by weight start.\n");
  for(sent=allsent;sent!=NULL;sent=sent->next){
    for(tmp=sent->next;tmp!=NULL;tmp=tmp->next){
      if(sent->weight < tmp->weight) {
	    zcas_sentence sentence = *sent;
	    sent->text = tmp->text;
	    sent->words = tmp->words;
	    sent->mold = tmp->mold;
	    sent->weight = tmp->weight;
	    sent->lenth = tmp->lenth;
	    sent->isfilt = tmp->isfilt;

	    tmp->text = sentence.text;
	    tmp->words = sentence.words;
	    tmp->mold = sentence.mold;
	    tmp->weight = sentence.weight;
	    tmp->lenth = sentence.lenth;
	    tmp->isfilt = sentence.isfilt;
      }
    }
    sentcount++;
  }
  DEBUG_PRINT("sort all sentences by weight over,sentcount=%d.\n",sentcount);
  
  /* mode 0:按百分比输出，1：按句子数输出 */
  count = 0;
  ratio = 1;/* 输出比例调整系数  */
  if(config->threldmode){
    /* 根据最终输出句子数获取权重分割阈 */
    if(config->outcount < 5 && config->bestout){
      ratio = 2;
    }

    if(config->outcount*ratio > sentcount)
      realcount = sentcount;
    else
      realcount = config->outcount *ratio;
    for(sent=allsent;sent!=NULL;sent=sent->next){
      ++count;
      if(count >= realcount){
	    part->threshold = sent->weight;
	    break;
      }
    }
  }
  else{
    /* 根据文章分割比例获取权重分割阈 */
    for(sent=allsent;sent!=NULL;sent=sent->next){
	  double val;
      ++count;
      sum += sent->lenth;
      val = sum/len;
      /* 输出两倍的句子 *2 */
      if(config->outpercent < val){
	    part->threshold = sent->weight;
	    break;
      }
    }
  }
  
  /* 阈获取失败时，返回 */
  DEBUG_PRINT("part threld:%lf,real count:%d,ratio:%d\n",part->threshold,count,ratio);
  if(0 == part->threshold){
    printf("[ZCAS_SUMMARY]get threshold failed,the threshold is 0.\n");
    zcas_Sentlist_clear(allsent);
    return 1;
  }
  
  /* 调整输出结果，去重复句 */
  if(ZCAS_OK != zcas_filtrepeatsent(part,allsent,count)){
    DEBUG_PRINT("filtrepeatsent failed.\n");
    zcas_Sentlist_clear(allsent);
    return -1;
  }
  DEBUG_PRINT("[ZCAS_SUMMARY]filt repeat sentence end.\n");

  if(0 == part->threshold){
    DEBUG_PRINT("get threshold exception:part threld=0.\n");
    zcas_Sentlist_clear(allsent);
    return -1;
  }

  if (gprintlevel == 1){
    for(sent=allsent;sent!=NULL;sent=sent->next){
      DEBUG_PRINT("[sort]%s%lf\n",sent->text,sent->weight);
    }
  }
  
  if(config->bestout && 1 == config->threldmode){
    //去掉重复句后，重新获取输出的句子数
    count = zcas_getoutcount(part,NULL); 

    //在输出的句子中寻找与主轴最接近的组合 */
    DEBUG_PRINT("在%d句话中挑选出最佳%d句,threld=%lf：\n",count,config->outcount,part->threshold);
    if(ZCAS_OK != zcas_getbestoutput(config,part,count,config->outcount)){
      DEBUG_PRINT("[ZCAS_SUMMARY]zcas_getbestoutput failed.\n");
    }
    DEBUG_PRINT("[ZCAS_SUMMARY]get best output end.\n");
  }

  /* 根据字数调整输出结果 */
  zcas_adjustresultbywordcount(part,count,config->wordcount);
  DEBUG_PRINT("[ZCAS_SUMMARY]adjust result by word count end.\n");

  /* 释放资源 */
  zcas_Sentlist_clear(allsent);

  DEBUG_PRINT("[ZCAS_RESULT]exit zcas_getPartThreshold\n");
  return ZCAS_OK;
}

/* 
功能：调整输出的句子,去掉结果中的重复句
参数：part：
     allsent：根据句子权重降序排序后的句子链表；
     threld：输出阈
     count：应该输出的句子数；
返回值：0-ok,其他-失败
*/
int zcas_filtrepeatsent(zcas_part *part,zcas_sentence *allsent,int count)
{
  /* i:根据权重获得输出句子数；n:过滤后的句子数(去重复)，count：理应输出的句子数, */
  int errflag = 0;
  int i,n;
  zcas_sentence *out[2000];
  zcas_sentence *sent = NULL;

  if(NULL == part || NULL == allsent){
    DEBUG_PRINT("zcas_filtrepeatsent:input err(0x%x,0x%x).\n",part,allsent);
    return -1;
  }
  DEBUG_PRINT("enter zcas_filtrepeatsent:threld=%lf,count=%d\n",part->threshold,count);
  memset(out,0,sizeof(out));

  i = zcas_getoutcount(part,out);/* 获得输出句子数 */
  n = zcas_getrealsentcount(out,i);/* 获得不重复的句子数 */
  DEBUG_PRINT("zcas_filtrepeatsent:threld=%lf,outsent=%d,realsent=%d\n",part->threshold,i,n);
  while(n != i){  
    int m = count - n;/* 不重复句子少于目标句子数，补齐句子 */
    while(m-- > 0){
      for(sent=allsent;sent!=NULL;sent=sent->next){
	    if(sent->weight < part->threshold){
	      part->threshold = sent->weight;
	      break;
	    }
	    errflag ++;
	    if(errflag > 10000){
	      printf("adjustoutput error!\n");
	      break;
	    }
      }
    }
    errflag = 0;
    memset(out,0,sizeof(out));
    i = zcas_getoutcount(part,out);
    n = zcas_getrealsentcount(out,i);
    DEBUG_PRINT("zcas_filtrepeatsent:threld=%lf,outsent=%d,realsent=%d\n",part->threshold,i,n);
    /* printf("[]%lf,%d,%d\n",threld,i,n); */
    errflag++;
    if(errflag >10000){
      printf("adjustthreld error!\n");
      break;
    }
  }

  DEBUG_PRINT("exit zcas_filtrepeatsent.\n");
  return ZCAS_OK;
}

/* 
功能：过滤相似的句子
参数：sent[]:存放句子指针的指针数组
     count：应该输出的句子数
返回值：sent[]中不重复的句子数
 */
int zcas_getrealsentcount(zcas_sentence *sent[],int count)
{
  int m = 0;
  int i,j;
  double similarity;

  DEBUG_PRINT("enter zcas_getrealsentcount:input count=%d\n.",count);
  for(i=0;i<count;i++){
    if(sent[i]->isfilt) continue;
    for(j=i+1;j<count;j++){
      similarity = zcas_getWordsSimilarity(sent[i],sent[j]);
      if(similarity > 0.9){
	    DEBUG_PRINT("similarity=%lf\n",similarity);
	    DEBUG_PRINT("repeat sent[%d]:%s,%lf\n",i,sent[i]->text,sent[i]->weight);
	    DEBUG_PRINT("repeat sent[%d]:%s,%lf\n",j,sent[j]->text,sent[j]->weight);
	    if(sent[i]->weight >= sent[j]->weight){
	      sent[j]->isfilt = 1;
	    }
	    else
	      sent[i]->isfilt = 1;
	    continue;
      }
    }
    m++;
  }

  DEBUG_PRINT("exit zcas_getrealsentcount:realcount=%d\n.",m);
  return m;
}
/* 
功能：计算两个句子的相似度
参数：sent1，sent2
返回值：相似度
 */
double zcas_getWordsSimilarity(zcas_sentence *sent1,zcas_sentence *sent2)
{
  double n = 0;
  zcas_word *word = NULL;
  if(NULL == sent1 || NULL == sent2){
    return 0;
  }

  n = fabs(zcas_getCosineEx(sent1->words,sent2->words));
  return n;
}

/* 
功能:获取符合阈值条件的输出句子数
参数：parts：文章part结构
     out[]：输出句子的指针数组
返回值：所有权重大于等于threld，且未被过滤的句子数
 */
int zcas_getoutcount(zcas_part *part,zcas_sentence *out[])
{
  int i = 0;
  zcas_parag *para = NULL;
  zcas_sentence *sent = NULL;

  DEBUG_PRINT("enter zcas_getoutcount:threld=%lf\n.",part->threshold);
  for(para=part->parags;para!=NULL;para=para->next){
    for(sent=para->sents;sent!=NULL;sent=sent->next){
      if(sent->weight < part->threshold) continue;
      if(sent->isfilt) continue;
      if(NULL != out)
        out[i] = sent;
	//printf("0x%x,0x%x,0x%x,%s\n",out,out[i],sent,sent->text); 
      i++;
    }
  }
  DEBUG_PRINT("exit zcas_getoutcount:count=%d\n.",i);
  return i;
}

/* 
**功能：挑选与主轴最接近的句子组合
**参数：config-zcas句柄
**     part：文章part结构
**     count：根据输出阈得到的句子数
**     outcount：输出结果的句子数
**返回值：0
*/
int zcas_getbestoutput(zcas_config *config,zcas_part *part,int count,int outcount)
{
  int i,j,k;
  double cos = 0;
  int cmb = 0;
  double maxcos = 0;
  UTHASH_DOUBLE *keywords=NULL;
  zcas_sentence **out=NULL;
  zcas_parag *para=NULL;
  zcas_sentence *sent=NULL;
  zcas_sentcombine *insent = NULL;
  zcas_sentcombine *sentcmb = NULL;
  zcas_sentcombine **outsent = NULL;
  inputstruct input;
  zcas_sentcombine **result = NULL;

  DEBUG_PRINT("enter zcas_getbestoutput:%lf,%d,%d\n",part->threshold,count,outcount);  
  if(NULL == config || NULL == part){
    DEBUG_PRINT("zcas_getbestoutput:input err(0x%x,0x%x).\n",config,part);
    DEBUG_PRINT("exit zcas_getbestoutput.\n");
    return -1;
  }
  if(count == outcount || count <= 1){
    DEBUG_PRINT("exit zcas_getbestoutput.\n");
    return ZCAS_OK;
  }
  
  out = (zcas_sentence **)malloc(sizeof(zcas_sentence*)*count);
  memset(out,0,sizeof(zcas_sentence*)*count);

  /* 1.获取输出句子数组 */
  i = 0;
  for(para=part->parags;para!=NULL;para=para->next){
    for(sent=para->sents;sent!=NULL;sent=sent->next){
      if(sent->weight < part->threshold) continue;
      if(sent->isfilt) continue;
      out[i++] = sent;
      //printf("11     out[%d]:%s\n",i,out[i]->text);
      if(i>=count) break;
    }
    if(part->hashwords != NULL){
      keywords = part->hashwords;
    }
  }
  
  /* 以实际的输出句子数为准 */
  if(0 == i){
    DEBUG_PRINT("zcas_getbestoutput:获取输出句子数失败，应输出%d，实际输出%d.\n",outcount,i);
    free(out);
    return 1;
  }
  else if(i <= outcount){
    DEBUG_PRINT("zcas_getbestoutput:获取输出句子数异常，应输出%d，实际输出%d.\n",outcount,i);
    free(out);
    return 2;
  }
  else if(i < count){
    DEBUG_PRINT("zcas_getbestoutput:获取输出句子数需调整，原输出%d，现输出%d.\n",outcount,i);
    count = i;
  }

  insent = (zcas_sentcombine *)malloc(sizeof(zcas_sentcombine)*count);
  memset(insent,0,sizeof(zcas_sentcombine)*count);
  for(i=0;i<count;i++){
    insent[i].sent = out[i];
  }

  sentcmb = (zcas_sentcombine *)malloc(sizeof(zcas_sentcombine)*outcount);
  memset(sentcmb,0,sizeof(zcas_sentcombine)*outcount);
  
  cmb = zcas_getcombineval(count,outcount);/* 计算组合数，定义输出结构 */
  if(cmb <= 0){
    free(out);
    free(insent);
    free(sentcmb);
    return -1;
  }

  outsent = (zcas_sentcombine **)malloc(sizeof(zcas_sentcombine *)*cmb);
  memset(outsent,0,sizeof(zcas_sentcombine *)*cmb);
  for(i=0;i<cmb;i++){
	outsent[i]=(zcas_sentcombine *)malloc(sizeof(zcas_sentcombine)*outcount);
	memset(outsent[i],0,sizeof(zcas_sentcombine)*outcount);
  }
  DEBUG_PRINT("zcasgetbestoutput:C[%d][%d]=%d\n",count,outcount,cmb);

  /* 2.排列组合句子 */
  input.len = count;
  input.count = outcount;
  input.m = 0;
  zcas_combinesents(outcount,&input,insent,sentcmb,outsent);

  /* 3.计算各个组合与主矢量夹角，提取最接近的组合 */
  maxcos = 0;
  for(i=0;i<cmb;i++){
	zcas_word *word = NULL;
    for(j=0;j<outcount;j++){
	  zcas_word *words = NULL;
      if(NULL == outsent[i][j].sent) continue;
      DEBUG_PRINT("outsent[%d][%d]:%s\n",i,j,outsent[i][j].sent->text); 
      outsent[i][j].sent->isfilt = 1;
      /* 矢量相加 */
      for(words=outsent[i][j].sent->words ;words!=NULL;words=words->next){
	    int find;
	    zcas_word *tmp;
	    if(NULL == word){
	      zcas_Wordlist_addtail(&word,*words);
	    }
	    else{
	      find = 0;
	      for(tmp=word;tmp!=NULL;tmp=tmp->next){	  
		if(0 == strcmp(words->text,tmp->text)){
		  tmp->weight += words->weight;
		  find =1;
		  break;
		}
	      }
	      if(0 == find)
		zcas_Wordlist_addtail(&word,*words);
	    }
      }   
    }
    if(word != NULL){
      /* 计算夹角 */
      cos = fabs(zcas_getCosine(keywords,word));
      cos = sqrt(cos);
      /* printf("cos=%lf\n",cos); */
      /* 保留最大夹角与矢量 */
      if(maxcos < cos){
	    maxcos = cos;
	    result = &outsent[i];
      }
      zcas_Wordlist_clear(word);
    }
  }

  /* 4.保留与主矢量最接近的组合，将其作为输出 */
  DEBUG_PRINT("zcas_getbestoutput:maxcos=%lf,outcount=%d,presult=0x%x\n",maxcos,outcount,result);
  for(k=0;k<outcount;k++){
    if(NULL == result) break;
    (*result+k)->sent->isfilt = 0;
  }

  /* 释放资源 */
  free(out);
  free(insent);
  free(sentcmb);
  for(i=0;i<cmb;i++){
    free(outsent[i]); 
  }
  free(outsent);

  DEBUG_PRINT("exit zcas_getbestoutput\n");
  return ZCAS_OK;
}

/* 这个函数写的真是狗屎一般
功能：在len个元素中，每n个元素作为一个组合，得到所有的组合。
     insent为输入句子，outsent存放输出结果，sent为临时存放组合
参数说明：n:每个组合的元素个数
        len：可选择的元素个数
        count:数组的长度
        insent：输入的组合元素集
        sent:排列组合临时存放结构
        outsent:输出的组合元素集
 */
void zcas_combinesents(int n,inputstruct *input,zcas_sentcombine insent[],zcas_sentcombine sent[],zcas_sentcombine **outsent)
{
  int i,j,k,tmp;
  if(NULL == input){
    DEBUG_PRINT("zcas_combinesents:input err(0x%x).\n",input);
    return;
  }

  tmp = zcas_getcombineval(input->len,input->count);
  DEBUG_PRINT("enter zcas_combinesents,n=%d,len=%d,count=%d,m=%d\n",n,input->len,input->count,input->m);
  if(input->m >= tmp){
    printf("...........................\n");
    input->m = 0;
  }

  n--;
  for(i=0;i<input->len;i++){
    if(insent[i].exist) continue;
    insent[i].exist = 1;
    sent[n].sent = insent[i].sent;
    if(0 == n){
      for(j=0;j<input->count;j++){
	    outsent[input->m][j] = sent[j];
	    //printf("outsent[%d][%d]:%s\n",input->m,j,sent[j].sent->text);
      }
      input->m++;
      break;
    }
    if(n > 0){
      zcas_combinesents(n,input,insent,sent,outsent);
    }
    for(k=i+1;k<input->len;k++){
      insent[k].exist=0;
    }
  }

  DEBUG_PRINT("exit zcas_combinesents\n");
  return ;
}

/* 
功能：根据输出字数要求调整结果
参数：part:输入part结构
     sentcount：调整前输出的句子数；
     wordcount：要求的字数；
返回值：ZCAS_OK
 */
int zcas_adjustresultbywordcount(zcas_part *part,int sentcount,int wordcount)
{
  zcas_sentence **outsent = NULL;
  int i,j,n = 0;
  int sum = 0,real_count = 0;
  zcas_parag *para = NULL;
  zcas_sentence *sent = NULL;

  if(NULL == part){
    DEBUG_PRINT("zcas_adjustresultbywordcount:input err(0x%x).\n",part);
    return -1;
  }

  DEBUG_PRINT("enter zcas_adjustresultbywordcount[%lf,%d,%d]\n",part->threshold,sentcount,wordcount);

  if(sentcount <= 0){
    DEBUG_PRINT("exit zcas_adjustresultbywordcount:sentcount=0.\n");
    return -1;
  }

  outsent = (zcas_sentence **)malloc(sizeof(zcas_sentence*)*sentcount);
  memset(outsent,0,sizeof(outsent));
  /* 获取全部输出句子 */
  for(para=part->parags;para!=NULL;para=para->next){
    for(sent=para->sents;sent!=NULL;sent=sent->next){
      if(1 == sent->isfilt || sent->weight < part->threshold)  continue;
      if(n >= sentcount) break;
      outsent[n++] = sent;
      DEBUG_PRINT("%s,%lf\n",sent->text,sent->weight);
    }
  }

  DEBUG_PRINT("not filted sent count:%d\n",n);
  if(n > sentcount){
    free(outsent);
    DEBUG_PRINT("exit zcas_adjustresultbywordcount:n>sentcount,n=%d,sentcount=%d\n",n,sentcount);
    return -1;
  }

  /* 按权重排序,isfilt置1 */
  for(i=0;i<n;i++){
    for(j=i+1;j<n;j++){
	  zcas_sentence *tmp = NULL;
      if(outsent[i]->weight < outsent[j]->weight){
	    tmp = outsent[i];
	    outsent[i] = outsent[j];
	    outsent[j] = tmp;
      }
    }
    /* printf("[]sent=%s,%d\n",outsent[i]->text,outsent[i]->isfilt); */
   outsent[i]->isfilt = 1;
  }

  //统计每个句子字数，字数超标
  for(i=0;i<n;i++){
    int m;
	m = zcas_getwordcount(outsent[i]->text);
    if(m > wordcount)
      continue;
    /* printf("sent=%s,%d,%d\n",outsent[i]->text,m,outsent[i]->isfilt); */
    sum += m;
    if(sum <= wordcount)
      outsent[i]->isfilt = 0;
    else{
      real_count = sum-m;
      break;
    }
  }

  free(outsent);
  DEBUG_PRINT("real word count:%d\n",real_count);
  DEBUG_PRINT("exit zcas_adjustresultbywordcount.\n");
  return ZCAS_OK;
}


