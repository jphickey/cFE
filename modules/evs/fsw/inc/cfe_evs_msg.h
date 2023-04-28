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
 *  Title:    Event Services Message definition header file Header File
 *
 *  Purpose:
 *	           Unit specification for Event services command codes and data structures.
 *
 *  Design Notes:
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *
 */

#ifndef CFE_EVS_MSG_H
#define CFE_EVS_MSG_H

/********************************** Include Files  ************************************/
#include "common_types.h"            /* Basic data types */
#include "cfe_msg_hdr.h"             /* for header definitions */
#include "cfe_evs_extern_typedefs.h" /* for EVS-specific types such as CFE_EVS_LogMode_Enum_t */
#include "cfe_es_extern_typedefs.h"  /* for CFE_ES_AppId_t type */

/*
 * EDS-defined function codes (*_CC)
 */
#include "cfe_evs_eds_cc.h"

#endif /* CFE_EVS_MSG_H */
