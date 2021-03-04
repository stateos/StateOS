#include <os.h>

#pragma once

extern spn_t  spn0;
extern spn_id spn1;
extern spn_id spn2;

extern one_t  one0;
extern one_id one1;
extern one_id one2;

extern evt_t  evt0;
extern evt_id evt1;
extern evt_id evt2;

extern sig_t  sig0;
extern sig_id sig1;
extern sig_id sig2;

extern flg_t  flg0;
extern flg_id flg1;
extern flg_id flg2;

extern bar_t  bar0;
extern bar_id bar1;
extern bar_id bar2;

extern sem_t  sem0;
extern sem_id sem1;
extern sem_id sem2;

extern mut_t  mut0;
extern mut_id mut1;
extern mut_id mut2;

extern mtx_t  mtx0;
extern mtx_id mtx1;
extern mtx_id mtx2;

extern cnd_t  cnd0;
extern cnd_id cnd1;
extern cnd_id cnd2;

extern lst_t  lst0;
extern lst_id lst1;
extern lst_id lst2;

extern mem_t  mem0;
extern mem_id mem1;
extern mem_id mem2;

extern stm_t  stm0;
extern stm_id stm1;
extern stm_id stm2;

extern msg_t  msg0;
extern msg_id msg1;
extern msg_id msg2;

extern box_t  box0;
extern box_id box1;
extern box_id box2;

extern evq_t  evq0;
extern evq_id evq1;
extern evq_id evq2;

extern job_t  job0;
extern job_id job1;
extern job_id job2;

extern tmr_t  tmr0;
extern tmr_id tmr1;
extern tmr_id tmr2;

extern tsk_t  tsk0;
extern tsk_id tsk1;
extern tsk_id tsk2;
extern tsk_id tsk3;
extern tsk_id tsk4;
extern tsk_id tsk5;

#ifdef  __cplusplus

extern stateos::Task        Tsk0;
extern stateos::Task        Tsk1;
extern stateos::TaskT<512>  Tsk2;
extern stateos::TaskT<1024> Tsk3;
extern stateos::Task        Tsk4;
extern stateos::Task        Tsk5;

#endif
