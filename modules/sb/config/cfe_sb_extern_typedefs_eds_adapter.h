/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *
 * Declarations and prototypes for cfe_sb_extern_typedefs module
 */

#ifndef CFE_SB_EXTERN_TYPEDEFS_H
#define CFE_SB_EXTERN_TYPEDEFS_H

#include "common_types.h"
#include "cfe_mission_cfg.h"
#include "cfe_resourceid_typedef.h"

/* Source the definitions from EDS */
#include "cfe_sb_eds_typedefs.h"

/**
 * @brief  CFE_SB_MsgId_Atom_t primitive type definition
 *
 * Due to the way EDS toolchain works, this type name in the
 * generated header does not match.
 */
typedef CFE_SB_MsgIdValue_Atom_t CFE_SB_MsgId_Atom_t;

#endif /* CFE_SB_EXTERN_TYPEDEFS_H */
