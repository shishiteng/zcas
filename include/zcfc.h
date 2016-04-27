/*
 * zcfc.h
 *
 *  Created on: 2013年8月1日
 *      Author: zczhaoyj
 */

#ifndef ZCFC_H_
#define ZCFC_H_

#include "zcfc_struct.h"

#ifdef __cplusplus
extern "C"
{
#endif
 zcfc_handle *zcfc_init(ZCFCSTR *confoath);
 zcfc_oword *zcfc_run(zcfc_handle *handle,ZCFCSTR *text,int length,int *wnum);
 int zcfc_env_release(zcfc_handle *handle);
 int zcfc_release_words(zcfc_oword *words);
 int zcfc_set_outnumci_mode(zcfc_handle *handle,int value);
 int zcfc_set_givepos_mode(zcfc_handle *handle,int value);
 int zcfc_set_posswitch_mode(zcfc_handle *handle,int value);
 int zcfc_set_outencoding_mode(zcfc_handle *handle,int value);
 int zcfc_set_outpos_mode(zcfc_handle *handle,int value);
 int zcfc_set_wordsegmentation_mode(zcfc_handle *handle,int value);

 #ifdef __cplusplus
}
#endif


#endif /* ZCFC_H_ */
