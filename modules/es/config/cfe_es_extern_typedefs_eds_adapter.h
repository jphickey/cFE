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
 * Declarations and prototypes for cfe_es_extern_typedefs module
 */

#ifndef CFE_ES_EXTERN_TYPEDEFS_H
#define CFE_ES_EXTERN_TYPEDEFS_H

#include "common_types.h"
#include "cfe_resourceid_typedef.h"
#include "cfe_mission_cfg.h"

/* Source the definitions from EDS */
#include "cfe_es_eds_typedefs.h"

/* Add adapter typedefs and supplemental macros */
#define CFE_ES_PERF_TRIGGER_START  CFE_ES_PerfTrigger_START
#define CFE_ES_PERF_TRIGGER_CENTER CFE_ES_PerfTrigger_CENTER
#define CFE_ES_PERF_TRIGGER_END    CFE_ES_PerfTrigger_END
#define CFE_ES_PERF_MAX_MODES      (1 + CFE_ES_PerfTrigger_END)

/**
 * @brief Memory Offset initializer wrapper
 *
 * A converter macro to use when initializing a CFE_ES_MemOffset_t
 * from an integer value of a different type.
 */
#define CFE_ES_MEMOFFSET_C(x) ((CFE_ES_MemOffset_t)(x))

/**
 * @brief Memory Offset to integer value (size_t) wrapper
 *
 * A converter macro to use when interpreting a CFE_ES_MemOffset_t
 * value as a "size_t" type
 */
#define CFE_ES_MEMOFFSET_TO_SIZET(x) ((size_t)(x))

/**
 * @brief Memory Address initializer wrapper
 *
 * A converter macro to use when initializing a CFE_ES_MemAddress_t
 * from a pointer value of a different type.
 */
#define CFE_ES_MEMADDRESS_C(x) ((CFE_ES_MemAddress_t)((cpuaddr)(x)))

/**
 * @brief Memory Address to pointer wrapper
 *
 * A converter macro to use when interpreting a CFE_ES_MemAddress_t
 * as a pointer value.
 */
#define CFE_ES_MEMADDRESS_TO_PTR(x) ((void *)(cpuaddr)(x))

#endif /* CFE_ES_EXTERN_TYPEDEFS_H */
