#include "zcas_init.h"
#include "zcas_common.h"
#include "zctool_license.h"
#include "zcfc.h"

int gprintlevel = 0;


/*功能：zcas初始化
 *参数：confpath-zcas配置文件路径
 *返回值：zcas_config的指针
 */
int zcas_init(char *confpath,zcas_config **pconfig)
{
  int ret = -1;
  char filename[1024] = {'\0'};
  zcas_config *config = NULL;

  DEBUG_PRINT("[ZCAS_INIT]enter zcas_init[%s].\n",confpath);
  if(NULL == confpath || NULL == pconfig){
    printf("[ZCAS_INIT]paras invalid(0x%x,0x%x).\n",confpath,pconfig);
    return MODULE_INIT+1;
  }

#if 0
  /* Copyright：2013-2015 */
  if(zcas_islimited()){
    return MODULE_INIT;
  }
#endif
  
  /* 读取配置文件 */
  config = (zcas_config*)malloc(sizeof(zcas_config));
  memset(config,0,sizeof(zcas_config));
  ret = zcas_getconfig(config,confpath);
  if(ZCAS_OK != ret){
    free(config);
    printf("[ZCAS_INIT]get config failed.\n");
    return MODULE_INIT+10+ret;
  }
  DEBUG_PRINT("[ZCAS_INIT]zcas_getconfig end.\n");

  /* 开启调试开关 */
  if(0 == gprintlevel){
    if(config->debugmode)
	  gprintlevel = 1;
  }
  else{
    config->debugmode = gprintlevel;
  }
  
  /* 验证授权文件 */
  ret = zcas_verifylicense(config->zcas_license_dir);
  if(ZCAS_OK != ret){
    printf("[ZCAS_INIT]verify license failed.\n");
    free(config);
    return MODULE_INIT+20+ret;
  }
  DEBUG_PRINT("[ZCAS_INIT]zcas_verifylicense end.\n");

  /* 加载zcfc词典 */
  config->zcfchandle = zcas_loadzcfcdict(config->zcfc_confdir);/* 40byte */
  if(NULL == config->zcfchandle){
    printf("[ZCAS_INIT]load zcfc dict failed.\n");
    free(config);
    return MODULE_INIT+30;
  }
  DEBUG_PRINT("[ZCAS_INIT]zcas_loadzcfcdict end.\n");
  
  /* 加载zcas词典 */
  strcpy(filename,config->zcas_dicdir);
  strcat(filename,"zcas.dct");
  ret = zcas_loadzcasdict(config,filename);
  if(ZCAS_OK != ret){
    printf("[ZCAS_INIT]load zcas dict failed.\n");
    free(config);
    return MODULE_INIT+40+ret;
  }
  DEBUG_PRINT("[ZCAS_INIT]zcas_loadzcasdict end.\n");

  /* 版本信息 */
  strcpy(config->version,"2.2.2 Copyright (C) 2013-2015,by zctech.");
  DEBUG_PRINT("[ZCAS_INIT]zcas version:%s\n",config->version);

  *pconfig = config;
  if(NULL == *pconfig){
    printf("[ZCAS_INIT]pconfig is null.\n");
    return MODULE_INIT+50;
  } 

  DEBUG_PRINT("[ZCAS_INIT]exit zcas_Init\n");
  return ZCAS_OK;
}

/*功能：查看zcas是否使用受限:即是否在有效期内
**参数:无
**返回值：0-不受限，1-受限
*/
int zcas_islimited()
{
  time_t now;
  struct tm *timenow;
  
  time(&now);
  timenow = localtime(&now);
  if((timenow->tm_year+1900) >= 2016){
    return 1;
  }

  return 0;
}

/*功能：重新加载配置文件
**参数：config-zcas句柄，confpath-配置文件路径
**返回值:0-成功；其他-失败
*/
int zcas_reload(zcas_config *config,char *confpath)
{
  int ret = -1;
  char filename[1024] = {'\0'};

  if(NULL == config || NULL == confpath){
    printf("zcas_reload:input paras invalid,(0x%x,0x%x)\n",config,confpath);
    return -1;
  }
  strcpy(filename,confpath);
  strcat(filename,"zcas.conf");

  ret = zcas_loadconfig(filename,config);
  if(ZCAS_OK != ret){
    printf("zcas_reload failed.\n");
    return ret;
  }
  return ZCAS_OK;
}

/*功能：设置
**参数：config-zcas句柄，opt-设置的选项，val-选项值
**返回值：无
*/
void zcas_setoption(zcas_config *config,char *opt,char *val)
{
  DEBUG_PRINT("enter zcas_setoption[0x%x,%s,%s]\n",config,opt,val);
  if(NULL == config || NULL == opt || NULL == val){
    printf("input para is invalid.\n");
    DEBUG_PRINT("exit zcas_setoption.\n");
    return;
  }
  
  if(0 == strcmp(opt,"threldmode")){
    zcas_setopt_threldmode(config,atoi(val));
  }
  else if(0 == strcmp(opt,"outpercent")){
    zcas_setopt_outpercent(config,atof(val));
  }
  else if(0 == strcmp(opt,"outcount")){
    zcas_setopt_outcount(config,atoi(val));
  }
  else if(0 == strcmp(opt,"wordcount")){
    zcas_setopt_wordcount(config,atoi(val));
  }
  else if(0 == strcmp(opt,"thememode")){
    zcas_setopt_thememode(config,atoi(val));
  }
  else if(0 == strcmp(opt,"partbytes")){
    zcas_setopt_partbytes(config,atoi(val));
  }
  else if(0 == strcmp(opt,"paracount")){
    zcas_setopt_paracount(config,atoi(val));
  }
  else if(0 == strcmp(opt,"topicsent")){
    zcas_setopt_topicsent(config,atoi(val));
  }
  else if(0 == strcmp(opt,"bestout")){
    zcas_setopt_bestout(config,atoi(val));
  }
  else if(0 == strcmp(opt,"conjword")){
    zcas_setopt_conjword(config,atoi(val));
  }
  else if(0 == strcmp(opt,"freqword")){
    zcas_setopt_freqword(config,atoi(val));
  }
  else if(0 == strcmp(opt,"articletype")){
    zcas_setopt_articletype(config,atoi(val));
  }
  else if(0 == strcmp(opt,"debugmode")){
    zcas_setdebugmode(atoi(val));
  }
  else{
    printf("warning:can't find option \"%s\",zcas_setoption failed.\n",opt);
  }

  DEBUG_PRINT("exit zcas_setoption.\n");
}

/*功能：设置
**参数：config-zcas句柄，n-设置的值
**返回值：无
*/
void zcas_setopt_threldmode(zcas_config *config,int n)
{
  DEBUG_PRINT("enter zcas_setopt_threldmode[0x%x,%d]\n",config,n);
  if(NULL == config){
    printf("input err:config is null.\n");
    DEBUG_PRINT("exit zcas_setopt_threldmode.\n");
    return;
  }

  if(0 == n || 1 == n){
    config->threldmode = n;
    DEBUG_PRINT("zcas_setopt_threldmode:success.\n");
  }
  DEBUG_PRINT("exit zcas_setopt_threldmode.\n");
}

/*功能：设置
**参数：config-zcas句柄，n-设置的值
**返回值：无
*/
void zcas_setopt_outpercent(zcas_config *config,double d)
{
  DEBUG_PRINT("enter zcas_setopt_outpercent[0x%x,%lf]\n",config,d);
  if(NULL == config){
    printf("input err:config is null.\n");
    DEBUG_PRINT("exit zcas_setopt_outpercent.\n");
    return;
  }

  if(d > 0 && d < 1){
    config->outpercent = d;
    DEBUG_PRINT("zcas_setopt_outpercent:success.\n");
  }
  DEBUG_PRINT("exit zcas_setopt_outpercent.\n");
}

/*功能：设置
**参数：config-zcas句柄，n-设置的值
**返回值：无
*/
void zcas_setopt_outcount(zcas_config *config,int n)
{
  DEBUG_PRINT("enter zcas_setopt_outcount[0x%x,%d]\n",config,n);
  if(NULL == config){
    printf("input err:config is null.\n");
    DEBUG_PRINT("exit zcas_setopt_outcount.\n");
    return;
  }

  if(n > 0){
    config->outcount = n;
    DEBUG_PRINT("zcas_setopt_outcount:success.\n");
  }
  DEBUG_PRINT("exit zcas_setopt_outcount.\n");
}

/*功能：设置
**参数：config-zcas句柄，n-设置的值
**返回值：无
*/
void zcas_setopt_wordcount(zcas_config *config,int n)
{
  DEBUG_PRINT("enter zcas_setopt_wordcount[0x%x,%d]\n",config,n);
  if(NULL == config){
    printf("input err:config is null.\n");
    DEBUG_PRINT("exit zcas_setopt_wordcount.\n");
    return;
  }

  if(n > 0){
    config->wordcount = n;
    DEBUG_PRINT("zcas_setopt_wordcount:success.\n");
  }
  DEBUG_PRINT("exit zcas_setopt_wordcount.\n");
}

/*功能：设置
**参数：config-zcas句柄，n-设置的值
**返回值：无
*/
void zcas_setopt_thememode(zcas_config *config,int n)
{
  DEBUG_PRINT("enter zcas_setopt_thememode[0x%x,%d]\n",config,n);
  if(NULL == config){
    printf("input err:config is null.\n");
    DEBUG_PRINT("exit zcas_setopt_thememode.\n");
    return;
  }

  if(n >= 0 && n <= 3){
    config->thememode = n;
    DEBUG_PRINT("zcas_setopt_thememode:success.\n");
  }
  DEBUG_PRINT("exit zcas_setopt_thememode.\n");
}

/*功能：设置
**参数：config-zcas句柄，n-设置的值
**返回值：无
*/
void zcas_setopt_partbytes(zcas_config *config,int n)
{
  DEBUG_PRINT("enter zcas_setopt_partbytes[0x%x,%d]\n",config,n);
  if(NULL == config){
    printf("input err:config is null.\n");
    DEBUG_PRINT("exit zcas_setopt_partbytes.\n");
    return;
  }

  if(n > 0){
    config->partbytes = n;
    DEBUG_PRINT("zcas_setopt_partbytes:success.\n");
  }
  DEBUG_PRINT("exit zcas_setopt_partbytes.\n");
}

/*功能：设置
**参数：config-zcas句柄，n-设置的值
**返回值：无
*/
void zcas_setopt_paracount(zcas_config *config,int n)
{
  DEBUG_PRINT("enter zcas_setopt_paracount[0x%x,%d]\n",config,n);
  if(NULL == config){
    printf("input err:config is null.\n");
    DEBUG_PRINT("exit zcas_setopt_paracount.\n");
    return;
  }

  if(n > 0){
    config->paracount = n;
    DEBUG_PRINT("zcas_setopt_paracount:success.\n");
  }
  DEBUG_PRINT("exit zcas_setopt_paracount.\n");
}

/*功能：设置
**参数：config-zcas句柄，n-设置的值
**返回值：无
*/
void zcas_setopt_topicsent(zcas_config *config,int n)
{
  DEBUG_PRINT("enter zcas_setopt_topicsent[0x%x,%d]\n",config,n);
  if(NULL == config){
    printf("input err:config is null.\n");
    DEBUG_PRINT("exit zcas_setopt_topicsent.\n");
    return;
  }

  if(0 == n || 1 == n){
    config->topicsent = n;
    DEBUG_PRINT("zcas_setopt_topicsent:success.\n");
  }
  DEBUG_PRINT("exit zcas_setopt_topicsent.\n");
}

/*功能：设置
**参数：config-zcas句柄，n-设置的值
**返回值：无
*/
void zcas_setopt_bestout(zcas_config *config,int n)
{
  DEBUG_PRINT("enter zcas_setopt_bestout[0x%x,%d]\n",config,n);
  if(NULL == config){
    printf("input err:config is null.\n");
    DEBUG_PRINT("exit zcas_setopt_bestout.\n");
    return;
  }

  if(0 == n || 1 == n){
    config->bestout = n;
    DEBUG_PRINT("zcas_setopt_bestout:success.\n");
  }
  DEBUG_PRINT("exit zcas_setopt_bestout.\n");
}

/*功能：设置
**参数：config-zcas句柄，n-设置的值
**返回值：无
*/
void zcas_setopt_conjword(zcas_config *config,int n)
{
  DEBUG_PRINT("enter zcas_setopt_conjword[0x%x,%d]\n",config,n);
  if(NULL == config){
    printf("input err:config is null.\n");
    DEBUG_PRINT("exit zcas_setopt_conjword.\n");
    return;
  }

  if(0 == n || 1 == n){
    config->conjword = n;
    DEBUG_PRINT("zcas_setopt_conjword:success.\n");
  }
  DEBUG_PRINT("exit zcas_setopt_conjword.\n");
}

/*功能：设置
**参数：config-zcas句柄，n-设置的值
**返回值：无
*/
void zcas_setopt_freqword(zcas_config *config,int n)
{
  DEBUG_PRINT("enter zcas_setopt_freqword[0x%x,%d]\n",config,n);
  if(NULL == config){
    printf("input err:config is null.\n");
    DEBUG_PRINT("exit zcas_setopt_freqword.\n");
    return;
  }

  if(0 == n || 1 == n){
    config->freqword = n;
    DEBUG_PRINT("zcas_setopt_freqword:success.\n");
  }
  DEBUG_PRINT("exit zcas_setopt_freqword.\n");
}

/*功能：设置文章类型
**参数：
**返回值：无
*/
void zcas_setopt_articletype(zcas_config *config,int type)
{
  DEBUG_PRINT("enter zcas_setopt_articletype[0x%x,%d]\n",config,type);
  if(NULL == config){
    printf("input err:config is null.\n");
    DEBUG_PRINT("exit zcas_setopt_articletype.\n");
    return;
  }

  if(type >= TYPE_NONE && type <= TYPE_OTHERS){
    config->articletype = type;
    DEBUG_PRINT("zcas_setopt_articletype:success.\n");
  }
  DEBUG_PRINT("exit zcas_setopt_articletype.\n");
}

/*功能：释放模块内申请的内存
**参数：无
**返回值：无
*/
void zcas_free(void *p)
{
  free(p);
}


/*功能：加载配置文件、关联词词典、标点词典、权重词词典
**参数：config-zcas句柄，confpath-配置文件路径
返回值：ZCAS_OK-成功，其他-失败
*/
int zcas_getconfig(zcas_config *config,char *confpath)
{
  int ret = -1;
  char filename[1024] = {'\0'};/* 修改了路径 */
  DEBUG_PRINT("[ZCAS_INIT]enter getconfig\n");

  if(NULL == config || NULL == confpath){
    printf("zcas_getconfig:input invalid(0x%x,0x%x).\n",config,confpath);
    DEBUG_PRINT("[ZCAS_INIT]exit getconfig\n");
    return -1;
  }
  /* 初始化空字符数组 */
  config->NullChrs[0] = '\t';
  config->NullChrs[1] = ' ';

  /* 加载配置文件 */
  strcpy(filename,confpath);
  strcat(filename,"zcas.conf");
  ret = zcas_loadconfig(filename,config);
  if(ret != ZCAS_OK){
    printf("[ZCAS_INIT]load configfile failed.\n");
    return ret;
  }

  DEBUG_PRINT("[ZCAS_INIT]exit getconfig\n");
  return ZCAS_OK;
}


/*功能：加载配置文件
**参数：filename-zcas配置文件，config-zcas句柄
**返回值：ZCAS_OK-成功，其他-失败
*/
int zcas_loadconfig(char *filename,zcas_config *config)
{
  FILE *fp = NULL;
  char str[256],key[32],value[124];

  if(NULL == filename || NULL == config){
    printf("[ZCAS_INIT]zcas_loadconfig:input invalid(0x%x,0x%x).\n",filename,config);
    return -1;
  }
  DEBUG_PRINT("[ZCAS_INIT]enter zcas_loadconfig[%s]\n",filename);  
  fp = fopen(filename,"r");
  if(NULL == fp){
    printf("[ZCAS_INIT]open file failed[%s].\n",filename);
    return 1;
  }

  while(!feof(fp)){
    memset(str,'\0',sizeof(str));
    if(NULL == fgets(str,sizeof(str),fp))
      continue;/* windows error,原来是break */
    if(NULL != strchr(str,'#'))
      continue;

    memset(key,'\0',sizeof(key));
    memset(value,'\0',sizeof(value));
    sscanf(str,"%[^'=']%*c%s",key,value);
    DEBUG_PRINT("zcas_loadconfig[key:%s_value:%s]\n",key,value);
    if (strcmp(key,"threldmode")==0)
      config->threldmode = atoi(value);
    if (strcmp(key,"percent")==0)
      config->outpercent = atof(value);
    if (strcmp(key,"sentcount")==0)
      config->outcount = atoi(value);
    if (strcmp(key,"wordcount")==0)
      config->wordcount = atoi(value);
    if (strcmp(key,"debugmode")==0)
      config->debugmode = atoi(value);
    if (strcmp(key,"thememode")==0)
      config->thememode = atoi(value);
    if (strcmp(key,"partbytes")==0)
      config->partbytes = atoi(value);
    if (strcmp(key,"paracount")==0)
      config->paracount = atoi(value);
    if (strcmp(key,"topicsent")==0)
      config->topicsent = atoi(value);
    if (strcmp(key,"bestout")==0)
      config->bestout = atoi(value);
    if (strcmp(key,"conjword")==0)
      config->conjword = atoi(value);
    if (strcmp(key,"freqword")==0)
      config->freqword = atoi(value);
    if (strcmp(key,"ZCFC_CONF_PATH")==0)
      strcpy(config->zcfc_confdir,value);
    if (strcmp(key,"ZCAS_DICT_PATH")==0)
      strcpy(config->zcas_dicdir,value);
    if (strcmp(key,"ZCAS_LICENSE_PATH")==0)
      strcpy(config->zcas_license_dir,value);
  }
  fclose(fp);
  DEBUG_PRINT("[ZCAS_INIT]exit zcas_loadconfig[%s]\n",filename);
  return ZCAS_OK;
}


/*功能：验证授权文件
**参数：path-授权文件路径
**返回值：ZCAS_OK-成功，其他-失败
*/
int zcas_verifylicense(char *path)
{
  int ret = -1;

  if(NULL == path){
    printf("[ZCAS_INIT]zcas_verifylicense:input invalid(0x%x).",path);
    return -1;
  }
  DEBUG_PRINT("enter zcas_verifylicense.\n");
  ret = tool_verifylicense(path);
  DEBUG_PRINT("exit zcas_verifylicense.\n");

  return ret;
}


/*功能：加载二进制词典 
**参数：
**返回值：0-成功，其他-失败
*/
int zcas_loadbinarydict(zcas_config *config,char *filename)
{
  int ret = -1;
  int i = 0;
  zcas_head head[10];
  zcas_dicthead dicthead;
  FILE *fp = NULL;
  
  if(NULL == filename || NULL == config){
    printf("[ZCAS_INIT]zcas_loadbinarydict:input invalid(0x%x,0x%x).\n",filename,config);
    return -1;
  }

  DEBUG_PRINT("enter zcas_loadbinarydict.\n");
  fp = fopen(filename,"rb");
  if(NULL == fp){
    printf("open file[%s] failed!\n",filename);
    return 1;
  }
  
  /* 读取文件总头 */
  if(fread(head,sizeof(zcas_head),10,fp) <= 0){
    printf("zcas_loadbinarydict:read zcas head failed.\n");
    fclose(fp);
    DEBUG_PRINT("exit zcas_loadbinarydict.\n");
    return -1;
  }

  /* 读取词典头 */
  for(i = DEPART;i<=CONJWORDS;i++){
    if(fread(&dicthead,sizeof(zcas_dicthead),1,fp) <= 0){
      printf("zcas_loadbinarydict:read zcas_dicthead failed.\n");
      fclose(fp);
      DEBUG_PRINT("exit zcas_loadbinarydict.\n");
      return 1;
    }
    /* printf("fileflag=0x%x,bytest=%d\n",dicthead.fileflag,dicthead.nbytes);  */
    if(dicthead.fileflag != (0xff00+i)){
      printf("zcas.dct文件头校验失败，dict:%s,flag=0x%x(0x%x)\n",head[i].dictname,dicthead.fileflag,0xff00+i);
      fclose(fp);
      DEBUG_PRINT("exit zcas_loadbinarydict.\n");
      return 2;
    }

    if(DEPART == i){
      ret = zcas_loadPuncts(fp,&dicthead,config->epuncts,config->cpuncts,config->erends,config->crends);
      if(ret != ZCAS_OK){
	    printf("zcas_loadPuncts error.\n");
	    fclose(fp);
	    DEBUG_PRINT("exit zcas_loadbinarydict.\n");
	    return 3;
      }
    }
    else if(WEIGHTWD == i)
      ret = zcas_loadWeightWords(fp,&dicthead,&config->weightwords);
      if(ret != ZCAS_OK){
	    printf("zcas_loadWeightWords error.\n");
	    fclose(fp);
	    DEBUG_PRINT("exit zcas_loadbinarydict.\n");
	    return 4;
      }
    else if(CONJWORDS == i)
      ret = zcas_loadConjwordDict(fp,&dicthead,&config->conjwords,&config->freqwords);
      if(ret != ZCAS_OK){
	    printf("zcas_loadConjwordDict error.\n");
	    fclose(fp);
	    DEBUG_PRINT("exit zcas_loadbinarydict.\n");
	    return 5;
      }
  }

  fclose(fp);
  DEBUG_PRINT("exit zcas_loadbinarydict.\n");
  return ZCAS_OK;
}


/*功能:加载标点词典
**参数：...
**返回值：ZCAS_OK-成功
*/
int zcas_loadPuncts(FILE* fp,zcas_dicthead *head,char*epuncts,char*cpuncts,char*erends,char*crends)
{
  char key[50],value[50];/* 标点词典每次读取最多50bytes*/
  char *line = NULL;
  
  DEBUG_PRINT("[ZCAS_INIT]enter zcas_loadpunct.\n");
  if(NULL == fp || NULL == head || epuncts == NULL || NULL == cpuncts || NULL == erends || NULL == crends){
    printf("input para error[0x%x,0x%x,0x%x,0x%x,0x%x,0x%x].\n",fp,head,epuncts,cpuncts,erends,crends);
    DEBUG_PRINT("[ZCAS_INIT]exit zcas_loadpunct.\n");
    return -1;
  }

  line = (char *)malloc(head->nbytes);
  while(head->size > 0){
    double len = 0;
    if(fread(&len,sizeof(double),1,fp) <= 0){
      printf("zcas_loadPuncts:read head bytes failed.\n");
      free(line);
      fclose(fp);
      DEBUG_PRINT("[ZCAS_INIT]exit zcas_loadpunct.\n");
      return -1;
    }

    memset(line,'\0',head->nbytes);
    memset(key,'\0',sizeof(key));
    memset(value,'\0',sizeof(value));
    if(fread(line,sizeof(char),(size_t)len,fp) <= 0){
      printf("zcas_loadPuncts:read dict line failed.\n");
      free(line);
      fclose(fp);
      DEBUG_PRINT("[ZCAS_INIT]exit zcas_loadpunct.\n");
      return -1;
    }
    
    sscanf(line,"%[^'\t'' '] %s",key,value);
    if (strcmp(key,"EPUNCT")==0)
      strcpy(epuncts,value);
    if (strcmp(key,"CPUNCT")==0)
      strcpy(cpuncts,value);
    if (strcmp(key,"EREND")==0)
      strcpy(erends,value);
    if (strcmp(key,"CREND")==0)
      strcpy(crends,value);

    DEBUG_PRINT("len=%lf[%s,%s]\n",len,key,value);
    head->size -= (len+sizeof(double));
  }
  free(line);
  DEBUG_PRINT("[ZCAS_INIT]loadpunct(ep:%s,cp:%s,er:%s,cr:%s\n)",epuncts,cpuncts,erends,crends);
  DEBUG_PRINT("[ZCAS_INIT]exit zcas_loadpunct.\n");

  return 0;
}


/*功能：加载关联词词库，得到freqwords和conjwords
**参数：fp-关联词典文件指针，head-文件头结构指针，conjwords-关联词存储结构，freqwords-词频存储结构
**返回值：ZCAS_OK-成功，其他-失败
*/
int zcas_loadConjwordDict(FILE *fp,zcas_dicthead *head,UTHASH_CONJWORDS **conjwords,UTHASH_DOUBLE **freqwords)
{
  double maxfreq = 0;
  double freq;
  char key[30];
  char sfreq[30];
  char value[1000];/* conjword每次最多读取1000bytes */
  const char *split = "/";
  char *str = NULL;
  
  double weight;
  char *line = NULL;

  DEBUG_PRINT("enter zcas_loadConjwordDict\n");
  if(NULL == fp || NULL == head || NULL == conjwords || NULL == freqwords){
    printf("input para error[0x%x,0x%x,0x%x,0x%x].\n",fp,head,conjwords,freqwords);
    DEBUG_PRINT("exit zcas_loadConjwordDict\n");
    return -1;
  }

  line = (char*)malloc(head->nbytes);
  while(head->size > 0){
    int loadcount = 0;/* 控制加载关联词词数，只加载20个 */
    double len = 0;
    conjwordlist *pwords = NULL;
    if(fread(&len,sizeof(double),1,fp) <= 0){
      printf("zcas_loadConjwordDict:read head lenth failed.\n");
      free(line);
      fclose(fp);
      DEBUG_PRINT("exit zcas_loadConjwordDict\n");
      return -1;
    }

    memset(line,'\0',head->nbytes);
    if(fread(line,sizeof(char),(size_t)len,fp) <= 0){
      printf("zcas_loadConjwordDict:read head bytes failed.\n");
      free(line);
      fclose(fp);
      DEBUG_PRINT("exit zcas_loadConjwordDict\n");
      return -1;
    }
    
    freq = 0;
    memset(key,'\0',sizeof(key));
    memset(value,'\0',sizeof(value));
    
    sscanf(line,"%[^'\t']%*c%[^':']%*c%s",key,sfreq,value);
    /*printf("%s,%s:",key,sfreq);*/
    freq = atof(sfreq);
    zcas_UTHASH_DOUBLE_add(freqwords,key,freq);

    if(freq > maxfreq){
      maxfreq = freq;
    }

    str = strtok(value,split);
    while(NULL != str){
      double wordcount = 0;
      char word[30];
      CONJWORD conjword;
      if(loadcount > 20){
	    break;
      }
      memset(word,'\0',sizeof(word));
      weight = 0;
      sscanf(str,"%[^',']%*c%lf",word,&wordcount);
      if(freq > 0.000001){
	    weight = wordcount/freq;
      }
      strcpy(conjword.key,word);
      conjword.value = weight;
      zcas_conjwordlist_addtail(&pwords,conjword);
      loadcount++;
      /* printf("%s,%lf/",word,weight); */
      str = strtok(NULL,split);
    }
    /* printf("\n");*/
    if(NULL != pwords){
      zcas_UTHASH_CONJWORDS_add(conjwords,key,pwords);
    }

    head->size -= (len+sizeof(double));
  }

  free(line);
  if(maxfreq != 0){
    char maxkey[] = "MAXFREQ";
    zcas_UTHASH_DOUBLE_add(freqwords,maxkey,maxfreq);
  }
  DEBUG_PRINT("exit zcas_loadConjwordDict\n");
  return ZCAS_OK;
}


/*功能：加载权重词典
**参数：
**返回值：ZCAS_OK-成功，其他-失败
*/
int zcas_loadWeightWords(FILE *fp,zcas_dicthead *head,UTHASH_DOUBLE **wtwds)
{
  char key[50];/* weightword词典每次最多读取50bytes*/
  double value;
  char *line = NULL;
  int ret = ZCAS_OK;

  DEBUG_PRINT("enter zcas_loadWeightWords.\n");
  if(NULL == fp || NULL == head || NULL == wtwds){
    printf("input para error[0x%x,0x%x,0x%x].\n",fp,head,wtwds);
    ret = -1;
    DEBUG_PRINT("exit zcas_loadWeightWords.\n");
    return ret;
  }

  line = (char *)malloc(head->nbytes);
  while(head->size > 0){
    double len = 0;
    if(fread(&len,sizeof(double),1,fp) <= 0){
      printf("zcas_loadWeightWords:read dict bytes failed.\n");
      ret = 1;
      break;
    }

    memset(line,'\0',head->nbytes);
    memset(key,'\0',sizeof(key));
    if(fread(line,sizeof(char),(size_t)len,fp) <= 0){
      printf("zcas_loadWeightWords:read dict line failed.\n");
      ret = 1;
      break;
    }
    sscanf(line,"%[^'\t'' '] %lf",key,&value);
    if(strlen(key)>0)
      zcas_UTHASH_DOUBLE_add(wtwds,key,value);

    DEBUG_PRINT("len=%lf[%s,%lf]\n",len,key,value);
    head->size -= (len+sizeof(double));
  }
  free(line);

  DEBUG_PRINT("exit zcas_loadWeightWords.\n");
  return ret;
}


/*功能：加载zcfc
**参数：dir-zcfc词典路径
**返回值：zcfc句柄
*/
zcfc_handle* zcas_loadzcfcdict(const char* dir)
{
  zcfc_handle* zcfchdl = NULL;/* zcfc暂时屏蔽 */

  if(NULL == dir){
    printf("zcas_loadzcfcdict:input invalid(0x%x).\n",dir);
    return NULL;
  }
  DEBUG_PRINT("enter zcas_loadzcfcdict,path:%s\n",dir);
  zcfchdl = (zcfc_handle *)zcfc_init((ZCFCSTR*)dir); 
  DEBUG_PRINT("zcfc_handle=0x%x\n",zcfchdl);

  DEBUG_PRINT("exit zcas_loadzcfcdict.\n");
  return zcfchdl;
}


/*功能：加载zcas词典
**参数：config-zcas句柄，filename-zcas词典
**返回值：ZCAS_OK-成功，其他-失败
*/
int zcas_loadzcasdict(zcas_config *config,char* filename)
{
  int ret = -1;
  DEBUG_PRINT("enter zcas_loadzcasdict\n");
  if(NULL == config || NULL == filename){
    printf("[ZCAS_INIT]zcas_loadzcasdict:input invalid(0x%x,0x%x).\n",config,filename);
    DEBUG_PRINT("exit zcas_loadzcasdict\n");
    return -1;
  }
  ret = zcas_loadbinarydict(config,filename);

  DEBUG_PRINT("exit zcas_loadzcasdict\n");

  return ret;
}


/*功能：分割字符串
**参数：s-待分割字符串，delim-分割字符串
**返回值：结果链表
*/
stringList *zcas_split(char* s,const char* delim,int includeDelim)
{
  int  n = 0;
  int ok = 0;
  int t=0;
  int nFlag = 0;
  unsigned int last = 0;
  unsigned int i,j;
  char *str = NULL;
  stringList *ret = NULL;

  if(NULL == s || NULL == delim){
    return NULL;
  }

  if (includeDelim){
    t= strlen(delim);
  }

  for (i = 0; i + strlen(delim) <= strlen(s); i++){
    ok = 1;
    for (j = 0; j < strlen(delim) && ok; j++){
      ok = s[i + j] == delim[j];
    }
    if (ok){
      if (i - last){
	    str = (char*)malloc(i-last+t+1);
	    memset(str,'\0',i-last+t+1);
	    strncpy(str,s+last,i-last+t);
	    zcas_stringlist_addtail(&ret,str);
	    free(str);
      }
      last = i + strlen(delim);
    }
  }

  if(last < strlen(s)){
    str = (char*)malloc(strlen(s)-last+1);
    memset(str,'\0',strlen(s)-last+1);
    strcpy(str,s+last);
    zcas_stringlist_addtail(&ret,str);
    free(str);
  }
  return ret;
}


/*功能：设置调试开关
**参数：mode-调试开关，0-关闭，1-打开
**返回值：无
*/
void zcas_setdebugmode(int mode)
{
  DEBUG_PRINT("enter zcas_setdebugmode.\n");
  gprintlevel = mode;
  DEBUG_PRINT("exit zcas_setdebugmode.\n");
}
