#include "test_resources.h"

evt_t  evt0 = EVT_INIT();
evt_id evt1 = EVT_CREATE();
OS_EVT(evt2);

sig_t  sig0 = SIG_INIT(0);
sig_id sig1 = SIG_CREATE();
OS_SIG(sig2);

flg_t  flg0 = FLG_INIT(0);
flg_id flg1 = FLG_CREATE();
OS_FLG(flg2);

bar_t  bar0 = BAR_INIT(1);
bar_id bar1 = BAR_CREATE(1);
OS_BAR(bar2, 1);

sem_t  sem0 = SEM_INIT(0, semDefault);
sem_id sem1 = SEM_CREATE(0);
OS_SEM(sem2, 0);

mut_t  mut0 = MUT_INIT();
mut_id mut1 = MUT_CREATE();
OS_MUT(mut2);

mtx_t  mtx0 = MTX_INIT(mtxDefault, 0);
mtx_id mtx1 = MTX_CREATE(mtxDefault);
OS_MTX(mtx2, mtxDefault);

cnd_t  cnd0 = CND_INIT();
cnd_id cnd1 = CND_CREATE();
OS_CND(cnd2);

lst_t  lst0 = LST_INIT();
lst_id lst1 = LST_CREATE();
OS_LST(lst2);

mem_t  mem0 = MEM_INIT(1, sizeof(unsigned));
mem_id mem1 = MEM_CREATE(1, sizeof(unsigned));
OS_MEM(mem2, 1, sizeof(unsigned));

stm_t  stm0 = STM_INIT(1, sizeof(unsigned));
stm_id stm1 = STM_CREATE(sizeof(unsigned));
OS_STM(stm2, sizeof(unsigned));

msg_t  msg0 = MSG_INIT(1, sizeof(unsigned));
msg_id msg1 = MSG_CREATE(2*sizeof(unsigned));
OS_MSG(msg2, 2*sizeof(unsigned));

box_t  box0 = BOX_INIT(1, sizeof(unsigned));
box_id box1 = BOX_CREATE(1, sizeof(unsigned));
OS_BOX(box2, 1, sizeof(unsigned));

evq_t  evq0 = EVQ_INIT(1);
evq_id evq1 = EVQ_CREATE(1);
OS_EVQ(evq2, 1);

job_t  job0 = JOB_INIT(1);
job_id job1 = JOB_CREATE(1);
OS_JOB(job2, 1);

tmr_t  tmr0 = TMR_INIT(0);
tmr_id tmr1 = TMR_CREATE(0);
OS_TMR(tmr2, NULL);
OS_TMR_DEF(tmr3) {}

tsk_t  tsk0 = TSK_INIT(0, NULL);
tsk_id tsk1 = TSK_CREATE(1, NULL);
OS_TSK(tsk2, 2, NULL);
OS_TSK(tsk3, 3, NULL);
OS_TSK_DEF(tsk4, 4) {}
OS_TSK_DEF(tsk5, 5) {}
