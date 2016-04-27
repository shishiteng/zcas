#include "zctool_license.h"

/* 创建授权书 */
int tool_createlicense(char *filename,char *input,int workdays)
{
  zcas_license lc;
  FILE *fp = NULL;
  char *usrinfo = NULL;
  char timestr[40];
  time_t now;
  struct tm *timenow;
#if 0
  wchar_t *winput = NULL;
  DWORD dwNum = 0;
#endif

  //  memset(timestr,'\0',sizeof(timestr));
  time(&now);
  timenow = localtime(&now);
  strftime(timestr,100,"%Y-%m-%d %H:%M:%S",timenow); 

  fp = fopen(filename,"a");
  if(NULL == fp){
    printf("创建文件失败[%s].\n",filename);
    return -1;
  }

  memset(lc.usrinfo,'\0',sizeof(lc.usrinfo));
  memset(lc.crc,'\0',sizeof(lc.crc));
  memset(lc.desc,'\0',sizeof(lc.desc));

  lc.year = timenow->tm_year+1900;
  lc.day = timenow->tm_yday;
  lc.workdays = workdays;

  /* linux默认使用utf-8编码，windows下字符做统一处理：修改为utf-8 */
  usrinfo = tool_encrypt(input);
  strcpy(lc.usrinfo,usrinfo);
  free(usrinfo);
#if 0/* windows编码转化 */
  dwNum = MultiByteToWideChar(CP_ACP,0,input,-1,NULL,0);
  winput = (wchar_t *)malloc(sizeof(wchar_t)*dwNum);
  memset(winput,'\0',sizeof(wchar_t)*dwNum);
  MultiByteToWideChar(CP_ACP,0,input,-1,winput,dwNum);
  usrinfo = zcas_TransformUnicodeToUTF8M(winput);
  strcpy(lc.usrinfo,usrinfo);
  free(usrinfo);
  free(winput);
#endif
  usrinfo = tool_md5(lc.usrinfo,5);
  strcpy(lc.crc,usrinfo);
  free(usrinfo);
  /* printf("%d,%d,%d,%s,%s\n",lc.year,lc.day,lc.workdays,lc.usrinfo,lc.crc); */

  fwrite(&lc,sizeof(zcas_license),1,fp);
  
  fclose(fp);
  return 0;
}

int main(int argc,char** argv)
{
  char *action = NULL;
  char *lcpath = NULL;
  char *usrinfo = NULL;
  int workdays = 0;

#if 0/* 加密测试 */
  char *en = NULL;
  char *de = NULL;
  //char *str = tool_md5("shishiteng",5);
  en = tool_encrypt("shishieng智呈网络科技");
  printf("len=%d\n",strlen("shishiteng智呈网络科技"));
  de = tool_decrypt(en);
#endif

  if(NULL == argv[1]){
    printf("请输入正确参数，argv1：create/verify\n");
    return -1;
  }
  if(NULL == strstr(argv[1],"create") && NULL == strstr(argv[1],"verify")){
    printf("请输入正确参数，argv1必须为create或verify\n");
    return -1;
  }

  //lcpath = "/home/zcshsht/work/zcautosummary/zcautosummary-2.0.0/src/tools/license/zclicense";;
  if(0 == strcmp(argv[1],"create")){
    if(NULL == argv[2] || NULL == argv[3] || NULL == argv[4]){
      printf("请输入完整参数：argv[2]-用户信息，argv[3]-有效天数，argv[4]-生成文件名\n");
      return -1;
    }
    usrinfo = argv[2];
    workdays = atoi(argv[3]);
    lcpath = argv[4];
    if(0 == tool_createlicense(lcpath,usrinfo,workdays)){
      printf("授权文件创建成功\n");
    }
  }
  else if(0 == strcmp(argv[1],"verify")){
    if(NULL == argv[2]){
      printf("请输入完整参数：argv[2]-生成文件名\n");
      return -1;
    }
    lcpath = argv[2];
    if(0 == tool_verifylicense(lcpath)){
      printf("授权文件验证成功\n");
    }
  }

  return 0;
}
