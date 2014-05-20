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

/* @file       EoProtocolAS_fun.c
    @brief      This file keeps constant configuration for ...
    @author     marco.accame@iit.it
    @date       06/06/2013
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolAS.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#if !defined(EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME)
    #define EOPROT_EP_AS_FUN_OVERRIDE

    #if defined(EOPROT_EP_AS_FUN_OVERRIDE)
    #include "EoProtocolAS_overridden_fun.h"
    #endif
#endif



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

#if     defined(EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME)
eOvoid_fp_uint32_voidp_t eoprot_fun_INITIALISE_as_fptr = NULL; 
#endif




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

#if     defined(EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME)
extern void eoprot_fun_INITIALISE_as(eOprotIP_t ip, void *ram) 
{
    if(NULL != eoprot_fun_INITIALISE_as_fptr)
    {
        eoprot_fun_INITIALISE_as_fptr(ip, ram);
    }
}
#else
#if !defined(OVERRIDE_eoprot_as_fun_INITIALISE)
EO_weak extern void eoprot_fun_INITIALISE_as(eOprotIP_t ip, void *ram) {}
#endif
#endif


#if     !defined(EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME)
// if not defined, then we remove them

// -- strain 

#if !defined(OVERRIDE_eoprot_fun_INIT_as_strain_wholeitem)
EO_weak extern void eoprot_fun_INIT_as_strain_wholeitem(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_as_strain_wholeitem)
EO_weak extern void eoprot_fun_UPDT_as_strain_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_as_strain_config)
EO_weak extern void eoprot_fun_INIT_as_strain_config(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_as_strain_config)
EO_weak extern void eoprot_fun_UPDT_as_strain_config(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_as_strain_status)
EO_weak extern void eoprot_fun_INIT_as_strain_status(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_as_strain_status)
EO_weak extern void eoprot_fun_UPDT_as_strain_status(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_as_strain_status_fullscale)
EO_weak extern void eoprot_fun_INIT_as_strain_status_fullscale(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_as_strain_status_fullscale)
EO_weak extern void eoprot_fun_UPDT_as_strain_status_fullscale(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_as_strain_status_calibratedvalues)
EO_weak extern void eoprot_fun_INIT_as_strain_status_calibratedvalues(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_as_strain_status_calibratedvalues)
EO_weak extern void eoprot_fun_UPDT_as_strain_status_calibratedvalues(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_as_strain_status_uncalibratedvalues)
EO_weak extern void eoprot_fun_INIT_as_strain_status_uncalibratedvalues(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_as_strain_status_uncalibratedvalues)
EO_weak extern void eoprot_fun_UPDT_as_strain_status_uncalibratedvalues(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


// -- mais

#if !defined(OVERRIDE_eoprot_fun_INIT_as_mais_wholeitem)
EO_weak extern void eoprot_fun_INIT_as_mais_wholeitem(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_as_mais_wholeitem)
EO_weak extern void eoprot_fun_UPDT_as_mais_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_as_mais_config)
EO_weak extern void eoprot_fun_INIT_as_mais_config(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_as_mais_config)
EO_weak extern void eoprot_fun_UPDT_as_mais_config(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_as_mais_config_mode)
EO_weak extern void eoprot_fun_INIT_as_mais_config_mode(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_as_mais_config_mode)
EO_weak extern void eoprot_fun_UPDT_as_mais_config_mode(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_as_mais_config_datarate)
EO_weak extern void eoprot_fun_INIT_as_mais_config_datarate(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_as_mais_config_datarate)
EO_weak extern void eoprot_fun_UPDT_as_mais_config_datarate(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_as_mais_config_resolution)
EO_weak extern void eoprot_fun_INIT_as_mais_config_resolution(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_as_mais_config_resolution)
EO_weak extern void eoprot_fun_UPDT_as_mais_config_resolution(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_as_mais_status)
EO_weak extern void eoprot_fun_INIT_as_mais_status(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_as_mais_status)
EO_weak extern void eoprot_fun_UPDT_as_mais_status(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_as_mais_status_the15values)
EO_weak extern void eoprot_fun_INIT_as_mais_status_the15values(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_as_mais_status_the15values)
EO_weak extern void eoprot_fun_UPDT_as_mais_status_the15values(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


// -- extorque

#if !defined(OVERRIDE_eoprot_fun_INIT_as_extorque_wholeitem)
EO_weak extern void eoprot_fun_INIT_as_extorque_wholeitem(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_as_extorque_wholeitem)
EO_weak extern void eoprot_fun_UPDT_as_extorque_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_as_extorque_config)
EO_weak extern void eoprot_fun_INIT_as_extorque_config(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_as_extorque_config)
EO_weak extern void eoprot_fun_UPDT_as_extorque_config(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_fun_INIT_as_extorque_inputs)
EO_weak extern void eoprot_fun_INIT_as_extorque_inputs(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_fun_UPDT_as_extorque_inputs)
EO_weak extern void eoprot_fun_UPDT_as_extorque_inputs(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#endif//!defined(EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME)

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

