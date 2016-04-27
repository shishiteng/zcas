#ifndef ZCAS_STRUCT_H
#define ZCAS_STRUCT_H
#ifdef OS_LINUX
#include <iconv.h>
#include <unistd.h>
#endif

#include "zcfc_struct.h"

#ifndef OS_LINUX
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif /* DLLEXPORT */


#define access(X,Y) _access((X),(Y))
#pragma warning(disable:4996)
#pragma warning(disable:4251)
#ifndef R_OK
#define R_OK 4
#endif /* R_OK */


#ifdef LINE_SEP
#undef LINE_SEP
#endif
#ifdef TAB4
#undef TAB4
#endif

#ifdef OS_LINUX
#define LINE_SEP "\n"
#else
#define LINE_SEP "\r\n"
#endif

#define TAB4 "    "

#ifdef OS_LINUX
#ifndef BOOL
#define BOOL int
#endif/* BOOL */
#ifndef TRUE
#define TRUE (1)
#define FALSE (0)
#endif/* TRUE */
#endif/* OS_LINUX */

#define GBK_CODE (0)
#define UTF8_CODE GBK_CODE+1

#define NeWeightofA(a,b) (a*(1+8*b))

#ifndef WORD_MAX_LENTH
#define WORD_MAX_LENTH 20
#endif

#ifndef WORD_MAX_LENTH_AC
#define WORD_MAX_LENTH_AC 21
#endif


#define ZCAS_OK 0
#define ZCAS_ERROR -1
#ifdef OS_LINUX
#define ZCAS_TEXT_MAX_LENTH (1*1024*1024)    /* linux下最大文章1M */
#else
#define ZCAS_TEXT_MAX_LENTH (1*1024*200)     /* windows下200kb */
#endif
#define ZCAS_WORD_MAX_LENTH 24


const static unsigned int title_Max_Len = 100;  /* 最大标题长度 windows error */
const static unsigned int pas_Min_Len = 300;    /* 最小文章长度 */
typedef uint32_t COLOR;

/* 跨平台需要考虑字节对齐 */
typedef struct zcas_head{
  int nbytes;/* 每次写入字节数 */
  int size;/* 大小 */
  int version;/* 版本描述 */
  unsigned int baseaddr;/* 基地址 */
  char dictname[50];/* 词典名称 */
  char desc[100];/* 描述*/
  char remain[1024];  /* 预留变化空间 */
}zcas_head;

typedef struct zcas_dicthead{
  int fileflag;/* 文件标记 0xff00+id */
  int nbytes;/* 每一条数据字节数，不要换行符内置 */
  double size;/* 字节数 */
}zcas_dicthead;

//词典头：
enum dictid{DEPART,WEIGHTWD,CONJWORDS};

typedef enum WORD_ATTR
{
  ATTRIB_UNKNOWN=0,/* unknown	0	未知词 */
  ATTRIB_NOUN=1,/* noun	n	名词 N */
  ATTRIB_TIME=2,/* time	t	时间词 */
  ATTRIB_LOCT=3,/* location	s	处所词 */
  ATTRIB_DIRECT=4,/* direction	f	方位词 */
  ATTRIB_VERB=5,/* verb	v	动词 */
  ATTRIB_ADJ=6,/* adjective	a	形容词 */
  ATTRIB_DIST=7,/* distinct	b	区别词 */
  ATTRIB_STAT=8,/* state	z	状态词 */
  ATTRIB_PRON=9,/* pronoun	r	代词 PRON */
  ATTRIB_NUM=10,/* number	m	数词 */
  ATTRIB_QUAN=11,/* quanlity	q	量词 */
  ATTRIB_ADV=12,/* adverb	d	副词 */
  ATTRIB_PREP=13,/* preposition	p	介词 */
  ATTRIB_CONJ=14,/* conjunction	c	连词 */
  ATTRIB_AUX=15,/* auxiliary	u	助词 */
  ATTRIB_EXC=16,/* exclaim	e	叹词 */
  ATTRIB_MOD=17,/* modalparticle	y	语气词 */
  ATTRIB_ONOM=18,/* onomatopoeia	o	拟声词 */
  ATTRIB_PREF=19,/* prefix	h	前缀 */
  ATTRIB_SUFF=20,/* suffix	k	后缀 */
  ATTRIB_ASTR=21,/* alphabeticstring	x	字符串 */
  ATTRIB_PUNC=22,/* punctuation	w	标点 */
}WORD_ATTR;

typedef struct stringList
{
  char *string;
  struct stringList *prev;
  struct stringList *next;
}stringList;

typedef struct doublelist
{
  double value;
  struct doublelist *prev;
  struct doublelist *next;
}doublelist;


typedef struct UTHASH_DOUBLE
{
  double value;
  UT_hash_handle hh;
  char key[ZCAS_WORD_MAX_LENTH];
}UTHASH_DOUBLE;

typedef struct UTHASH_MapDoubleToInt
{
  int value;
  double key;
  UT_hash_handle hh;
}UTHASH_MapDoubleToInt;

typedef zcfc_oword* (*SplitFunc)(zcfc_handle* handle,
				ZCFCSTR* text,
				int *length,
				int* wnum);

typedef struct CONJWORD
{
  char key[ZCAS_WORD_MAX_LENTH];
  double value;
}CONJWORD;

typedef struct conjwordlist
{
  CONJWORD word;
  struct conjwordlist *next; 
}conjwordlist;

typedef struct UTHASH_CONJWORDS
{
  char key[ZCAS_WORD_MAX_LENTH];
  conjwordlist *value;
  UT_hash_handle hh;
    
}UTHASH_CONJWORDS;

typedef enum ARTICLETYPE{
  TYPE_NONE = 0,
  TYPE_NEWS,
  TYPE_NOVEL,
  TYPE_OTHERS
}ARTICLETYPE;

#define ZCAS_PATH_MAX_LENTH 256
typedef struct zcas_config
{
  char zcfc_confdir[ZCAS_PATH_MAX_LENTH];     /* zcfc配置文件路径 */
  char zcas_dicdir[ZCAS_PATH_MAX_LENTH];      /* zcas词典路径 */
  char zcas_license_dir[ZCAS_PATH_MAX_LENTH]; /* zcas的license路径 */
  
  char epuncts[WORD_MAX_LENTH_AC];            /* 英语标点符号 */
  char cpuncts[WORD_MAX_LENTH_AC];            /* 中文标点符号 */
  char erends[WORD_MAX_LENTH_AC];             /* 英语双标点右部分 */
  char crends[WORD_MAX_LENTH_AC];             /* 中文双标点右部分 */

  char version[64];                /* 版本信息 */

  UTHASH_CONJWORDS *conjwords;     /* 关联词词表 */
  UTHASH_CONJWORDS *partconjwords; /* 文章的关联词词表 */
  UTHASH_DOUBLE *weightwords;      /* 加重关键词词表 */
  UTHASH_DOUBLE *freqwords;        /* 词频词表 */
  
  double threld;     /* 输出阈 */
  int threldmode;    /* 0:百分比输出；1：句子数输出 */
  int outcount;      /* 输出句子数 */
  int wordcount;     /* 输出最大字数 */
  double outpercent; /* 输出百分比 */

  char NullChrs[2];          /* 无效字符 */
  zcfc_handle *zcfchandle;   /* zcfc句柄 */

  int debugmode;      /* 调试模式 */
  int thememode;      /* 主题模式 */
  int partbytes;      /* 单个part最大字节数 */
  int paracount;      /* 单个part的段落数 */
  int topicsent;      /* 主题句加权 */
  int bestout;        /* 微调开关 */
  int conjword;       /* 权重加入关联度 */
  int freqword;       /* 词频调整开关 */

  int errcode;        /* 错误码 */
  ARTICLETYPE articletype; /* 文章类型 */
}zcas_config;

typedef struct zcas_word
{
  char text[WORD_MAX_LENTH_AC];
  WORD_ATTR attr;
  double weight;
  double count;/* word出现的次数 */
  int isKey;
  struct zcas_word *prev;
  struct zcas_word *next;
} zcas_word;

typedef struct zcas_sentence
{
	char *text;
	zcas_word *words;
	double mold;
	double weight;
	int lenth;
    int isfilt;
	struct zcas_sentence *prev;
	struct zcas_sentence *next;
} zcas_sentence;

typedef struct zcas_parag
{
  char *text;
  double mold;
  int lenth;
  zcas_sentence *sents;
  zcas_word *keywords; /* 段落中的关键词 */
  struct zcas_parag *prev;
  struct zcas_parag *next;
} zcas_parag;


typedef struct zcas_part
{
  char title[100];
  char *text;
  zcas_parag *parags;
  UTHASH_DOUBLE *hashwords; /* part中的关键词 */
  double threshold;/* 阈 */
  struct zcas_part *prev;
  struct zcas_part *next;
} zcas_part;

typedef struct zcas_doc
{
  zcas_part *parts;
  zcas_word *keywords; /* 整篇文章的关键词 */
  char *text;
  char *title;
} zcas_doc;

typedef enum LOG_LEVEL{
  LOG_ERR,
  LOG_WARN,
  LOG_INFO,
  LOG_DEBUG
}LOG_LEVEL;

/* 定义模块编号 */
#define MODULE_INIT     100
#define MODULE_SUMMARY  200
#define MODULE_RELEASE  300

#endif/* ZCAS_STRUCT_H__ */
