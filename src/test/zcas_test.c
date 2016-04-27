#include <stdio.h>

#ifdef OS_LINUX
#include <unistd.h>
#include <getopt.h>
#endif

#include "zcas.h"

#ifndef OS_LINUX
#include <windows.h>
#define PAUSE system("pause")
#ifdef _UNICODE
void TcharToChar (const TCHAR * tchar, char * _char)  
{  
    int iLength ;  
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);     
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);   
}  
 void CharToTchar (const char * _char, TCHAR * tchar)  
{  
    int iLength ;  
    iLength = MultiByteToWideChar (CP_ACP, 0, _char, strlen (_char) + 1, NULL, 0) ;  
    MultiByteToWideChar (CP_ACP, 0, _char, strlen (_char) + 1, tchar, iLength) ;  
}
#endif
#else
#define PAUSE
#define TCHAR char
#endif

char *zcas_getrootpath(char *input)
{
  char *rpath = NULL;
  char *real_path = NULL;
  char *tmp = NULL;
  char plat[128] = {'\0'};
  TCHAR treal_path[1024] = {'\0'};
  
  real_path = (char*)malloc(1024);
  memset(real_path,'\0',1024);

#ifdef OS_LINUX
  if(0 == strcmp(input,"zcas")){
    strcpy(real_path,"/usr/local/lib");
    return real_path;
  }

  rpath = realpath(input,NULL);
  if(NULL == rpath){
    printf("get realpath failed.");
    return NULL;
  }
  strcpy(real_path,rpath);
  free(rpath);
  if(4 == sizeof(int *)){
    strcpy(plat,"/bin/x86/");
  }
  else{
    strcpy(plat,"/bin/x64/");
  }
  if(NULL != (tmp=strstr(real_path,plat))){
    memset(tmp,'\0',strlen(tmp));
    //sprintf(infile_path,"%s/example/示例文章.txt",real_path);
  }
  else if(NULL != (tmp=strstr(real_path,"/src/test/"))){
    memset(tmp,'\0',strlen(tmp));
    //sprintf(infile_path,"%s/data/example/std/bmd.txt",real_path);
  }
  else if(NULL != (tmp=strstr(real_path,"/bin/zcas"))){
    memset(tmp,'\0',strlen(tmp));
  }
  else{
    //printf("get zcas root path failed.\n");
    return NULL;
  }
#else
  GetModuleFileName(NULL,treal_path,1024);
#ifdef _UNICODE
  TcharToChar(treal_path,real_path);
#else
  strcpy(real_path,treal_path);
#endif
  tmp=strstr(real_path,"\\vcprj\\test\\");
  if(tmp != NULL){
    memset(tmp,'\0',strlen(tmp));
    //sprintf(infile_path,"%s\\data\\example\\std\\bmd_win.txt",real_path);
  }
  else if(NULL != (tmp=strstr(real_path,"\\example\\"))){
    memset(tmp,'\0',strlen(tmp));
    //sprintf(infile_path,"%s\\example\\示例文章.txt",real_path);
  }
  else{
    //printf("get zcas root path failed.\n");
    return NULL;
  }
#endif
  
  return real_path;
}

void zcas_writefile(char *filename,char *text)
{
  FILE *fp = fopen(filename,"w");
  if(fp == NULL){
    printf("zcas_writefile:open file failed,%s\n",filename);
  }

  fwrite(text,sizeof(char),strlen(text),fp);
  fclose(fp);
  return;
}

int main(int argc,char** argv)
{
  int key = -1;
  zcas_config *config = NULL;
  char *instr = NULL;
  char *outtext = NULL;
  int ret = -1;
  int err = -1;
  char target[1024*100];
  char *detected = NULL;
  char *real_path = NULL;

  char conf_path[1024] = {'\0'};
  char infile_path[1024] = {'\0'};
  char outfile_path[1024] = {'\0'};
  int mode = -1;
  int debugmode = -1;
  int wordcount = -1;
  double percent = -1;

  int thememode = -1;
  int partbytes = -1;
  int paracount = -1;
  int sentcount = -1;
  int topicsent = -1;
  int bestout = -1;
  int conjword = -1;
  int freqword = -1;
  int articletype = -1;

#ifdef OS_LINUX
  char* const short_options = "c:i:o:m:s:p:w:d:t:b:n:e:0:1:2:3vh"; //带参数的需要加：，如c：，i:,不带参数的不加：，如vh
  struct option long_options[]={
    {"config",   1,NULL,'c'},
    {"input",    1,NULL,'i'},
    {"output",   1,NULL,'o'},
    {"mode",     1,NULL,'m'},
    {"sentcount",1,NULL,'s'},
    {"percent",  1,NULL,'p'},
    {"wordcount",1,NULL,'w'},
    {"debugmode",1,NULL,'d'},
    {"thememode",1,NULL,'t'},
    {"partbytes",1,NULL,'b'},
    {"paracount",1,NULL,'n'},
    {"topicsent",1,NULL,'e'},
    {"bestout",  1,NULL,'0'},
    {"conjword", 1,NULL,'1'},
    {"freqword", 1,NULL,'2'},
    {"articletype",1,NULL,'3'},
    {"version",  0,NULL,'v'},
    {"help",     0,NULL,'h'},
    {0,0,0,0}
  };
  while((key = getopt_long (argc,argv,short_options,long_options,NULL)) != -1) {
    switch (key) {
    case 'c':
      strcpy(conf_path, optarg);
      break;
    case 'i':
      strcpy(infile_path,optarg);
      break;
    case 'o':
      strcpy(outfile_path,optarg);
      break;
    case 'm':
      mode = atoi(optarg);
      if(mode != 0 && mode != 1){
	      printf("参数错误:-m 只能为0或者1\n");
	      PAUSE;
	      return -1;
	    }
      break;
    case 's':
      sentcount = atoi(optarg);
      if(sentcount <= 0){
	      printf("参数错误:-s 必须是大于0的整数\n");
	      PAUSE;
	      return -1;
	    }
      break;
    case 'p':
      percent = atof(optarg);
      if(percent <= 0 || percent >= 1){
	      printf("参数错误:-p 必须是大于0且小于1的小数\n");
	      PAUSE;
	      return -1;
	    }
      break;
    case 'w':
      wordcount = atoi(optarg);
      if(wordcount <= 0){
	      printf("参数错误:-w 必须是大于0的整数\n");
	      PAUSE;
	      return -1;
	    }
      break;
    case 'd':
      {
	    debugmode = atoi(optarg);
	    if(debugmode != 0 && debugmode != 1){
	      printf("参数错误:-d 只能为0或者1\n");
	      PAUSE;
	      return -1;
	    }
	    break;
      }
    case 't':
      {
	    thememode = atoi(optarg);
	    if(thememode < 0 || thememode > 3){
	      printf("参数错误:-t --thememode 只能为0到3之间的整数\n");
	      PAUSE;
	      return -1;
	    }
	    break;
      }
    case 'b':
      {
	    partbytes = atoi(optarg);
	    if(partbytes <= 0){
	      printf("参数错误:-b  --partbytes 只能为大于0的整数\n");
	      PAUSE;
	      return -1;
	    }
	    break;
      }
    case 'n':
      {
	    paracount = atoi(optarg);
	    if(paracount <= 0){
	      printf("参数错误:-n --paracount 只能为大于0的整数\n");
	      PAUSE;
	      return -1;
	    }
	    break;
      }
    case 'e':
      {
	    topicsent = atoi(optarg);
	    if(topicsent < 0){
	      printf("参数错误:-e --topicsent 只能为大于等于0的整数\n");
	      PAUSE;
	      return -1;
	    }
	    break;
      }
    case '0':
      {
	    bestout = atoi(optarg);
	    if(bestout!=0 && bestout!=1){
	      printf("参数错误:--bestout 只能为0或者1\n");
	      PAUSE;
	      return -1;
	    }
	    break;
      }
    case '1':
      {
	    conjword = atoi(optarg);
	    if(conjword!=0 && conjword!=1){
	      printf("参数错误:--conjword 只能为0或者1\n");
	      PAUSE;
	      return -1;
	    }
	    break;
      }
    case '2':
      {
	    freqword = atoi(optarg);
	    if(freqword!=0 && freqword!=1){
	      printf("参数错误:--freqword 只能为0或者1\n");
	      PAUSE;
	      return -1;
	    }
	    break;
      }
    case '3':
      {
	    articletype = atoi(optarg);
	    if(articletype < 0 || articletype > TYPE_OTHERS){
	      printf("参数错误：--articletype 只能为%d到%d之间的整数\n",TYPE_NONE,TYPE_OTHERS);
	      PAUSE;
	      return -1;
	    }
	    break;
      }
    case 'v':
      {
	    printf("zcas version:\"2.2.3\",Copyright (C) 2013-2015,by zctech.\n");
	    PAUSE;
	    return -1;
      }
    case 'h':
      {
	    printf("用法：zcas [-i 输入文件] [-o 输出文件] [-c 配置文件]\n");
	    printf("对输入文件进行摘要，结果保存在输出文件\n\n");
	    printf("选项:\n");
	    printf("    -i  --input     输入文件路径，可缺省，缺省时输入example目录下的示例文章\n");
	    printf("    -o  --output    输出文件路径，可缺省，缺省时摘要结果在控制台输出\n");
	    printf("    -c  --config    配置文件路径，可缺省，缺省时为默认配置\n");
	    printf("    -m  --mode      摘要模式，只能为0或1，0-按百分比，1-按句子数；可缺省，缺省时为默认配置\n");
	    printf("    -s  --sentcount 按句子数摘要，参数只能是大于0的整数，可缺省，缺省时为默认配置，-p与-s只能有一个\n");
	    printf("    -p  --percent   按百分比摘要，参数只能是大于0且小于1的数，可缺省，缺省时为默认配置，-p与-s只能有一个\n");
	    printf("    -w  --wordcount 摘要的最大字数，参数只能是大于0的整数，可缺省，缺省为默认配置\n");
	    printf("    -d  --debugmode 调试模式，只能为0或1，0-关闭调试，1-打开调试开关；可缺省，缺省时为默认配置\n");
	    printf("    -v  --version   版本信息\n");
	    printf("    -h  --help      帮助信息\n");
	    printf("更多选项：\n");
	    printf("    -t  --thememode 多主题模式，可缺省，缺省时按单主题摘要，参数只能整数，0-整篇文章作为part,1-按标题分part，2-按字节数分part，3-按段落数分part\n");
	    printf("        --partbytes 多主题模式下，按字节数划分文章时的单part字节数，参数只能是大于0的整数，可缺省，缺省时为默认配置\n");
	    printf("        --paracount 多主题模式下，按段落数划分文章时的单part段落数，参数只能是大于0的整数，可缺省，缺省时为默认配置\n");
	    printf("        --topicsent 主题句加权开关，只能为0或者1，0-不加权，1-加权，可缺省，缺省时为默认配置\n");
	    printf("        --conjword  关联词使用开关，只能为0或者1，0-关闭，1-打开，可缺省，缺省时为默认配置\n");
	    printf("        --freqword  词频调整开关，只能为0或者1，0-关闭，1-打开，可缺省，缺省时为默认配置\n");
	    printf("        --bestout   最佳输出开关，只能为0或者1，0-关闭，1-打开，可缺省，缺省时为默认配置\n");
	    printf("        --articletype   设置文章类型：无类型、新闻类、其他，可缺省，缺省时为无类型\n");
	    printf("注：默认配置为zcas根目录下的/conf/zcas.conf中的配置。\n\n");
	    printf("示例:\n");
	    printf("    zcas -i ./bmd.txt -o ./result.txt -c ./conf/\n");
	    printf("    zcas -i ./bmd.txt\n");
	    PAUSE;
	    return -1;
      }
    default:
      break;
    }
  }
#endif

  /* 获取zcas根目录 */
  real_path =  zcas_getrootpath(argv[0]);
  if(NULL == real_path){
    //printf("[MAIN]warning:get zcas root path failed.\n");
    PAUSE;
    //return -1;
  }

  //输入检查：配置文件路径
  if(0 == strlen(infile_path)){
    printf("参数不完整：摘要文章不能为空，请使用\"-i\"指定摘要文章，如：\n");
    printf("  zcas -i [输入文章] [更多参数...]\n");
    printf("  请尝试执行“zcas -h”来获取更多信息。\n");
    free(real_path);
    return -1;
  }
 
  //输入检查：配置文件路径
  if(0 == strlen(conf_path)){
    if(NULL != real_path){
#ifdef OS_LINUX
      if(0 == strcmp(real_path,"/usr/local/lib"))
	sprintf(conf_path,"/etc/zcas/conf/");
      else
	sprintf(conf_path,"%s/conf/",real_path);
#else
      sprintf(conf_path,"%s\\conf\\",real_path);
#endif
    }
    else{
      printf("参数不完整：请使用\"-c\"输入配置文件路径，\"-i\"输入摘要文章，如：\n");
      printf("  zcas -c ./conf/ -i ./test.txt\n");
      printf("  请尝试执行“zcas -h”来获取更多信息。\n");
      free(real_path);
      return -1;
    }
  }

  //输入检查：摘要文章
  if(0 == strlen(infile_path)){
    if(NULL != real_path){
#ifdef OS_LINUX
 if(NULL == strstr(argv[0],"src/test/"))
    sprintf(infile_path,"%s/example/示例文章.txt",real_path);
 else
    sprintf(infile_path,"%s/data/example/std/example_utf8.txt",real_path);
#else
  if(NULL == strstr(argv[0],"src\\test\\"))
    sprintf(infile_path,"%s\\example\\示例文章.txt",real_path);
  else
    sprintf(infile_path,"%s\\data\\example\\std\\example_gbk.txt",real_path);
#endif
    }
    else{
      printf("参数不完整：请使用\"-c\"输入配置文件路径，\"-i\"输入摘要文章，如：\n");
      printf("  zcas -c ./conf/ -i ./test.txt\n");
      printf("  请尝试执行“zcas -h”来获取更多信息。\n");
      free(real_path);
      return -1;
    }
  }
  free(real_path);

  /* 开启调试开关 */
  if(debugmode > 0){
    zcas_setdebugmode(debugmode);
  }

  /* 1.加载词典、配置文件 */
  ret = zcas_init(conf_path,&config);
  if(0 != ret){
    printf("zcas初始化失败.errcode:%d\n",ret);
	PAUSE;
    return -1;
  }

  /* 设置输出模式 */
  if(percent > 0){
    zcas_setopt_outpercent(config,percent);
  }
  if(sentcount > 0){
    zcas_setopt_outcount(config,sentcount);
  }
  if(mode >= 0){
    zcas_setopt_threldmode(config,mode);
  }
  else{
    if(percent >0 && sentcount < 0)
      zcas_setopt_threldmode(config,0);
    else if(percent < 0 && sentcount > 0)
      zcas_setopt_threldmode(config,1);
  }

  /*  
  zcas_setopt_outpercent(config,percent);
  zcas_setopt_outcount(config,sentcount);
  zcas_setopt_wordcount(config,wordcount);*/
  if(wordcount >= 0)
    zcas_setopt_wordcount(config,wordcount);
  if(thememode >= 0)
    zcas_setopt_thememode(config,thememode);
  if(partbytes > 0)
    zcas_setopt_partbytes(config,partbytes);
  if(paracount > 0)
    zcas_setopt_paracount(config,paracount);
  if(topicsent > 0)
    zcas_setopt_topicsent(config,topicsent);
  if(bestout >= 0)
    zcas_setopt_bestout(config,bestout);
  if(conjword >= 0)
    zcas_setopt_conjword(config,conjword);
  if(freqword >= 0)
    zcas_setopt_freqword(config,freqword);
  if(articletype >=0)
    zcas_setopt_articletype(config,articletype);

  /* 3.输入文档 */
  instr = zcas_ReadAllFromFile(infile_path);
  if(NULL == instr){
    printf("[MAIN]读取输入文件失败(%s),请检查文件是否存在.\n",infile_path);
    PAUSE;
    return -1;
  }

#ifndef OS_LINUX
  if(ZCAS_OK != zcas_detectTextEncoding(instr,strlen(instr),&detected)){
    printf("[MAIN]获取文件编码失败.\n");
    PAUSE;
    return -1;
  }
  if(NULL != detected && NULL == strstr(detected,"UTF-8")){
    memset(target,'\0',sizeof(target));
    if(zcas_convert("UTF-8", detected, target,sizeof(target), (const char*)instr, strlen(instr)) != 0){   
      printf("[MAIN]ucnv_convert error");
      free(detected);
      PAUSE;
      return -1;
    }
    zcas_free(detected);
    memset(instr,'\0',strlen(instr)+1);
    strcpy(instr,target);
  }
#endif

  /* 4.getsummary */
  outtext = zcas_getsummary(config,instr);
  if(NULL == outtext){
    printf("[MAIN]getsummary failed.\n");
    zcas_free(instr);
    ret = zcas_releasesummary(config);
    if(0 != ret)
      printf("[MAIN]release summary failed.\n");
    PAUSE;
    return -1;
  }

  if(0 != strlen(outfile_path)){
    /* 写输出文件 */
    zcas_writefile(outfile_path,outtext);
  }
  else{
#ifdef OS_LINUX
    printf("%s\n",outtext);
#else
    memset(target,'\0',sizeof(target));
    if(zcas_convert("GB18030","UTF-8",target,sizeof(target), (const char*)outtext, strlen(outtext)) != 0){
      printf("[MAIN]ucnv_convert error");
      PAUSE;
      return -1;
    }
    printf("%s\n",target);
#endif
  }
  
  /* 5.释放资源 */
  zcas_free(instr);
  zcas_free(outtext);
  ret = zcas_releasesummary(config);
  if(ret != 0){
    printf("[MAIN]释放资源失败！\n");
    PAUSE;
    return -1;
  }

  PAUSE;
  return 0;
}
