/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EOSTHESYSTEM_HID_H_
#define _EOSTHESYSTEM_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EONtheSystem_hid.h
    @brief      This header file implements hidden interface to the system singleton.
    @author     marco.accame@iit.it
    @date       04/07/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOVtheSystem.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EONtheSystem.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section



// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EONtheSystem_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EONtheSystem_hid 
{
    // base object
    EOVtheSystem                *thevsys;
    // other stuff
    eOabstime_t                 lifetime;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint64_t eon_sys_hid_tickperiodget(void);

extern uint64_t eon_sys_hid_tickoflifeget(void);



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

