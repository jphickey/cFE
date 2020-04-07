/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

#ifndef _CFE_SB_EXTERN_TYPEDEFS_H_
#define _CFE_SB_EXTERN_TYPEDEFS_H_

/* This header may be generated from an EDS file, 
 * tools are available and the feature is enabled */
#ifdef CFE_EDS_ENABLED_BUILD

/* Use the EDS generated version of these types */
#include "cfe_sb_eds_typedefs.h"

#else
/* Use the local definitions of these types */

#include "common_types.h"
#include "cfe_mission_cfg.h"

/**
 * @brief Label definitions associated with CFE_SB_QosPriority_Enum_t
 */
enum CFE_SB_QosPriority
{

   /**
    * @brief Normal priority level
    */
   CFE_SB_QosPriority_LOW                             = 0,

   /**
    * @brief High priority
    */
   CFE_SB_QosPriority_HIGH                            = 1
};

/**
 * @brief Selects the priorty level for message routing
 *
 *
 * @sa enum CFE_SB_QosPriority
 */
typedef uint8                                            CFE_SB_QosPriority_Enum_t;


/**
 * @brief Label definitions associated with CFE_SB_QosReliability_Enum_t
 */
enum CFE_SB_QosReliability
{

   /**
    * @brief Normal (best-effort) reliability
    */
   CFE_SB_QosReliability_LOW                          = 0,

   /**
    * @brief High reliability
    */
   CFE_SB_QosReliability_HIGH                         = 1
};

/**
 * @brief Selects the reliability level for message routing
 *
 *
 * @sa enum CFE_SB_QosReliability
 */
typedef uint8                                            CFE_SB_QosReliability_Enum_t;

/**
 * @brief An integer type that should be used for indexing into the Routing Table
 */
typedef uint16  CFE_SB_MsgRouteIdx_Atom_t;

/**
 * @brief  CFE_SB_MsgId_Atom_t primitive type definition
 *
 * This is an integer type capable of holding any Message ID value
 */
#ifdef MESSAGE_FORMAT_IS_CCSDS_VER_2
typedef uint32 CFE_SB_MsgId_Atom_t;
#else
typedef uint16 CFE_SB_MsgId_Atom_t;
#endif

/**
 * @brief  CFE_SB_MsgId_t type definition
 *
 * Software Bus message identifier used in many SB APIs
 *
 * If opaque MsgId option is disabled, this is directly mapped to the underlying holding type
 * (not wrapped) for compatibility with existing usage semantics in apps (mainly switch/case statements)
 *
 * If opaque MsgId option is enabled, this is a type safe abstract object and must only be accessed
 * through the CFE SB API.  This prevents message IDs getting mixed between other integer values.
 * Code should not use the internal value directly.
 */
#ifdef CFE_SB_OPAQUE_MSGIDS

/* In this mode, the real value lies in an struct field */
typedef struct
{
    CFE_SB_MsgId_Atom_t Value;
} CFE_SB_MsgId_t;

/**
 * \brief Translation macro to convert from MsgId integer values to opaque/abstract API values
 */
#define CFE_SB_MSGID_WRAP_VALUE(val)     { .Value = (CFE_SB_MsgId_Atom_t)(val) }

/**
 * \brief Translation macro to convert to MsgId integer values from opaque/abstract API values
 */
#define CFE_SB_MSGID_UNWRAP_VALUE(mid)   (mid.Value)

#else  /* not CFE_SB_OPAQUE_MSGIDS */

/* In this mode, the value is directly used (backward compatible) */
typedef CFE_SB_MsgId_Atom_t CFE_SB_MsgId_t;

/**
 * \brief Translation macro to convert from MsgId integer values to opaque/abstract API values
 */
#define CFE_SB_MSGID_WRAP_VALUE(val)     ((CFE_SB_MsgId_t)(val))

/**
 * \brief Translation macro to convert to MsgId integer values from opaque/abstract API values
 */
#define CFE_SB_MSGID_UNWRAP_VALUE(mid)   ((CFE_SB_MsgId_Atom_t)(mid))

#endif /* CFE_SB_OPAQUE_MSGIDS */


#endif /* CFE_EDS_ENABLED_BUILD */

#endif /* _CFE_SB_EXTERN_TYPEDEFS_H_ */

