#include "zctool_license.h"

/* md5加密,密码必须为字母、数字等组成 */
char* tool_md5(char *instr,int n)
{
  unsigned int i;
  char *out = NULL;
  unsigned char decrypt[16];      
  MD5_CTX md5; 

  //printf("tool_md5:in =%s\n",instr);
  out = (char *)malloc(64);
  memset(out,'\0',64);

  MD5Init(&md5);                
  MD5Update(&md5,(unsigned char *)instr,strlen(instr));  
  MD5Final(&md5,decrypt);   

  for(i=0;i<16;i++){
    char tmp[3];
    memset(tmp,'\0',2);
    sprintf(tmp,"%02x",decrypt[i]);
    strcat(out,tmp);
  }
  
  for(i=0;i<strlen(out);i++){
    out[i] += n;
    if(out[i]>'9' && out[i]<='9'+n){
      out[i] = out[i]-10;
    }
    if((out[i]>'Z' && out[i]<='Z'+n) || out[i]>'z'){
      out[i] = out[i]-26;
    }
  }
  
  //printf("tool_md5:out=%s\n",out);
  return out;
}

/* 加密 */
char* tool_encrypt(char *in){
  char *out = NULL;
  unsigned int i = 0;
  int n = 5;

  //printf("encrypt:in=%s\n",in);
  /* 字符按位取反 */
  out = (char*)malloc(strlen(in)+1);
  memset(out,'\0',strlen(in)+1);  
  for(i=0;i<strlen(in);i++){
    out[i] = in[i]+n;
    out[i] = ~out[i];
  }

  //printf("encrypt:out=%s\n",out);
  return out;
}
/* 解密 */
char* tool_decrypt(char *in)
{
  char *out = NULL;
  unsigned int i = 0;
  int n = 5;

  //printf("decrypt:in=%s\n",in);
  /* 16进制转化为字符 */
  out = (char*)malloc(strlen(in)+1);
  memset(out,'\0',strlen(in)+1);
  for(i=0;i<strlen(in);i++){
    out[i] = ~in[i];
    out[i] = out[i]-n;
  }

  //printf("decrypt:out=%s\n",out);
  return out;
}

/* 验证密码是否正确*/
int tool_verifylicense(char *filename)
{
  int flag = 0;
  FILE *fp = NULL;
  zcas_license lc;
  char *usrinfo = NULL;
  time_t now;
  struct tm *timenow;

  fp = fopen(filename,"rb");
  if(NULL == fp){
    printf("open file failed,please check whether the file is existed.[%s].\n",filename);
    return 1;
  }
  if(fread(&lc,sizeof(zcas_license),1,fp) <= 0){
    printf("read file failed,the file is [%s].\n",filename);
    return 4;
  }

  usrinfo = tool_md5(lc.usrinfo,5);
  if(NULL != usrinfo && 0 != strcmp(lc.crc,usrinfo)){
    printf("this program is unauthorized,please obtain authorization before using.\n");
    fclose(fp);
    return 2;
  }
  free(usrinfo);
  
  time(&now);
  timenow = localtime(&now);
  if((((timenow->tm_year+1900)-lc.year)*365+(timenow->tm_yday-lc.day)) > lc.workdays){
    printf("this program is overdue，please obtain authorization before using.\n");
    fclose(fp);
    return 3;
  }

  fclose(fp);
  return 0;
}


