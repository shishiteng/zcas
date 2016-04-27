#ifndef ZCAS_SENTENCE_H
#define ZCAS_SENTENCE_H
#include "zcas_struct.h"

int zcas_getSentences(zcas_config *config,zcas_part *parts);
int zcas_getPartSents(zcas_config *config,zcas_part *part);
int zcas_getParagSents(zcas_config *config,zcas_parag *para);

zcas_sentence *zcas_getFirstSent(zcas_config *config,const char *txt);
char *zcas_getTextofFirstSent(zcas_config *config,const char *txt);
int zcas_isDecimal(char *chr);
int zcas_isPunctMatched(char *text,char *punct);

int zcas_getWords(zcas_config *config,zcas_part *parts);
int zcas_getPartWords(zcas_config *config,zcas_part *part);
int zcas_getParagWords(zcas_config *config,zcas_parag *parag);
int zcas_getSentWords(zcas_config *config,zcas_sentence *sent);
int zcas_getConjWords(zcas_config *config,zcas_part *parts);

int zcas_Sentlist_addtail(zcas_sentence **sentlist,zcas_sentence sent);
int zcas_Wordlist_addtail(zcas_word **segwordlist,zcas_word segword);

int zcas_Sentlist_clear(zcas_sentence *sents);
int zcas_Wordlist_clear(zcas_word *words);

WORD_ATTR zcas_getAttrib(char v);
char zcas_getChar(WORD_ATTR attr);
int zcas_isKeyWord(WORD_ATTR attr);
int zcas_isKeyWord_char(char v);
int zcas_isKeyWord_str(char *attr);

int zcas_GetPunctPos(zcas_config *configAttr,const char* s);
zcfc_oword* zcas_zcfc_Split(zcfc_handle* uhandle,char* input,int input_size, int* return_count);

#endif/* _ZCAS_SENTENCE_H__ */
