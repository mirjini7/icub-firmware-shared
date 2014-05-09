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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "string.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"

#include "EOvector_hid.h" 

#include "EoProtocol.h"
#include "EoProtocolMN.h"
#include "EoProtocolMC.h"
#include "EoProtocolAS.h"
#include "EoProtocolSK.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOnvsetDEVbuilder.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOnvsetDEVbuilder_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    uint8_t         en;
    uint8_t         enmultiplicity;     // ie en is a joint, it tells how many joints there are
    uint8_t         tagsnumber;         // ie en is a joint, it tells how many tags there are in a joint
    uint8_t         filler01;
} ET_holder_t;

typedef struct
{
    uint8_t         ep;
    uint8_t         numofentities;  // if ep is motion control, it tells how many entities there are: 3 (joint, motor, controller) 
    uint16_t        filler02;
    EOvector*       entities; // contains ET_holder_t items
} ep_holder_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_device_initialise(void *p, eObool_t islocal);

static uint16_t s_eoprot_ep2index(void* p, eOnvEP8_t ep);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOnvsetDEVbuilder";


static const eOvoid_fp_uint32_voidp_t ram_init_ep[] =
{
    eoprot_fun_INITIALISE_mn,
    eoprot_fun_INITIALISE_mc,
    eoprot_fun_INITIALISE_as,
    eoprot_fun_INITIALISE_sk
};

extern const uint8_t* eoprot_ep_tags_numberof[];

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


 
extern EOnvsetDEVbuilder* eo_nvsetdevbuilder_New(eOnvBRD_t board)
{
    EOnvsetDEVbuilder *p = NULL;  
    uint8_t i = 0;


    // i get the memory for the object
    p = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOnvsetDEVbuilder), 1);
    
    p->boardnum                 = board;
    //p->theendpoints_numberof    = 0;            // maxendpoints
    p->theepcfgs                = NULL;
    p->theendpoints             = eo_vector_New(sizeof(ep_holder_t), eo_vectorcapacity_dynamic, NULL, 0, NULL, NULL);  // or maxendpoints
    for(i=0; i<eoprot_endpoints_numberof; i++)
    {
        p->ep2indexhashtable[i] = EOK_uint16dummy; 
    }
    
    memset(p->eoprot_mn_entities_numberofeach, 0, sizeof(p->eoprot_mn_entities_numberofeach));
    memset(p->eoprot_mc_entities_numberofeach, 0, sizeof(p->eoprot_mc_entities_numberofeach));
    memset(p->eoprot_as_entities_numberofeach, 0, sizeof(p->eoprot_as_entities_numberofeach));
    memset(p->eoprot_sk_entities_numberofeach, 0, sizeof(p->eoprot_sk_entities_numberofeach));
    p->eoprot_ep_entities_numberofeach[eoprot_endpoint_management]      = NULL;
    p->eoprot_ep_entities_numberofeach[eoprot_endpoint_motioncontrol]   = NULL;
    p->eoprot_ep_entities_numberofeach[eoprot_endpoint_analogsensors]   = NULL;
    p->eoprot_ep_entities_numberofeach[eoprot_endpoint_skin]            = NULL;
    
    p->isvarproxied = NULL;
    
    p->devcfg = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eOnvset_DEVcfg_t), 1);
    
    p->devcfg->boardnum                     = eo_prot_BRDdummy;
    p->devcfg->param                        = NULL;
    p->devcfg->fptr_device_initialise       = NULL;         // bel problema. la funzione necessita di dati interni alla classe. bisognerebbe aggiungere un void*  
    p->devcfg->vectorof_epcfg               = NULL;
    p->devcfg->fptr_ep2indexofepcfg         = NULL;          // bel problema. la funzione necessita di dati interni alla classe. bisognerebbe aggiungere un void*
    
    return(p);
}


extern eOresult_t eo_nvsetdevbuilder_ENDPOINTpushback(EOnvsetDEVbuilder* p, eOnvEP8_t ep) 
{
 	if(NULL == p) 
	{
		return(eores_NOK_nullpointer); 
	}
    
    if(ep >= eoprot_endpoints_numberof)
    {
        return(eores_NOK_generic); 
    }
    
    uint16_t size = eo_vector_Size(p->theendpoints);
    
    if(EOK_uint16dummy != p->ep2indexhashtable[ep])
    {   // already inserted
        return(eores_OK); 
    }
    
    ep_holder_t epholder = {0};
    epholder.ep                 = ep;
    epholder.numofentities      = 0; // or maxentities
    epholder.entities           = eo_vector_New(sizeof(ET_holder_t), eo_vectorcapacity_dynamic, NULL, 0, NULL, NULL);  // or maxentities
    
    
    // i put the ep inside ... in next position. i put its size in the hash table
    p->ep2indexhashtable[ep] = size;
    eo_vector_PushBack(p->theendpoints, &epholder);   
    
    // now i assign the ..
    uint8_t *nn = NULL;
    switch(ep)
    {
        case eoprot_endpoint_management:
        {
            nn = p->eoprot_mn_entities_numberofeach;
        } break;
        case eoprot_endpoint_motioncontrol:
        {
            nn = p->eoprot_mc_entities_numberofeach;
        } break; 
        case eoprot_endpoint_analogsensors:
        {
            nn = p->eoprot_as_entities_numberofeach;
        } break;    
        case eoprot_endpoint_skin:
        {
            nn = p->eoprot_sk_entities_numberofeach;
        } break;  
        default:
        {
            nn = NULL;
        } break;
    }
    
    p->eoprot_ep_entities_numberofeach[ep] = nn;
        
    return(eores_OK);
}

extern eOresult_t eo_nvsetdevbuilder_ENTITYpushback(EOnvsetDEVbuilder* p, eOnvEP8_t ep, eOnvENT_t en, uint8_t howmanyofthisentity) //, uint8_t numberoftagsinentity)
{
 	if(NULL == p) 
	{
		return(eores_NOK_nullpointer); 
	}
    
    if(ep >= eoprot_endpoints_numberof)
    {
        return(eores_NOK_generic); 
    }
    
    if(eores_OK != eo_nvsetdevbuilder_ENDPOINTpushback(p, ep))
    {
        return(eores_NOK_generic); 
    }
    
    uint16_t epindex = p->ep2indexhashtable[ep];
    
    if(EOK_uint16dummy == epindex)
    {   // ep was never inserted.
        //#warning --> i may insert it now .... with: eo_nvsetdevbuilder_ENDPOINTpushback(p, ep); epindex = p->ep2indexhashtable[ep];
        return(eores_NOK_generic); 
//        eo_nvsetdevbuilder_ENDPOINTpushback(p, ep); 
//        epindex = p->ep2indexhashtable[ep];
    }
    
    // ok, i get the epholder an i modify it.
    ep_holder_t* eph = (ep_holder_t*) eo_vector_At(p->theendpoints, epindex);
    
    // i hope that eph is not NULL
    
    if(NULL == eph)
    {
        return(eores_NOK_generic); 
    }
    
    ET_holder_t etholder = {0};
    etholder.en                 = en;
    etholder.enmultiplicity     = howmanyofthisentity;
    etholder.tagsnumber         = eoprot_ep_tags_numberof[ep][en]; // or numberoftagsinentity

    // now i put the ethholde inside the eph

    eo_vector_PushBack(eph->entities, &etholder);
    eph->numofentities =  eo_vector_Size(eph->entities);
    
    // now change the number of entities in the ep. the default is zero.
    p->eoprot_ep_entities_numberofeach[ep][en] = howmanyofthisentity;
 
    return(eores_OK);
    
}

extern eOresult_t eo_nvsetdevbuilder_PROXIEDRULEset(EOnvsetDEVbuilder* p, eObool_fp_uint32_t isvarproxied_fn)
{
 	if(NULL == p) 
	{
		return(eores_NOK_nullpointer); 
	}
    
    p->isvarproxied     = isvarproxied_fn;

    
    return(eores_OK);    
}

extern eOresult_t eo_nvsetdevbuilder_Prepare(EOnvsetDEVbuilder* p)
{
    uint8_t i = 0;
    uint8_t j = 0;
    
 	if(NULL == p) 
	{
		return(eores_NOK_nullpointer); 
	}
    
    if(eo_prot_BRDdummy != p->devcfg->boardnum)
    {
        return(eores_OK);
    }
    
    
    p->devcfg->boardnum                     = p->boardnum;
    p->devcfg->param                        = p;
    p->devcfg->fptr_device_initialise       = s_device_initialise;  
    p->devcfg->vectorof_epcfg               = NULL;
    p->devcfg->fptr_ep2indexofepcfg         = s_eoprot_ep2index;           // bel problema. la funzione necessita di dati interni alla classe. bisognerebbe aggiungere un void*
    
    uint16_t numofeps = eo_vector_Size(p->theendpoints);
    p->theepcfgs = eo_vector_New(sizeof(eOnvset_EPcfg_t), numofeps, NULL, 0, NULL, NULL);
    
    eOnvset_EPcfg_t epcfg = {0};
    
    epcfg.dummy         = 0;
    epcfg.protif        = (eOnvset_protocol_Interface_t*)&eoprot_eonvset_Interface;
    
    for(i=0; i<numofeps; i++)
    {   // i is for endpoint
        ep_holder_t* eph = (ep_holder_t*) eo_vector_At(p->theendpoints, i);
        
        uint8_t numofentities = eph->numofentities;
        uint16_t epram_sizeof = 0;
        for(j=0; j<numofentities; j++)
        {   // j is for entity
            ET_holder_t* ETh = (ET_holder_t*) eo_vector_At(eph->entities, j);
            epram_sizeof += (ETh->enmultiplicity * eoprot_entity_sizeof_get(p->boardnum, eph->ep, ETh->en));
        }       
        
        epcfg.endpoint              = eph->ep;
        epcfg.epram_sizeof          = epram_sizeof;
        epcfg.fptr_ram_initialise   = ram_init_ep[eph->ep];   
        
        // ok, add it in the vector
        eo_vector_PushBack(p->theepcfgs, &epcfg);
        
        
//         // init the protocol ... with the number of entity for the endpoint
//         if(NULL != p->eoprot_ep_entities_numberofeach[eph->ep])
//         {
//             eoprot_config_endpoint_entities(p->boardnum, eph->ep, p->eoprot_ep_entities_numberofeach[eph->ep]);
//         }
               
    }
       
    
    eOnvset_EPcfg_t* data = p->theepcfgs->item_array_data;
    
    p->devcfg->vectorof_epcfg = eo_constvector_New(sizeof(eOnvset_EPcfg_t), numofeps, data);
    
    
    return(eores_OK);
    
}

extern eOnvset_DEVcfg_t* eo_nvsetdevbuilder_DEVcfg_Get(EOnvsetDEVbuilder* p)
{
    eOresult_t res = eores_NOK_generic;
 	if(NULL == p) 
	{
		return(NULL); 
	}
    
    if(eo_prot_BRDdummy == p->devcfg->boardnum)
    {
        res = eo_nvsetdevbuilder_Prepare(p);
        if(eores_OK != res)
        {
            return(NULL);
        }
    }
    
    return(p->devcfg);
   
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_device_initialise(void* param, eObool_t islocal)
{
    EOnvsetDEVbuilder* p = (EOnvsetDEVbuilder*)param;
    
    uint8_t i = 0;
    
    uint16_t numofeps = eo_vector_Size(p->theendpoints);    
    
    // 1. configure protocol for teh given board so that it manages the correct number of entities
    
    for(i=0; i<numofeps; i++)
    {   // i is for endpoint
        ep_holder_t* eph = (ep_holder_t*) eo_vector_At(p->theendpoints, i);

        
        // init the protocol ... with the number of entity for the endpoint
        if(NULL != p->eoprot_ep_entities_numberofeach[eph->ep])
        {
            eoprot_config_endpoint_entities(p->boardnum, eph->ep, p->eoprot_ep_entities_numberofeach[eph->ep]);
        }
               
    }
    
    // 2. if the board is local, then set it
    
    if(eobool_true == islocal)
    {
        eoprot_config_board_local(p->boardnum);
    }    
    
    // 3. if board is local configure proxied variables in this board

    if(eobool_true == islocal)
    {    
        eoprot_config_proxied_variables(p->boardnum, p->isvarproxied);
    }   
        
    
    return(eores_OK);
}

static uint16_t s_eoprot_ep2index(void *param, eOnvEP8_t ep)
{
    EOnvsetDEVbuilder* p = (EOnvsetDEVbuilder*)param;
    uint16_t epindex = p->ep2indexhashtable[ep];
    return(epindex);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------