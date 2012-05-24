/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "string.h"
#include "EoCommon.h"

#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"


#include "EOMtheEMStransceiver.h"
#include "EOMtheEMSsocket.h"
#include "EOMtask.h"

#include "hal_timer.h"
#include "osal_task.h"

#include "EOMtask_hid.h" // to retrieve its osaltask pointer

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSrunner.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSrunner_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemsrunner_cfg_t eom_emsrunner_DefaultCfg = 
{
    EO_INIT(.taskpriority)              {62,     61,     60},  
    EO_INIT(.taskstacksize)             {1024,   1024,   1024},
    EO_INIT(.execDOafter)               500, 
    EO_INIT(.execTXafter)               750, 
    EO_INIT(.safetyGAP)                 50
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

extern void tskEMSrunRX(void *p);
extern void tskEMSrunDO(void *p);
extern void tskEMSrunTX(void *p);

static void s_eom_emsrunner_taskRX_startup(EOMtask *p, uint32_t t);
static void s_eom_emsrunner_taskRX_run(EOMtask *p, uint32_t t);

static void s_eom_emsrunner_taskDO_startup(EOMtask *p, uint32_t t);
static void s_eom_emsrunner_taskDO_run(EOMtask *p, uint32_t t);

static void s_eom_emsrunner_taskTX_startup(EOMtask *p, uint32_t t);
static void s_eom_emsrunner_taskTX_run(EOMtask *p, uint32_t t);


static void s_eom_emsrunner_enable_task(EOMtask *tsk);

static void s_eom_emsrunner_start_cycle(void *arg);

static void s_eom_emsrunner_warn_task(void *arg);

static void s_eom_emsrunner_start_taskDO(void *arg);

static void s_eom_emsrunner_start_taskTX(void *arg);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOMtheEMSrunner";

 
static EOMtheEMSrunner s_theemsrunner = 
{
    EO_INIT(.task)              {NULL, NULL, NULL},
    EO_INIT(.cfg)               {0},
    EO_INIT(.cycleisrunning)    eobool_false
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMtheEMSrunner * eom_emsrunner_Initialise(const eOemsrunner_cfg_t *cfg)
{
    if(NULL != s_theemsrunner.task[0])
    {
        return(&s_theemsrunner);
    }
    
    if(NULL == cfg)
    {
        cfg = &eom_emsrunner_DefaultCfg;
    }
    
    
    // verify that the cfg has timing that are compatible ... d2>d1+gap, period > d1+d2+gap
    
    memcpy(&s_theemsrunner.cfg, cfg, sizeof(eOemsrunner_cfg_t));
    
    s_theemsrunner.cycleisrunning = eobool_false; 
    
    s_theemsrunner.task[eo_emsrunner_taskid_runRX] = eom_task_New(eom_mtask_OnAllEventsDriven, 
                                                                  cfg->taskpriority[eo_emsrunner_taskid_runRX], 
                                                                  cfg->taskstacksize[eo_emsrunner_taskid_runRX], 
                                                                  s_eom_emsrunner_taskRX_startup, s_eom_emsrunner_taskRX_run,  
                                                                  (eOevent_t)(eo_emsrunner_evt_enable) & (eOevent_t)(eo_emsrunner_evt_execute), 
                                                                  eok_reltimeINFINITE, NULL, 
                                                                  tskEMSrunRX, "tskEMSrunRX");
 
    s_theemsrunner.task[eo_emsrunner_taskid_runDO] = eom_task_New(eom_mtask_OnAllEventsDriven, 
                                                                  cfg->taskpriority[eo_emsrunner_taskid_runDO], 
                                                                  cfg->taskstacksize[eo_emsrunner_taskid_runDO], 
                                                                  s_eom_emsrunner_taskDO_startup, s_eom_emsrunner_taskDO_run,  
                                                                  (eOevent_t)(eo_emsrunner_evt_enable) & (eOevent_t)(eo_emsrunner_evt_execute), 
                                                                  eok_reltimeINFINITE, NULL, 
                                                                  tskEMSrunDO, "tskEMSrunDO"); 
                                                                  
    s_theemsrunner.task[eo_emsrunner_taskid_runTX] = eom_task_New(eom_mtask_OnAllEventsDriven, 
                                                                  cfg->taskpriority[eo_emsrunner_taskid_runTX], 
                                                                  cfg->taskstacksize[eo_emsrunner_taskid_runTX], 
                                                                  s_eom_emsrunner_taskTX_startup, s_eom_emsrunner_taskTX_run,  
                                                                  (eOevent_t)(eo_emsrunner_evt_enable) & (eOevent_t)(eo_emsrunner_evt_execute), 
                                                                  eok_reltimeINFINITE, NULL, 
                                                                  tskEMSrunTX, "tskEMSrunTX");                                                              
                                                   
    
    return(&s_theemsrunner);
}


extern EOMtheEMSrunner* eom_emsrunner_GetHandle(void) 
{
    if(NULL != s_theemsrunner.task)
    {
        return(&s_theemsrunner);
    }
    else
    {
        return(NULL);
    }
}

extern EOMtask * eom_emsrunner_GetTask(EOMtheEMSrunner *p, eOemsrunner_taskid_t id)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(s_theemsrunner.task[id]);
}


extern eOresult_t eom_emsrunner_Start(EOMtheEMSrunner *p)
{
    hal_result_t res;
    
    // instantiate a periodic hal timer which sends an evt eo_emsrunner_evt_execute every period and start a oneshot hal timer to ...
    // also, send an evt eo_emsrunner_evt_enable straigth away.
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }  

    hal_timer_cfg_t t2per_cfg  = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 0,
        .priority           = hal_int_priority02,
        .mode               = hal_timer_mode_periodic,
        .callback_on_exp    = NULL,
        .arg                = NULL
    };  

    t2per_cfg.countdown         = p->cfg.period;
    t2per_cfg.callback_on_exp   = s_eom_emsrunner_start_cycle;

    // after a period time, the first task shall be executed
    res = hal_timer_init(hal_timer2, &t2per_cfg, NULL);  
    hal_timer_start(hal_timer2);

    // but now we need to enable the first task: taskRX. the other enables shall be send at teh end of the relevant tasks
    s_eom_emsrunner_enable_task(s_theemsrunner.task[eo_emsrunner_taskid_runRX]);
    
    s_theemsrunner.cycleisrunning = eobool_true;
    
    return(eores_OK);
}

extern eOresult_t eom_emsrunner_Stop(EOMtheEMSrunner *p)
{
    // stop the hal timers.

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    } 

    // simply stop timer2 ... that allows to finish the cycle without restarting it.
   
    hal_timer_stop(hal_timer2);
    
    s_theemsrunner.cycleisrunning = eobool_false;
    
    return(eores_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void tskEMSrunRX(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 

extern void tskEMSrunDO(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 

extern void tskEMSrunTX(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
} 




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eom_emsrunner_taskRX_startup(EOMtask *p, uint32_t t)
{
    
}



static void s_eom_emsrunner_taskRX_run(EOMtask *p, uint32_t t)
{
    // do things .... only when both eo_emsrunner_evt_enable and a eo_emsrunner_evt_execute are received.



    // at the end enable next in the chain by sending to it a eo_emsrunner_evt_enable
    s_eom_emsrunner_enable_task(s_theemsrunner.task[eo_emsrunner_taskid_runDO]);
}


static void s_eom_emsrunner_taskDO_startup(EOMtask *p, uint32_t t)
{
    
    
}



static void s_eom_emsrunner_taskDO_run(EOMtask *p, uint32_t t)
{
    // do things .... only when both eo_emsrunner_evt_enable and a eo_emsrunner_evt_execute are received.



    // at the end enable next in the chain by sending to it a eo_emsrunner_evt_enable
    s_eom_emsrunner_enable_task(s_theemsrunner.task[eo_emsrunner_taskid_runTX]);
}


static void s_eom_emsrunner_taskTX_startup(EOMtask *p, uint32_t t)
{
    
}



static void s_eom_emsrunner_taskTX_run(EOMtask *p, uint32_t t)
{
    // do things .... only when both eo_emsrunner_evt_enable and a eo_emsrunner_evt_execute are received.



    // at the end enable next in the chain by sending to it a eo_emsrunner_evt_enable
    if(eobool_true == s_theemsrunner.cycleisrunning)
    {
        s_eom_emsrunner_enable_task(s_theemsrunner.task[eo_emsrunner_taskid_runRX]);
    }
    else
    {
        #warning ---> put in here the evolution of the state machine .... 
    }
}

static void s_eom_emsrunner_enable_task(EOMtask *tsk)
{
    // send an event to enable tsk. 
    eom_task_SetEvent(tsk, eo_emsrunner_evt_enable);
}

static void s_eom_emsrunner_start_cycle(void *arg)
{
    hal_timer_cfg_t toneshot_cfg  = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 0,
        .priority           = hal_int_priority02,
        .mode               = hal_timer_mode_oneshot,
        .callback_on_exp    = NULL,
        .arg                = NULL
    };  

    // one-shot timer which verify if taskRX has finished. if not ... it issues a warning.
    toneshot_cfg.countdown          = s_theemsrunner.cfg.execDOafter - s_theemsrunner.cfg.safetyGAP;
    toneshot_cfg.callback_on_exp    = s_eom_emsrunner_warn_task;
    toneshot_cfg.arg                = s_theemsrunner.task[eo_emsrunner_taskid_runRX];
    hal_timer_init(hal_timer3, &toneshot_cfg, NULL); 
    
    // one-shot timer which starts taskDO.
    toneshot_cfg.countdown          = s_theemsrunner.cfg.execDOafter;
    toneshot_cfg.callback_on_exp    = s_eom_emsrunner_start_taskDO;
    toneshot_cfg.arg                = s_theemsrunner.task[eo_emsrunner_taskid_runDO];
    hal_timer_init(hal_timer4, &toneshot_cfg, NULL); 
    
    hal_timer_start(hal_timer3);
    hal_timer_start(hal_timer4);
 
    // send an event to execute taskRX
    eom_task_isrSetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runRX], eo_emsrunner_evt_execute);
}

static void s_eom_emsrunner_warn_task(void *arg)
{
    static uint32_t count = 0;
    EOMtask* task = (EOMtask*) arg;
    osal_task_t *scheduledosaltask = osal_task_get(osal_callerISR);
        
    if(task->osaltask == scheduledosaltask)
    {
        count++;
        count = count;
        #warning ---> if execution is in here it means that the task has not finished its computing in the time it was given to it minus the guard    
        // we could ... send an event to the task to force a graceful finish ... issue a warning ... issue a fatal error ... whatever else.
        // un attimo ... potrebbe essere che il task interrotto sia uno dei task di ipnet ... in tal caso andrebbe bene?
        // oppure 
    }

}


static void s_eom_emsrunner_start_taskDO(void *arg)
{
    hal_timer_cfg_t toneshot_cfg  = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 0,
        .priority           = hal_int_priority02,
        .mode               = hal_timer_mode_oneshot,
        .callback_on_exp    = NULL,
        .arg                = NULL
    };  
    EOMtask* task = (EOMtask*) arg;
    EOMtask* prevtask = s_theemsrunner.task[eo_emsrunner_taskid_runRX];
    osal_task_t *scheduledosaltask = osal_task_get(osal_callerISR);
    
    // che controlli si potrebbe fare qui per evitare che ci siano problemi? ad esempio che lo scheduled task non sia quello prima di task?
    
    if(prevtask->osaltask == scheduledosaltask)
    {
        #warning --> ahi ahi ahi problemi.
    }
    
    // one-shot timer which verify if taskDO has finished. if not ... it issues a warning.
    toneshot_cfg.countdown          = s_theemsrunner.cfg.execTXafter - s_theemsrunner.cfg.execDOafter - s_theemsrunner.cfg.safetyGAP;
    toneshot_cfg.callback_on_exp    = s_eom_emsrunner_warn_task;
    toneshot_cfg.arg                = s_theemsrunner.task[eo_emsrunner_taskid_runDO];
    hal_timer_init(hal_timer3, &toneshot_cfg, NULL); 
    
    // one-shot timer which starts taskTX.
    toneshot_cfg.countdown          = s_theemsrunner.cfg.execTXafter - s_theemsrunner.cfg.execDOafter;
    toneshot_cfg.callback_on_exp    = s_eom_emsrunner_start_taskTX;
    toneshot_cfg.arg                = s_theemsrunner.task[eo_emsrunner_taskid_runTX];
    hal_timer_init(hal_timer4, &toneshot_cfg, NULL); 

    hal_timer_start(hal_timer3);
    hal_timer_start(hal_timer4);
    
    // send event to activate the task in argument (taskDO)
    //eom_task_isrSetEvent(task, eo_emsrunner_evt_execute);
    eom_task_isrSetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runDO], eo_emsrunner_evt_execute);
}


static void s_eom_emsrunner_start_taskTX(void *arg)
{
    hal_timer_cfg_t toneshot_cfg  = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 0,
        .priority           = hal_int_priority02,
        .mode               = hal_timer_mode_oneshot,
        .callback_on_exp    = NULL,
        .arg                = NULL
    };  
    EOMtask* task = (EOMtask*) arg;
    EOMtask* prevtask = s_theemsrunner.task[eo_emsrunner_taskid_runDO];
    osal_task_t *scheduledosaltask = osal_task_get(osal_callerISR);
    
    // che controlli si potrebbe fare qui per evitare che ci siano problemi? ad esempio che lo scheduled task non sia quello prima di task?
    
    if(prevtask->osaltask == scheduledosaltask)
    {
        #warning --> ahi ahi ahi altri problemi.
    }
    
    // one-shot timer which verify if taskTX has finished. if not ... it issues a warning.
    toneshot_cfg.countdown          = s_theemsrunner.cfg.period - s_theemsrunner.cfg.execTXafter - s_theemsrunner.cfg.safetyGAP;
    toneshot_cfg.callback_on_exp    = s_eom_emsrunner_warn_task;
    toneshot_cfg.arg                = s_theemsrunner.task[eo_emsrunner_taskid_runDO];
    hal_timer_init(hal_timer3, &toneshot_cfg, NULL); 
    
//    // one-shot timer which starts taskTX.
//    toneshot_cfg.countdown          = s_theemsrunner.cfg.period - s_theemsrunner.cfg.execTXafter;
//    toneshot_cfg.callback_on_exp    = s_eom_emsrunner_start_taskRX;
//    toneshot_cfg.arg                = s_theemsrunner.task[eo_emsrunner_taskid_runRX];
//    hal_timer_init(hal_timer4, &toneshot_cfg, NULL); 

    hal_timer_start(hal_timer3);
//    hal_timer_start(hal_timer4);
    
    // send event to activate the task in argument (taskTX)
    //eom_task_isrSetEvent(task, eo_emsrunner_evt_execute);
    eom_task_isrSetEvent(s_theemsrunner.task[eo_emsrunner_taskid_runTX], eo_emsrunner_evt_execute);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




