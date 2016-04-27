#ifndef ZCAS_COMMON_H
#define ZCAS_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef OS_LINUX
#include <windows.h>
#include <unicode/ucnv.h>
#include <unicode/utypes.h>  
#include <unicode/ucsdet.h>
#pragma comment(lib,"icuuc.lib")
#pragma comment(lib,"icuin.lib")
#pragma comment(lib,"zcfc.lib")
#endif

#include "zcas_struct.h"

/* uthash的操作*/
int zcas_UTHASH_DOUBLE_add(UTHASH_DOUBLE **pwords,char *key,double value);
int zcas_UTHASH_DOUBLE_find(UTHASH_DOUBLE *pwords,char *key,double *value);
void zcas_UTHASH_DOUBLE_sort(UTHASH_DOUBLE *pwords);
int zcas_UTHASH_DOUBLE_release(UTHASH_DOUBLE *map);

int zcas_UTHASH_CONJWORDS_add(UTHASH_CONJWORDS **pconwords,char *key,conjwordlist *value);
int zcas_UTHASH_CONJWORDS_find(UTHASH_CONJWORDS *pconwords,char *key,conjwordlist **pvalue);
int zcas_UTHASH_CONJWORDS_release(UTHASH_CONJWORDS *map);

/* list函数：添加节点，查找节点，释放链表 */
int zcas_stringlist_addtail(stringList **list,char *str);
int zcas_doublelist_addtail(doublelist **list,double element);
int zcas_conjwordlist_addtail(conjwordlist **list,CONJWORD element);

int zcas_stringlist_release(stringList *list);
int zcas_doublelist_release(doublelist *list);
int zcas_conjwordlist_release(conjwordlist *list);

conjwordlist* zcas_conjwordlist_find(conjwordlist *list,char *key);

/* 通用函数 */
DLLEXPORT char* zcas_ReadAllFromFile(char* filename);
char* zcas_strEncConvert(const char* fromStr,const char* fromEnc,const char* toEnc);

int zcas_getcombineval(int n,int m);/* 求组合的值 */
int zcas_getfactorial(int n);/* 求阶乘 */
int zcas_getbytesnum(char ch);/* 求字节数 */
int zcas_getwordcount(char *str);/* 求word数 */

#ifndef OS_LINUX
DLLEXPORT wchar_t* zcas_TransformUTF8ToUnicodeM(const char* _str);
DLLEXPORT char* zcas_TransformUnicodeToUTF8M(const wchar_t* _str);
#endif

#ifndef OS_LINUX
DLLEXPORT int zcas_detectTextEncoding(const char *data, int32_t len, char **detected);
DLLEXPORT int zcas_convert(const char *toConverterName,
	const char *fromConverterName,  
    char *target, int32_t targetCapacity,
	const char *source,
	int32_t sourceLength);
#endif

/* 打印内存 */
void zcas_printmemory(void *pt,int nsize);

/* 获取时间：微秒数 */
void zcas_gettime(char *s);

/* 判断是否匹配正则表达式 */
int zcas_regmatch(char *buf,char *pattern);

/* 判断字符串是否与特定格式匹配 */
int zcas_stringmatch(char *buf,char *pmatch);

/* 打印级别 */
extern int gprintlevel;
#define DEBUG_PRINT(format,...)                                             \
do {                                                                        \
 if(gprintlevel){						         	\
   char t[32] = {'\0'};							\
   zcas_gettime(t);						        	\
   printf("[DEBUG]"__FILE__",%d(%s) "format"", __LINE__,t,##__VA_ARGS__);   \
 }\
}while (0);

#endif
