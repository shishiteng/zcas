/*
 * zcfc_struct.h
 *
 *  Created on: 2013年7月22日
 *      Author: zczhaoyj
 */

#define EXPORTDLL __declspec(dllexport)

#ifndef ZCFC_STRUCT_H_
#define ZCFC_STRUCT_H_

#include "uthash.h"

#define ZCFC_MAX_WORD_LENGTH 31
#define ZCFC_MAX_MARK_LENGTH 10
#define ZCFC_MAX_MARK_NUM	10
#define ZCFC_MAX_POS_NUM 10

#define ZCFC_DICT_TEXT_ITEM_LEN 100
#define ZCFC_MARK_TEXT_ITEM_LEN 100
#define ZCFC_POSRULE_TEXT_ITEM_LEN 200

#define ZCFC_DICT_NUM 25

typedef char ZCFCSTR;
#ifdef LINUX
typedef int32_t ZCFCINT32;
#else
typedef int ZCFCINT32;
#endif


typedef enum zcfc_enum{
	zcfc_e_chinese=49,
	zcfc_e_sentence=100,
	zcfc_e_ideographic,
	zcfc_e_mark,
	zcfc_e_lbook,
	zcfc_e_rbook,
	zcfc_e_halt,
	zcfc_e_lquote,
	zcfc_e_rquote,
	zcfc_e_lbracket,
	zcfc_e_rbracket,
	zcfc_e_digital,
	zcfc_e_letter,
	zcfc_e_special,
	zcfc_e_space,
	zcfc_e_utf8,
	zcfc_e_gb2312,
	zcfc_e_gbk,
	zcfc_e_gb18030,
	zcfc_e=-10,
	zcfc_e_unkown=-11,
	zcfc_on=1,
	zcfc_off=0,
	zcfc_utf8=200,
	zcfc_gbk
}zcfc_enum;
typedef enum zcfc_pos_sign{
	N=1024,NP,NPP,NPI,NPD,NR,NRN,NRB,NRM,NRC,NRP,NRL,NA,NG,NRT,
	V=2048,VP,VW,VT,VS,VY,
	ADJ=3072,ADJS,ADJA,ADJC,ADJT,
	ADV=4096,ADVR,ADVT,ADVQ,ADVY,ADVD,ADVL,ADVN,ADVE,ADVO,ADVH,ADVM,
	NUM=5120,NUMB,NUMO,NUMF,NUMM,NUMA,
	CI=6144,CIN,CINI,CING,CINM,CINC,CINF,CINA,CINS,CINN,CINO,CIV,CIT,
	PREP=7168,PRET,PREL,PREI,PREM,PREB,PREO,PRER,PREE,PREJ,PRED,PREA,
	AUXI=8192,AUXS,AUXSM,AUXSF,AUXSD,AUXSS,AUXSI,AUXT,AUXM,AUXSZ,
	CONJ=9216,CONJP,CONJC,CONJR,CONJO,CONJT,CONJL,CONJH,CONJN,CONJJ,CONJS,
	INTE=10240,INTS,INTC,INTA,
	ONO=11264,
	EVENT=10,
	NUMCI=9,
	Z=11,
	x=-1
}zcfc_pos_sign;
typedef enum zcfc_phrase_sign{
	ch,
	en,
	di,
	sc,
	ot
}zcfc_phrase_sign;
//词性
typedef struct zcfc_word_pos{
	zcfc_pos_sign pos_sign;
	float pos_weight;
}zcfc_word_pos;
//词
typedef struct zcfc_word{
	struct zcfc_word *prev;
	struct zcfc_word *next;
	ZCFCSTR word[ZCFC_MAX_WORD_LENGTH];
	zcfc_word_pos pos[ZCFC_MAX_POS_NUM];
	int length;
	int pos_num;
	int word_freq;
	int key;
}zcfc_word;
//输出词
typedef struct zcfc_oword{
	char word[ZCFC_MAX_WORD_LENGTH*2];
	char pos[ZCFC_MAX_MARK_LENGTH];
}zcfc_oword;
//词典
typedef struct zcfc_dict_item{
	ZCFCSTR key[ZCFC_MAX_WORD_LENGTH];
	zcfc_word value;
	UT_hash_handle hh;
}zcfc_dict_item;
//词典体
typedef struct zcfc_dict{
	int key;
	zcfc_dict_item *dict;
	UT_hash_handle hh;
}zcfc_dict;
//词性规则
typedef struct zcfc_pos_rule_item{
	zcfc_pos_sign y_pos;
	zcfc_word_pos p_pos[ZCFC_MAX_POS_NUM];
	int p_pos_num;
	int sign;
}zcfc_pos_rule_item;
//词性规则表
typedef struct zcfc_pos_stat{
	zcfc_pos_sign sign;
	int stat;
}zcfc_pos_stat;
typedef struct zcfc_pos_rule_table{
	zcfc_pos_stat key;
	zcfc_pos_rule_item value;
	UT_hash_handle hh;
}zcfc_pos_rule_table;
//符号
typedef struct zcfc_mark{
	ZCFCSTR text[ZCFC_MAX_MARK_LENGTH];
	zcfc_enum type;
}zcfc_mark;
//符号表
typedef struct zcfc_mark_table{
	ZCFCSTR key[ZCFC_MAX_MARK_LENGTH];
	zcfc_mark value;
	UT_hash_handle hh;
}zcfc_mark_table;
//串单元
typedef struct zcfc_phrase{
	struct zcfc_phrase *prev;
	struct zcfc_phrase *next;
	ZCFCSTR *text;
	int length;
	zcfc_mark lmark[ZCFC_MAX_MARK_NUM];
	zcfc_mark rmark[ZCFC_MAX_MARK_NUM];
	int lmark_num;
	int rmark_num;
	zcfc_phrase_sign sign;
	zcfc_word *words;
	int words_num;
	int key;
}zcfc_phrase;
//doc文件体
typedef struct zcfc_doc{
	struct zcfc_doc *prev;
	struct zcfc_doc *next;
	ZCFCSTR *text;
	int length;
	zcfc_phrase *phrase;
	int phrase_num;
}zcfc_doc;
//配制项
typedef struct zcfc_conf{
	ZCFCSTR *licensepath;
	ZCFCSTR *dictpath;
	ZCFCSTR *other_dict_path;
	ZCFCSTR *dictpath_brand;
	ZCFCSTR *dictpath_media;
	ZCFCSTR *posrulepath;
	ZCFCSTR *markpath;

	int word_segmentation_mode;

	int pos_switch;
	int give_pos_mode;
	int out_pos_mode;

	int brand_dict_switch;
	int media_dict_switch;
	int other_dict_switch;

	int out_encoding;

	int out_numci_mode;

	int main_dict_offset;
	int brand_dict_offset;
	int media_dict_offset;
	int other_dict_offset;
	int mark_table_offset;
	int posrule_table_offset;

	char user_dict_order[20][100];
	int user_dict_num;
	int zcfc_dict_order[20];
	int zcfc_dict_num;
	float license_verify;
}zcfc_conf;
//初始化handle
typedef struct zcfc_handle{
	zcfc_dict *dict;
	zcfc_pos_rule_table *posrule;
	zcfc_mark_table *mark;
	zcfc_conf *conf;
}zcfc_handle;
#endif /* ZCFC_STRUCT_H_ */
