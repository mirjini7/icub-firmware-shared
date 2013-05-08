/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOTHEEMASBACKDOORCFG_H_
#define _EOTHEEMASBACKDOORCFG_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif


/** @file       OPCprotocolManager_Cfg.h
    @brief      This file provides interface to the configuration of OPCprotoclManager shared between pc104 and ems board.
    @author     valentina.gaggero@iit.it
    @date       04/29/2013
**/

/** @defgroup OPCprotocolManager_Cfg
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------
#include "EoCommon.h"
#include "EoMeasures.h"
#include "OPCprotocolManager.h"


// - public #define  --------------------------------------------------------------------------------------------------
#define 	eom_ipnet_hid_DEBUG_id 				1
#define 	eom_emsrunner_hid_DEBUG_id  		2
#define		eom_emstransceiver_hid_DEBUG_id 	3

#define 	opcProt_numofvar_max				3
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 
typedef struct
{
    uint32_t    datagrams_failed_to_go_in_rxfifo;
    uint32_t    datagrams_failed_to_go_in_txosalqueue;
    uint32_t    datagrams_failed_to_be_retrieved_from_txfifo;
    uint32_t    datagrams_failed_to_be_sent_by_ipal;    
} EOMtheIPnetDEBUG_t;


typedef struct
{
    uint64_t    numberofperiods;
    uint64_t    cumulativeabsoluteerrorinperiod;  
    uint32_t    meanofabsoluteerrorinperiod;
    uint32_t    movingmeanofabsoluteerrorinperiod;
    uint32_t    maxabsoluteerrorinperiod;
    uint32_t    minabsoluteerrorinperiod;  
    uint32_t    executionoverflows[3]; //enum { eo_emsrunner_task_numberof   = 3 };
    uint32_t    datagrams_failed_to_go_in_txsocket;    
} EOMtheEMSrunnerDEBUG_t;


typedef struct
{
    uint32_t    rxinvalidropframes;
    uint32_t    errorsinsequencenumber;
    uint32_t    lostreplies;
    uint32_t    failuresinloadofreplyropframe;
    uint32_t    txropframeistoobigforthepacket;
    uint32_t    cannotloadropinregulars;
    uint32_t    cannotloadropinoccasionals;
} EOMtheEMStransceiverDEBUG_t;  



// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
extern opcprotman_cfg_t* OPCprotocolManager_Cfg_getconfig(void);
  

/** @}            
    end of group OPCprotocolManager_Cfg
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif  
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


