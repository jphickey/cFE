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

/*
**  File: cfe_es.h
**
**  Purpose:
**	Unit specification for Executive Services library functions and macros.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**	Notes:
**
*/

#ifndef _cfe_es_
#define _cfe_es_

/*
** Includes 
*/
#include "cfe_es_extern_typedefs.h"
#include "cfe_mission_cfg.h"
#include "cfe_perfids.h"
#include "cfe_error.h"

/*****************************************************************************/

/*
** Macro Definitions
*/

/* 
** The OS_PRINTF macro may be defined by OSAL to enable
** printf-style argument checking.  If using a version of OSAL
** that does not define this then define it as a no-op.
*/
#ifndef OS_PRINTF
#define OS_PRINTF(m,n)
#endif

#define CFE_ES_DBIT(x) (1L << (x))                                    /* Places a one at bit positions 0 thru 31 */
#define CFE_ES_DTEST(i,x) (((i) & CFE_ES_DBIT(x)) != 0)               /* true iff bit x of i is set */
#define CFE_ES_TEST_LONG_MASK(m,s)  (CFE_ES_DTEST(m[(s)/32],(s)%32))  /* Test a bit within an array of 32-bit integers. */

#define CFE_ES_DEFAULT_MEMPOOL_BLOCK_SIZES     17    /**< Default number of size divisions in a memory pool */

/*
** Note about reset type and subtypes:
**
** These values come from the PSP so the actual definition of these enumerations
** was moved to the PSP header file <cfe_psp.h>.
**
** In the future the Electronic Data sheets (EDS) for PSP/ES
** will define the exact values to use in telemetry messages.
*/

/*
**  Reset types
*/
/** \name Reset Type extensions */
/** \{ */
#define CFE_ES_APP_RESTART       CFE_PSP_RST_TYPE_MAX       /**< Application only was reset (extend the PSP enumeration here) */
/** \} */


/** \name Critical Data Store Macros */
/** \{ */
/** Maximum length allowed for CDS name. <BR>
** NOTE: "+2" is for NULL Character and "." (i.e. - "AppName.CDSName") */
#define CFE_ES_CDS_MAX_FULL_NAME_LEN (CFE_MISSION_ES_CDS_MAX_NAME_LENGTH + CFE_MISSION_MAX_API_LEN + 2)

#define CFE_ES_CDS_BAD_HANDLE  CFE_ES_RESOURCEID_UNDEFINED
/** \} */

#define CFE_ES_NO_MUTEX                 0 /**< \brief Indicates that the memory pool selection will not use a semaphore */
#define CFE_ES_USE_MUTEX                1 /**< \brief Indicates that the memory pool selection will use a semaphore */


/*****************************************************************************/
/*
** Type Definitions
*/

/**
 * @brief A type that provides a common, abstract identifier for
 * all ES managed resources (e.g. apps, tasks, counters, etc).
 *
 * Fundamentally an unsigned integer but users should treat it as
 * opaque, and only go through the ES API for introspection.
 *
 * Simple operations are provided as inline functions, which
 * should alleviate the need to do direct manipulation of the value:
 *
 *  - Check for undefined ID value
 *  - Check for equality of two ID values
 *  - Convert ID to simple integer (typically for printing/logging)
 *  - Convert simple integer to ID (inverse of above)
 */
typedef uint32 CFE_ES_ResourceID_t;

/**
 * \brief Memory Handle type
 *
 *  Data type used to hold Handles of Memory Pools
 *  created via CFE_ES_PoolCreate and CFE_ES_PoolCreateNoSem
 */
typedef CFE_ES_ResourceID_t CFE_ES_MemHandle_t;

/**
 * Type used for memory pool offsets
 *
 * For backward compatibility with existing CFE code this can be uint32,
 * but pools will be limited to 4GB in size as a result.
 *
 * On 64-bit platforms this can be a 64-bit value (e.g. size_t) which should
 * allow larger pools.
 *
 * In either case this _must_ be an unsigned type.
 */
typedef uint32 CFE_ES_MemOffset_t;


/**
 * @brief A resource ID value that represents an undefined/unused resource
 *
 * This constant may be used to initialize local variables of the
 * CFE_ES_ResourceID_t type to a safe value that will not alias a valid ID.
 *
 * By design, this value is also the result of zeroing a CFE_ES_ResourceID_t
 * type via standard functions like memset(), such that objects initialzed
 * using this method will also be set to safe values.
 */
#define CFE_ES_RESOURCEID_UNDEFINED     ((CFE_ES_ResourceID_t)0)

/**
 * @brief A resource ID value that represents a reserved entry
 *
 * This is not a valid value for any resource type, but is used to mark
 * table entries that are not available for use.  For instance, this may
 * be used while setting up an entry initially.
 */
#define CFE_ES_RESOURCEID_RESERVED      ((CFE_ES_ResourceID_t)0xFFFFFFFF)

/**
 * @brief Convert a resource ID to an integer.
 *
 * This is primarily intended for logging purposes, such was writing
 * to debug console, event messages, or log files, using printf-like APIs.
 *
 * For compatibility with C library APIs, this returns an "unsigned long"
 * type and should be used with the "%lx" format specifier in a printf
 * format string.
 *
 * @note No assumptions should be made about the actual integer value,
 * such as its base/range.  It may be printed, but should not be modified
 * or tested/compared using other arithmetic ops, and should never be used
 * as the index to an array or table.  See the related function
 * CFE_ES_ResourceID_ToIndex() for cases where a zero-based array/table index
 * is needed.
 *
 * @sa CFE_ES_ResourceID_FromInteger()
 *
 * @param[in]   id    Resource ID to convert
 * @returns Integer value corresponding to ID
 */
static inline unsigned long CFE_ES_ResourceID_ToInteger(CFE_ES_ResourceID_t id)
{
    return ((unsigned long)id);
}

/**
 * @brief Convert an integer to a resource ID.
 *
 * This is the inverse of CFE_ES_ResourceID_ToInteger(), and reconstitutes
 * the original CFE_ES_ResourceID_t value from the integer representation.
 *
 * This may be used, for instance, where an ID value is parsed from a text
 * file or message using C library APIs such as scanf() or strtoul().
 *
 * @sa CFE_ES_ResourceID_ToInteger()
 *
 * @param[in]   Value    Integer value to convert
 * @returns ID value corresponding to integer
 */
static inline CFE_ES_ResourceID_t CFE_ES_ResourceID_FromInteger(unsigned long Value)
{
    return ((CFE_ES_ResourceID_t)Value);
}

/**
 * @brief Compare two Resource ID values for equality
 *
 * @param[in]   id1    Resource ID to check
 * @param[in]   id2    Resource ID to check
 * @returns true if id1 and id2 are equal, false otherwise.
 */
static inline bool CFE_ES_ResourceID_Equal(CFE_ES_ResourceID_t id1, CFE_ES_ResourceID_t id2)
{
    return (id1 == id2);
}

/**
 * @brief Check if a resource ID value is defined
 *
 * The constant #CFE_ES_RESOURCEID_UNDEFINED represents an undefined ID value,
 * such that the expression:
 *
 *      CFE_ES_ResourceID_IsDefined(CFE_ES_RESOURCEID_UNDEFINED)
 *
 * Always returns false.
 *
 * @param[in]   id    Resource ID to check
 * @returns True if the ID may refer to a defined entity, false if invalid/undefined.
 */
static inline bool CFE_ES_ResourceID_IsDefined(CFE_ES_ResourceID_t id)
{
    return (id != 0);
}

/**
 * @brief Obtain an index value correlating to an ES Application ID
 *
 * This calculates a zero based integer value that may be used for indexing
 * into a local resource table/array.
 *
 * Index values are only guaranteed to be unique for resources of the same
 * type.  For instance, the indices corresponding to two [valid] application
 * IDs will never overlap, but the index of an application and a library ID
 * may be the same.  Furthermore, indices may be reused if a resource is
 * deleted and re-created.
 *
 * @note There is no inverse of this function - indices cannot be converted
 * back to the original AppID value.  The caller should retain the original ID
 * for future use.
 *
 * @param[in]   AppID  Application ID to convert
 * @param[out]  Idx    Buffer where the calculated index will be stored
 *
 * @return Execution status, see @ref CFEReturnCodes
 * @retval #CFE_SUCCESS                 @copybrief CFE_SUCCESS
 */
CFE_Status_t CFE_ES_AppID_ToIndex(CFE_ES_ResourceID_t AppID, uint32 *Idx);

/**
 * @brief Obtain an index value correlating to an ES Library ID
 *
 * This calculates a zero based integer value that may be used for indexing
 * into a local resource table/array.
 *
 * Index values are only guaranteed to be unique for resources of the same
 * type.  For instance, the indices corresponding to two [valid] Library
 * IDs will never overlap, but the index of an Library and a library ID
 * may be the same.  Furthermore, indices may be reused if a resource is
 * deleted and re-created.
 *
 * @note There is no inverse of this function - indices cannot be converted
 * back to the original LibID value.  The caller should retain the original ID
 * for future use.
 *
 * @param[in]   LibID  Library ID to convert
 * @param[out]  Idx    Buffer where the calculated index will be stored
 *
 * @return Execution status, see @ref CFEReturnCodes
 * @retval #CFE_SUCCESS                 @copybrief CFE_SUCCESS
 */
int32 CFE_ES_LibID_ToIndex(CFE_ES_ResourceID_t LibID, uint32 *Idx);

/**
 * @brief Obtain an index value correlating to an ES Task ID
 *
 * This calculates a zero based integer value that may be used for indexing
 * into a local resource table/array.
 *
 * Index values are only guaranteed to be unique for resources of the same
 * type.  For instance, the indices corresponding to two [valid] Task
 * IDs will never overlap, but the index of an Task and a library ID
 * may be the same.  Furthermore, indices may be reused if a resource is
 * deleted and re-created.
 *
 * @note There is no inverse of this function - indices cannot be converted
 * back to the original TaskID value.  The caller should retain the original ID
 * for future use.
 *
 * @param[in]   TaskID  Task ID to convert
 * @param[out]  Idx     Buffer where the calculated index will be stored
 *
 * @return Execution status, see @ref CFEReturnCodes
 * @retval #CFE_SUCCESS                 @copybrief CFE_SUCCESS
 */
CFE_Status_t CFE_ES_TaskID_ToIndex(CFE_ES_ResourceID_t TaskID, uint32 *Idx);

/**
 * \brief Application Information
 * 
 * Structure that is used to provide information about an app.
 * It is primarily used for the QueryOne and QueryAll Commands.
 */
typedef struct CFE_ES_AppInfo
{
   CFE_ES_ResourceID_t   AppId;                          /**< \cfetlmmnemonic \ES_APP_ID
                                                 \brief Application ID for this Application */
   uint32   Type;                           /**< \cfetlmmnemonic \ES_APPTYPE
                                                 \brief The type of App: CORE or EXTERNAL */

   char     Name[CFE_MISSION_MAX_API_LEN];  /**< \cfetlmmnemonic \ES_APPNAME
                                                 \brief The Registered Name of the Application */
   char     EntryPoint[CFE_MISSION_MAX_API_LEN];    /**< \cfetlmmnemonic \ES_APPENTRYPT
                                                 \brief The Entry Point label for the Application */
   char     FileName[CFE_MISSION_MAX_PATH_LEN];     /**< \cfetlmmnemonic \ES_APPFILENAME
                                                 \brief The Filename of the file containing the Application */

   uint32   StackSize;                      /**< \cfetlmmnemonic \ES_STACKSIZE
                                                 \brief The Stack Size of the Application */
   osal_id_t   ModuleId;                       /**< \cfetlmmnemonic \ES_MODULEID
                                                 \brief The ID of the Loadable Module for the Application */
   uint32   AddressesAreValid;              /**< \cfetlmmnemonic \ES_ADDRVALID
                                                 \brief Indicates that the Code, Data, and BSS addresses/sizes are valid */
   uint32   CodeAddress;                    /**< \cfetlmmnemonic \ES_CODEADDR
                                                 \brief The Address of the Application Code Segment*/
   uint32   CodeSize;                       /**< \cfetlmmnemonic \ES_CODESIZE
                                                 \brief The Code Size of the Application */
   uint32   DataAddress;                    /**< \cfetlmmnemonic \ES_DATAADDR
                                                 \brief The Address of the Application Data Segment*/
   uint32   DataSize;                       /**< \cfetlmmnemonic \ES_DATASIZE
                                                 \brief The Data Size of the Application */
   uint32   BSSAddress;                     /**< \cfetlmmnemonic \ES_BSSADDR
                                                 \brief The Address of the Application BSS Segment*/
   uint32   BSSSize;                        /**< \cfetlmmnemonic \ES_BSSSIZE
                                                  \brief The BSS Size of the Application */
   uint32   StartAddress;                   /**< \cfetlmmnemonic \ES_STARTADDR
                                                 \brief The Start Address of the Application */
   uint16   ExceptionAction;                /**< \cfetlmmnemonic \ES_EXCEPTNACTN
                                                 \brief What should occur if Application has an exception
                                                 (Restart Application OR Restart Processor) */
   uint16   Priority;                       /**< \cfetlmmnemonic \ES_PRIORITY
                                                 \brief The Priority of the Application */
   CFE_ES_ResourceID_t   MainTaskId;        /**< \cfetlmmnemonic \ES_MAINTASKID
                                                 \brief The Application's Main Task ID */
   uint32   ExecutionCounter;               /**< \cfetlmmnemonic \ES_MAINTASKEXECNT
                                                 \brief The Application's Main Task Execution Counter */
   char     MainTaskName[CFE_MISSION_MAX_API_LEN];  /**< \cfetlmmnemonic \ES_MAINTASKNAME
                                                 \brief The Application's Main Task ID */
   uint32   NumOfChildTasks;                /**< \cfetlmmnemonic \ES_CHILDTASKS
                                                 \brief Number of Child tasks for an App */

} CFE_ES_AppInfo_t;

/**
 * \brief Task Info
 */
typedef struct CFE_ES_TaskInfo
{
   CFE_ES_ResourceID_t TaskId;                            /**< \brief Task Id */
   uint32              ExecutionCounter;                  /**< \brief Task Execution Counter */
   char                TaskName[CFE_MISSION_MAX_API_LEN]; /**< \brief Task Name */
   CFE_ES_ResourceID_t AppId;                             /**< \brief Parent Application ID */
   char                AppName[CFE_MISSION_MAX_API_LEN];  /**< \brief Parent Application Name */
} CFE_ES_TaskInfo_t;

/**
 * \brief Block statistics
 */
typedef struct CFE_ES_BlockStats
{
    CFE_ES_MemOffset_t BlockSize;    /**< \brief Number of bytes in each of these blocks */
    uint32  NumCreated;              /**< \brief Number of Memory Blocks of this size created */
    uint32  NumFree;                 /**< \brief Number of Memory Blocks of this size that are free */
} CFE_ES_BlockStats_t;

/**
 * \brief Memory Pool Statistics
 */
typedef struct CFE_ES_MemPoolStats
{
    CFE_ES_MemOffset_t    PoolSize;                /**< \cfetlmmnemonic \ES_POOLSIZE
                                                        \brief  Size of Memory Pool (in bytes) */
    uint32                NumBlocksRequested;      /**< \cfetlmmnemonic \ES_BLKSREQ
                                                        \brief Number of times a memory block has been allocated */
    uint32                CheckErrCtr;             /**< \cfetlmmnemonic \ES_BLKERRCTR
                                                        \brief Number of errors detected when freeing a memory block */
    CFE_ES_MemOffset_t    NumFreeBytes;            /**< \cfetlmmnemonic \ES_FREEBYTES
                                                        \brief Number of bytes never allocated to a block */
    CFE_ES_BlockStats_t   BlockStats[CFE_ES_DEFAULT_MEMPOOL_BLOCK_SIZES]; /**< \cfetlmmnemonic \ES_BLKSTATS
                                                                           \brief Contains stats on each block size */
} CFE_ES_MemPoolStats_t;

/**
 * \brief CDS Handle type
 *
 * Data type used to hold Handles of Critical Data Stores. See #CFE_ES_RegisterCDS
 */
typedef CFE_ES_ResourceID_t CFE_ES_CDSHandle_t;

/**
 * Type used for CDS sizes and offsets.
 *
 * This must match the type used in the PSP CDS API, e.g.:
 * CFE_PSP_GetCDSSize()
 * CFE_PSP_WriteToCDS()
 * CFE_PSP_ReadFromCDS()
 *
 * It is defined separately from the CFE_ES_MemOffset_t as the type used in
 * the PSP CDS access API may be different than the ES Pool API.
 *
 * In either case this _must_ be an unsigned type.
 */
typedef uint32 CFE_ES_CDS_Offset_t;

/**
 * \brief CDS Register Dump Record
 */
typedef struct CFE_ES_CDSRegDumpRec
{
    CFE_ES_CDSHandle_t    Handle;          /**< \brief Handle of CDS */
    uint32                Size;            /**< \brief Size, in bytes, of the CDS memory block */
    bool                  Table;           /**< \brief Flag that indicates whether CDS contains a Critical Table */
    char                  Name[CFE_ES_CDS_MAX_FULL_NAME_LEN]; /**< \brief Processor Unique Name of CDS */
    uint8                 ByteAlignSpare1; /**< \brief Spare byte to insure structure size is multiple of 4 bytes */
} CFE_ES_CDSRegDumpRec_t;

/*
** Child Task Main Function Prototype
*/
typedef void (*CFE_ES_ChildTaskMainFuncPtr_t)(void); /**< \brief Required Prototype of Child Task Main Functions */
typedef int32 (*CFE_ES_LibraryEntryFuncPtr_t)(CFE_ES_ResourceID_t LibId); /**< \brief Required Prototype of Library Initialization Functions */

/**
 * \brief Pool Alignement
 *
 * Union that can be used for minimum memory alignment of ES memory pools on the target.
 * It contains the longest native data types such that the alignment of this structure
 * should reflect the largest possible alignment requirements for any data on this processor.
 */
typedef union CFE_ES_PoolAlign
{
    void *Ptr; /**< \brief Aligned pointer */
    /* note -- native types (int/double) are intentional here */
    long long int LongInt; /**< \brief Aligned Long Integer */
    long double LongDouble; /**< \brief Aligned Long Double */
} CFE_ES_PoolAlign_t;

/**
 * \brief Static Pool Type
 *
 * A macro to help instantiate static memory pools that are correctly aligned.
 * This resolves to a union type that contains a member called "Data" that will
 * be correctly aligned to be a memory pool and sized according to the argument.
 */
#define CFE_ES_STATIC_POOL_TYPE(size)    union { CFE_ES_PoolAlign_t Align; uint8 Data[size]; }

/*****************************************************************************/
/*
** Exported Functions
*/

/*****************************************************************************/

/** @defgroup CFEAPIESEntryExit cFE Entry/Exit APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief cFE Main Entry Point used by Board Support Package to start cFE
**
** \par Description
**        cFE main entry point. This is the entry point into the cFE software.
**        It is called only by the Board Support Package software.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  StartType     Identifies whether this was a #CFE_PSP_RST_TYPE_POWERON or #CFE_PSP_RST_TYPE_PROCESSOR.
**
** \param[in]  StartSubtype  Specifies, in more detail, what caused the \c StartType identified above.
**                           See #CFE_PSP_RST_SUBTYPE_POWER_CYCLE for possible examples.
**
** \param[in]  ModeId        Identifies the source of the Boot as determined by the BSP.
**
** \param[in]  StartFilePath Identifies the startup file to use to initialize the cFE apps.
**
** \sa #CFE_ES_ResetCFE
**
******************************************************************************/
void CFE_ES_Main(uint32 StartType, uint32 StartSubtype, uint32 ModeId , const char *StartFilePath );

/*****************************************************************************/
/**
** \brief Reset the cFE Core and all cFE Applications
**
** \par Description
**        This API causes an immediate reset of the cFE Kernel and all cFE Applications.
**        The caller can specify whether the reset should clear all memory (#CFE_PSP_RST_TYPE_POWERON)
**        or try to retain volatile memory areas (#CFE_PSP_RST_TYPE_PROCESSOR).
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  ResetType    Identifies the type of reset desired.  Allowable settings are:
**                          \arg #CFE_PSP_RST_TYPE_POWERON     - Causes all memory to be cleared 
**                          \arg #CFE_PSP_RST_TYPE_PROCESSOR   - Attempts to retain volatile disk, critical data store and user reserved memory. 
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS            \copybrief CFE_SUCCESS
** \retval #CFE_ES_BAD_ARGUMENT    \copybrief CFE_ES_BAD_ARGUMENT
** \retval #CFE_ES_NOT_IMPLEMENTED \copybrief CFE_ES_NOT_IMPLEMENTED
**
** \sa #CFE_ES_Main
**
******************************************************************************/
CFE_Status_t  CFE_ES_ResetCFE(uint32 ResetType);
/**@}*/

/** @defgroup CFEAPIESAppControl cFE Application Control APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Restart a single cFE Application
**
** \par Description
**        This API causes a cFE Application to be stopped and restarted.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  AppID       Identifies the application to be reset.
**
** \return Execution status, see \ref CFEReturnCodes
**
** \sa #CFE_ES_ReloadApp, #CFE_ES_DeleteApp
**
******************************************************************************/
CFE_Status_t CFE_ES_RestartApp(CFE_ES_ResourceID_t AppID);

/*****************************************************************************/
/**
** \brief Reload a single cFE Application
**
** \par Description
**        This API causes a cFE Application to be stopped and restarted from
**        the specified file.
**
** \par Assumptions, External Events, and Notes:
**        The specified application will be deleted before it is reloaded from the
**        specified file.  In the event that an application cannot be reloaded due to
**        a corrupt file, the application may no longer be reloaded when given a valid
**        load file (it has been deleted and no longer exists).  To recover, the
**        application may be restarted by loading the application via the ES_STARTAPP
**        command (#CFE_ES_START_APP_CC).
**
** \param[in]  AppID       Identifies the application to be reset.
**
** \param[in]  AppFileName Identifies the new file to start.
**
** \return Execution status, see \ref CFEReturnCodes
**
** \sa #CFE_ES_RestartApp, #CFE_ES_DeleteApp, #CFE_ES_START_APP_CC
**
******************************************************************************/
CFE_Status_t CFE_ES_ReloadApp(CFE_ES_ResourceID_t AppID, const char *AppFileName);

/*****************************************************************************/
/**
** \brief Delete a cFE Application
**
** \par Description
**        This API causes a cFE Application to be stopped deleted.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  AppID       Identifies the application to be reset.
**
** \return Execution status, see \ref CFEReturnCodes
**
** \sa #CFE_ES_RestartApp, #CFE_ES_ReloadApp
**
******************************************************************************/
CFE_Status_t CFE_ES_DeleteApp(CFE_ES_ResourceID_t AppID);
/**@}*/

/** @defgroup CFEAPIESAppBehavior cFE Application Behavior APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Exit a cFE Application
**
** \par Description
**        This API is the "Exit Point" for the cFE application
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  ExitStatus Acceptable values are: \arg #CFE_ES_RunStatus_APP_EXIT - \copybrief CFE_ES_RunStatus_APP_EXIT
 \arg #CFE_ES_RunStatus_APP_ERROR - \copybrief CFE_ES_RunStatus_APP_ERROR
 \arg #CFE_ES_RunStatus_CORE_APP_INIT_ERROR - \copybrief CFE_ES_RunStatus_CORE_APP_INIT_ERROR
 \arg #CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR - \copybrief CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR
**    
**
** \sa #CFE_ES_RunLoop, #CFE_ES_RegisterApp
**
******************************************************************************/
void CFE_ES_ExitApp(uint32 ExitStatus);


/*****************************************************************************/
/**
** \brief Check for Exit, Restart, or Reload commands
**
** \par Description
**        This is the API that allows an app to check for exit requests from
**         the system.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  ExitStatus   A pointer to a variable containing the Application's
**                          desired run status.  Acceptable values are:
**                          \arg #CFE_ES_RunStatus_APP_RUN - \copybrief CFE_ES_RunStatus_APP_RUN
**                          \arg #CFE_ES_RunStatus_APP_EXIT - \copybrief CFE_ES_RunStatus_APP_EXIT
**                          \arg #CFE_ES_RunStatus_APP_ERROR - \copybrief CFE_ES_RunStatus_APP_ERROR
**
** \return Boolean indicating application should continue running
** \retval true  Application should continue running
** \retval false Application should not continue running
**
** \sa #CFE_ES_ExitApp, #CFE_ES_RegisterApp
**
******************************************************************************/
bool CFE_ES_RunLoop(uint32 *ExitStatus);

/*****************************************************************************/
/**
** \brief Allow an Application to Wait for a minimum global system state
**
** \par Description
**        This is the API that allows an app to wait for the rest of the apps
**        to complete a given stage of initialization before continuing.
**
**        This gives finer grained control than #CFE_ES_WaitForStartupSync
**
** \par Assumptions, External Events, and Notes:
**        This API assumes that the caller has also been initialized sufficiently
**        to satisfy the global system state it is waiting for, and the apps own
**        state will be updated accordingly.
**
** \param[in]  TimeOutMilliseconds   The timeout value in Milliseconds.
**                                   This parameter must be at least 1000. Lower values
**                                   will be rounded up. There is not an option to
**                                   wait indefinitely to avoid hanging a critical
**                                   application because a non-critical app did not start.
**
** \param[in]  MinSystemState        Determine the state of the App
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                State successfully achieved
** \retval #CFE_ES_OPERATION_TIMED_OUT Timeout was reached
**
** \sa #CFE_ES_RunLoop
**
******************************************************************************/
CFE_Status_t CFE_ES_WaitForSystemState(uint32 MinSystemState, uint32 TimeOutMilliseconds);

/*****************************************************************************/
/**
** \brief Allow an Application to Wait for the "OPERATIONAL" global system state
**
** \par Description
**        This is the API that allows an app to wait for the rest of the apps
**        to complete their entire initialization before continuing. It is most
**        useful for applications such as Health and Safety or the Scheduler that need
**        to wait until applications exist and are running before sending out
**        packets to them.
**
**        This is a specialized wrapper for CFE_ES_WaitForSystemState for compatibility
**        with applications using this API.
**
** \par Assumptions, External Events, and Notes:
**          This API should only be called as the last item of an Apps initialization.
**          In addition, this API should only be called by an App that is started
**          from the ES Startup file. It should not be used by an App that is 
**          started after the system is running. ( Although it will cause no harm )
**
** \param[in]  TimeOutMilliseconds   The timeout value in Milliseconds.
**                                   This parameter must be at least 1000. Lower values
**                                   will be rounded up. There is not an option to 
**                                   wait indefinitely to avoid hanging a critical
**                                   application because a non-critical app did not start. 
**
**
** \sa #CFE_ES_RunLoop
**
******************************************************************************/
void CFE_ES_WaitForStartupSync(uint32 TimeOutMilliseconds);

/*****************************************************************************/
/**
** \brief Registers a cFE Application with the Executive Services
**
** \par Description
**        This API registers the calling Application with the cFE.
**
** \par Assumptions, External Events, and Notes:
**        NOTE: This function \b MUST be called before any other cFE API functions are called.
**
** \return Execution status, see \ref CFEReturnCodes
**
** \sa #CFE_ES_ExitApp, #CFE_ES_RunLoop
**
******************************************************************************/
CFE_Status_t CFE_ES_RegisterApp(void);

/*****************************************************************************/
/**
** \ingroup CFEAPIESAppBehavior
** \brief Increments the execution counter for the calling task
**
** \par Description
**        This routine increments the execution counter that is stored for
**        the calling task. It can be called from cFE Application main tasks, child
**        tasks, or cFE Core application main tasks. Normally, the call is not
**        necessary from a cFE Application, since the CFE_ES_RunLoop call increments
**        the counter for the Application.
**
** \par Assumptions, External Events, and Notes:
**        NOTE: This API is not needed for Appplications that call the CFE_ES_RunLoop call.
**
** \sa #CFE_ES_RunLoop
**
******************************************************************************/
void  CFE_ES_IncrementTaskCounter(void);
/**@}*/

/** @defgroup CFEAPIESInfo cFE Information APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Return the most recent Reset Type
**
** \par Description
**        Provides the caller with codes that identifies the type of Reset
**        the processor most recently underwent.  The caller can also obtain
**        information on what caused the reset by supplying a pointer to a
**        variable that will be filled with the Reset Sub-Type.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in, out]   ResetSubtypePtr    Pointer to \c uint32 type variable in which the Reset Sub-Type will be stored.
**                                 The caller can set this pointer to NULL if the Sub-Type is of no interest. \n *ResetSubtypePtr If the provided pointer was not \c NULL, the Reset Sub-Type is stored at the given address.
**                                 For a list of possible Sub-Type values, see \link #CFE_PSP_RST_SUBTYPE_POWER_CYCLE "Reset Sub-Types" \endlink.
**
**
** \return Processor reset type
** \retval #CFE_PSP_RST_TYPE_POWERON   \copybrief CFE_PSP_RST_TYPE_POWERON
** \retval #CFE_PSP_RST_TYPE_PROCESSOR \copybrief CFE_PSP_RST_TYPE_PROCESSOR
**
** \sa #CFE_ES_GetAppID, #CFE_ES_GetAppIDByName, #CFE_ES_GetAppName, #CFE_ES_GetTaskInfo
**
******************************************************************************/
int32 CFE_ES_GetResetType(uint32 *ResetSubtypePtr);

/*****************************************************************************/
/**
** \brief Get an Application ID for the calling Application
**
** \par Description
**        This routine retrieves the cFE Application ID for the calling Application.  
**
** \par Assumptions, External Events, and Notes:
**        NOTE: \b All tasks associated with the Application would return the same Application ID.
**
** \param[out]   AppIdPtr       Pointer to variable that is to receive the Application's ID.
**                              *AppIdPtr will be set to the application ID of the calling Application.
**
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS       \copybrief CFE_SUCCESS
** \retval #CFE_ES_ERR_APPID  \copybrief CFE_ES_ERR_APPID
** \retval #CFE_ES_ERR_BUFFER \copybrief CFE_ES_ERR_BUFFER
**
** \sa #CFE_ES_GetResetType, #CFE_ES_GetAppIDByName, #CFE_ES_GetAppName, #CFE_ES_GetTaskInfo
**
******************************************************************************/
CFE_Status_t CFE_ES_GetAppID(CFE_ES_ResourceID_t *AppIdPtr);

/*****************************************************************************/
/**
** \brief Get the task ID of the calling context
**
** \par Description
**        This retrieves the current task context from OSAL
**
** \par Assumptions, External Events, and Notes:
**        Applications which desire to call other CFE ES services such as
**        CFE_ES_TaskGetInfo() should use this API rather than getting the ID
**        from OSAL directly via OS_TaskGetId().
**
** \param[out]   TaskIdPtr      Pointer to variable that is to receive the ID.
**                              Will be set to the ID of the calling task.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS          \copybrief CFE_SUCCESS
** \retval #CFE_ES_ERR_TASKID    \copybrief CFE_ES_ERR_TASKID
**
******************************************************************************/
CFE_Status_t CFE_ES_GetTaskID(CFE_ES_ResourceID_t *TaskIdPtr);

/*****************************************************************************/
/**
** \brief Get an Application ID associated with a specified Application name
**
** \par Description
**        This routine retrieves the cFE Application ID associated with a
**        specified Application name.  
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out]  AppIdPtr       Pointer to variable that is to receive the Application's ID.
** \param[in]   AppName        Pointer to null terminated character string containing an Application name.
**
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_ES_ERR_APPNAME  \copybrief CFE_ES_ERR_APPNAME
** \retval #CFE_ES_ERR_BUFFER   \copybrief CFE_ES_ERR_BUFFER
**
** \sa #CFE_ES_GetResetType, #CFE_ES_GetAppID, #CFE_ES_GetAppName, #CFE_ES_GetTaskInfo
**
******************************************************************************/
CFE_Status_t CFE_ES_GetAppIDByName(CFE_ES_ResourceID_t *AppIdPtr, const char *AppName);

/*****************************************************************************/
/**
** \brief Get an Application name for a specified Application ID
**
** \par Description
**        This routine retrieves the cFE Application name associated with a
**        specified Application ID.
**
** \par Assumptions, External Events, and Notes:
**        In the case of a failure (#CFE_ES_ERR_APPID), an empty string is returned.  #CFE_ES_ERR_APPID
**        will be returned if the specified Application ID (AppId) is invalid or not in use.
**
** \param[in, out]   AppName       Pointer to a character array of at least \c BufferLength in size that will
**                            be filled with the appropriate Application name.  *AppName is the null terminated Application name of the Application associated with the 
**                            specified Application ID
**
** \param[in]   AppId         Application ID of Application whose name is being requested.
**
** \param[in]   BufferLength  The maximum number of characters, including the null terminator, that can be put
**                            into the \c AppName buffer.  This routine will truncate the name to this length,
**                            if necessary.
**
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_ES_ERR_APPID    \copybrief CFE_ES_ERR_APPID
**
** \sa #CFE_ES_GetResetType, #CFE_ES_GetAppID, #CFE_ES_GetAppIDByName, #CFE_ES_GetTaskInfo
**
******************************************************************************/
CFE_Status_t CFE_ES_GetAppName(char *AppName, CFE_ES_ResourceID_t AppId, uint32 BufferLength);

/*****************************************************************************/
/**
** \brief Get Application Information given a specified App ID
**
** \par Description
**        This routine retrieves the information about an App associated with a
**        specified App ID. The information includes all of the information ES 
**        maintains for an application ( documented in the CFE_ES_AppInfo_t type ) 
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in, out]   AppInfo      Pointer to a \c CFE_ES_AppInfo_t structure that holds the specific 
**                            Application information. *AppInfo is the filled out \c CFE_ES_AppInfo_t structure containing the 
**                            App Name, and application memory addresses among other fields.
**
** \param[in]   AppId        Application ID of Application whose name is being requested.
**
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_ES_ERR_APPID    \copybrief CFE_ES_ERR_APPID
** \retval #CFE_ES_ERR_BUFFER   \copybrief CFE_ES_ERR_BUFFER
**
** \sa #CFE_ES_GetResetType, #CFE_ES_GetAppID, #CFE_ES_GetAppIDByName, #CFE_ES_GetAppName
**
******************************************************************************/
CFE_Status_t CFE_ES_GetAppInfo(CFE_ES_AppInfo_t *AppInfo, CFE_ES_ResourceID_t AppId);

/*****************************************************************************/
/**
** \brief Get Task Information given a specified Task ID
**
** \par Description
**        This routine retrieves the information about a Task associated with a
**        specified Task ID. The information includes Task Name, and Parent/Creator 
**        Application ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in, out]   TaskInfo      Pointer to a \c CFE_ES_TaskInfo_t structure that holds the specific 
**                            task information. *TaskInfo is the filled out \c CFE_ES_TaskInfo_t structure containing the 
**                            Task Name, Parent App Name, Parent App ID among other fields.
**
** \param[in]   TaskId        Application ID of Application whose name is being requested.
**
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_ES_ERR_TASKID   \copybrief CFE_ES_ERR_TASKID
** \retval #CFE_ES_ERR_BUFFER   \copybrief CFE_ES_ERR_BUFFER
**
** \sa #CFE_ES_GetResetType, #CFE_ES_GetAppID, #CFE_ES_GetAppIDByName, #CFE_ES_GetAppName
**
******************************************************************************/
CFE_Status_t CFE_ES_GetTaskInfo(CFE_ES_TaskInfo_t *TaskInfo, CFE_ES_ResourceID_t TaskId);
/**@}*/

/** @defgroup CFEAPIESChildTask cFE Child Task APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Registers a cFE Child task associated with a cFE Application
**
** \par Description
**        This routine registers a cFE Child task and associates it with its parent
**        cFE Application.  
**
** \par Assumptions, External Events, and Notes:
**        NOTE: This API \b MUST be called by the Child Task before any other cFE API calls are made.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                    \copybrief CFE_SUCCESS
** \retval #CFE_ES_ERR_CHILD_TASK_REGISTER \copybrief CFE_ES_ERR_CHILD_TASK_REGISTER
**
** \sa #CFE_ES_CreateChildTask, #CFE_ES_DeleteChildTask, #CFE_ES_ExitChildTask
**
******************************************************************************/
CFE_Status_t  CFE_ES_RegisterChildTask(void);

/*****************************************************************************/
/**
** \brief Creates a new task under an existing Application
**
** \par Description
**        This routine creates a new task (a separate execution thread) owned by the calling Application.  
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in, out]   TaskIdPtr     A pointer to a variable that will be filled in with the new task's ID. *TaskIdPtr is the Task ID of the newly created child task.
**
** \param[in]   TaskName      A pointer to a string containing the desired name of the new task.  
**                            This can be up to #OS_MAX_API_NAME characters, including the trailing null.
**
** \param[in]   FunctionPtr   A pointer to the function that will be spawned as a new task.  This function 
**                            must have the following signature: uint32 function(void).  Input parameters 
**                            for the new task are not supported.
**
** \param[in]   StackPtr      A pointer to the location where the child task's stack pointer should start.   
**                            NOTE: Not all underlying operating systems support this parameter.
**
** \param[in]   StackSize     The number of bytes to allocate for the new task's stack.
**
** \param[in]   Priority      The priority for the new task.  Lower numbers are higher priority, with 0 being 
**                            the highest priority.  Applications cannot create tasks with a higher priority 
**                            (lower number) than their own priority.
**
** \param[in]   Flags         Reserved for future expansion.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                  \copybrief CFE_SUCCESS
** \retval #CFE_ES_ERR_CHILD_TASK_CREATE \copybrief CFE_ES_ERR_CHILD_TASK_CREATE
**
** \sa #CFE_ES_RegisterChildTask, #CFE_ES_DeleteChildTask, #CFE_ES_ExitChildTask
**
******************************************************************************/
CFE_Status_t  CFE_ES_CreateChildTask(CFE_ES_ResourceID_t             *TaskIdPtr,
                                     const char                      *TaskName,
                                     CFE_ES_ChildTaskMainFuncPtr_t    FunctionPtr,
                                     uint32                          *StackPtr,
                                     uint32                           StackSize,
                                     uint32                           Priority,
                                     uint32                           Flags);

/*****************************************************************************/
/**
** \brief Deletes a task under an existing Application
**
** \par Description
**        This routine deletes a task under an Application specified by the \c TaskId obtained
**        when the child task was created using the #CFE_ES_CreateChildTask API.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   TaskId     The task ID previously obtained when the Child Task was created with the #CFE_ES_CreateChildTask API.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS            \copybrief CFE_SUCCESS
** \retval #CFE_ES_NOT_IMPLEMENTED \copybrief CFE_ES_NOT_IMPLEMENTED
**
** \sa #CFE_ES_RegisterChildTask, #CFE_ES_CreateChildTask, #CFE_ES_ExitChildTask
**
******************************************************************************/
CFE_Status_t CFE_ES_DeleteChildTask(CFE_ES_ResourceID_t TaskId);

/*****************************************************************************/
/**
** \brief Exits a child task
**
** \par Description
**        This routine allows the current executing child task to exit and 
**        be deleted by ES.
**
** \par Assumptions, External Events, and Notes:
**        This function cannot be called from an Application's Main Task.
**
** \note This function does not return a value, but if it does return
**        at all, it is assumed that the Task was either unregistered or
**        this function was called from a cFE Application's main task.
**
** \sa #CFE_ES_RegisterChildTask, #CFE_ES_CreateChildTask, #CFE_ES_DeleteChildTask
**
******************************************************************************/
void CFE_ES_ExitChildTask(void);
/**@}*/

/** @defgroup CFEAPIESMisc cFE Miscellaneous APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Write a string to the cFE System Log
**
** \par Description
**        This routine writes a formatted string to the cFE system log.  This 
**        can be used to record very low-level errors that can't be reported 
**        using the Event Services. This function is used in place of printf 
**        for flight software. It should be used for significant startup events, 
**        critical errors, and conditionally compiled debug software. 
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   SpecStringPtr     The format string for the log message.  
**                                This is similar to the format string for a printf() call.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
** \retval #CFE_ES_ERR_SYS_LOG_FULL \copybrief CFE_ES_ERR_SYS_LOG_FULL
**
******************************************************************************/
CFE_Status_t CFE_ES_WriteToSysLog(const char *SpecStringPtr, ...) OS_PRINTF(1,2);

/*****************************************************************************/
/**
** \brief Calculate a CRC on a block of memory
**
** \par Description
**        This routine calculates a cyclic redundancy check (CRC) on a block of memory.  The CRC algorithm
**        used is determined by the last parameter. 
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   DataPtr     Pointer to the base of the memory block.
**
** \param[in]   DataLength  The number of bytes in the memory block.
**
** \param[in]   InputCRC    A starting value for use in the CRC calculation.  This parameter
**                          allows the user to calculate the CRC of non-contiguous blocks as
**                          a single value.  Nominally, the user should set this value to zero.
**
** \param[in]   TypeCRC     One of the following CRC algorithm selections:
**                          \arg \c CFE_MISSION_ES_CRC_8 - (Not currently implemented)
**                          \arg \c CFE_MISSION_ES_CRC_16 - a CRC-16 algorithm
**                          \arg \c CFE_MISSION_ES_CRC_32 - (not currently implemented)
**
** \return The result of the CRC calculation on the specified memory block, or error code \ref CFEReturnCodes
**
******************************************************************************/
uint32 CFE_ES_CalculateCRC(const void *DataPtr, uint32 DataLength, uint32 InputCRC, uint32 TypeCRC);

/*****************************************************************************/
/**
** \ingroup CFEAPIESMisc
** \brief Notification that an asynchronous event was detected by the underlying OS/PSP
**
** \par Description
**        This hook routine is called from the PSP when an exception or
**        other asynchronous system event occurs
**
** \par Assumptions, External Events, and Notes:
**        The PSP must guarantee that this function is only invoked from a
**        context which may use OSAL primitives.  In general this means that
**        it shouldn't be _directly_ invoked from an ISR/signal context.
**
**
******************************************************************************/
void CFE_ES_ProcessAsyncEvent(void);

/**@}*/

/** @defgroup CFEAPIESCritData cFE Critical Data Store APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Reserve space (or re-obtain previously reserved space) in the Critical Data Store (CDS)
**
** \par Description
**        This routine allocates a block of memory in the Critical Data Store and associates it with
**        the calling Application.  The memory can survive an Application restart as well as a Processor Reset. 
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in, out]   HandlePtr   Pointer Application's variable that will contain the CDS Memory Block Handle. *HandlePtr is the handle of the CDS block that can be used in 
**                          #CFE_ES_CopyToCDS and #CFE_ES_RestoreFromCDS.
**
** \param[in]   BlockSize   The number of bytes needed in the CDS.
**
** \param[in]   Name        A pointer to a character string containing an application 
**                          unique name of #CFE_MISSION_ES_CDS_MAX_NAME_LENGTH characters or less.
**
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS               The memory block was successfully created in the CDS.
** \retval #CFE_ES_NOT_IMPLEMENTED    The processor does not support a Critical Data Store.
** \retval #CFE_ES_CDS_ALREADY_EXISTS \copybrief CFE_ES_CDS_ALREADY_EXISTS
** \retval #CFE_ES_CDS_INVALID_SIZE   \copybrief CFE_ES_CDS_INVALID_SIZE
** \retval #CFE_ES_CDS_INVALID_NAME   \copybrief CFE_ES_CDS_INVALID_NAME
** \retval #CFE_ES_CDS_REGISTRY_FULL  \copybrief CFE_ES_CDS_REGISTRY_FULL
**
** \sa #CFE_ES_CopyToCDS, #CFE_ES_RestoreFromCDS
**
******************************************************************************/
CFE_Status_t CFE_ES_RegisterCDS(CFE_ES_CDSHandle_t *HandlePtr, CFE_ES_CDS_Offset_t BlockSize, const char *Name);

/*****************************************************************************/
/**
** \brief Save a block of data in the Critical Data Store (CDS)
**
** \par Description
**        This routine copies a specified block of memory into the Critical Data Store that
**        had been previously registered via #CFE_ES_RegisterCDS.  The block of memory to be
**        copied must be at least as big as the size specified when registering the CDS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   Handle       The handle of the CDS block that was previously obtained from #CFE_ES_RegisterCDS.
**
** \param[in]   DataToCopy   A Pointer to the block of memory to be copied into the CDS.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #OS_SUCCESS              \copybrief OS_SUCCESS
** \retval #CFE_ES_ERR_MEM_HANDLE   \copybrief CFE_ES_ERR_MEM_HANDLE
** \retval #OS_ERROR                Problem with handle or a size mismatch
**
** \sa #CFE_ES_RegisterCDS, #CFE_ES_RestoreFromCDS
**
*/
CFE_Status_t CFE_ES_CopyToCDS(CFE_ES_CDSHandle_t Handle, void *DataToCopy);

/*****************************************************************************/
/**
** \brief Recover a block of data from the Critical Data Store (CDS)
**
** \par Description
**        This routine copies data from the Critical Data Store identified with the \c Handle into
**        the area of memory pointed to by the \c RestoreToMemory pointer.  The area of memory to
**        be copied into must be at least as big as the size specified when registering the CDS.
**        The recovery will indicate an error if the data integrity check maintained by the CDS 
**        indicates the contents of the CDS have changed.  However, the contents will still be
**        copied into the specified area of memory.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   Handle             The handle of the CDS block that was previously obtained from #CFE_ES_RegisterCDS.
**
** \param[in, out]   RestoreToMemory    A Pointer to the block of memory that is to be restored with the contents of the CDS. *RestoreToMemory is the contents of the specified CDS.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                \copybrief CFE_SUCCESS
** \retval #CFE_ES_CDS_BLOCK_CRC_ERR   \copybrief CFE_ES_CDS_BLOCK_CRC_ERR
** \retval #OS_ERROR                   Problem with handle or a size mismatch
**
** \sa #CFE_ES_RegisterCDS, #CFE_ES_CopyToCDS
**
*/
CFE_Status_t CFE_ES_RestoreFromCDS(void *RestoreToMemory, CFE_ES_CDSHandle_t Handle);
/**@}*/

/** @defgroup CFEAPIESMemManage cFE Memory Manager APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Initializes a memory pool created by an application without using a semaphore during processing.
**
** \par Description
**        This routine initializes a pool of memory supplied by the calling application.  When a memory pool
**        created by this routine is processed, no mutex handling is performed.  
**
** \par Assumptions, External Events, and Notes:
**        -# The size of the pool must be an integral number of 32-bit words
**        -# The start address of the pool must be 32-bit aligned
**        -# 168 bytes are used for internal bookkeeping, therefore, they will not be available for allocation.
**
** \param[in, out]   PoolID   A pointer to the variable the caller wishes to have the memory pool handle kept in. *PoolID is the memory pool handle.
**
** \param[in]   MemPtr      A Pointer to the pool of memory created by the calling application. This address must
**                          be on a 32-bit boundary.
**
** \param[in]   Size        The size of the pool of memory.  Note that this must be an integral number of 32 bit words.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS           \copybrief CFE_SUCCESS
** \retval #CFE_ES_BAD_ARGUMENT   \copybrief CFE_ES_BAD_ARGUMENT
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateEx, #CFE_ES_GetPoolBuf, #CFE_ES_PutPoolBuf, #CFE_ES_GetMemPoolStats
**
******************************************************************************/
CFE_Status_t CFE_ES_PoolCreateNoSem(CFE_ES_MemHandle_t *PoolID, uint8 *MemPtr, CFE_ES_MemOffset_t Size);

/*****************************************************************************/
/**
** \brief Initializes a memory pool created by an application while using a semaphore during processing.
**
** \par Description
**        This routine initializes a pool of memory supplied by the calling application.  When a memory pool
**        created by this routine is processed, mutex handling will be performed.  
**
** \par Assumptions, External Events, and Notes:
**        -# The size of the pool must be an integral number of 32-bit words
**        -# The start address of the pool must be 32-bit aligned
**        -# 168 bytes are used for internal bookkeeping, therefore, they will not be available for allocation.
**
** \param[in, out]   PoolID   A pointer to the variable the caller wishes to have the memory pool handle kept in. *PoolID is the memory pool handle.
**
** \param[in]   MemPtr      A Pointer to the pool of memory created by the calling application. This address must
**                          be on a 32-bit boundary.
**
** \param[in]   Size        The size of the pool of memory.  Note that this must be an integral number of 32 bit words.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS           \copybrief CFE_SUCCESS
** \retval #CFE_ES_BAD_ARGUMENT   \copybrief CFE_ES_BAD_ARGUMENT
**
** \sa #CFE_ES_PoolCreateNoSem, #CFE_ES_PoolCreateEx, #CFE_ES_GetPoolBuf, #CFE_ES_PutPoolBuf, #CFE_ES_GetMemPoolStats
**
******************************************************************************/
CFE_Status_t CFE_ES_PoolCreate(CFE_ES_MemHandle_t *PoolID, uint8 *MemPtr, CFE_ES_MemOffset_t Size);

/*****************************************************************************/
/**
** \brief Initializes a memory pool created by an application with application specified block sizes.
**
** \par Description
**        This routine initializes a pool of memory supplied by the calling application.  
**
** \par Assumptions, External Events, and Notes:
**        -# The size of the pool must be an integral number of 32-bit words
**        -# The start address of the pool must be 32-bit aligned
**        -# 168 bytes are used for internal bookkeeping, therefore, they will not be available for allocation.
**
** \param[in, out]   PoolID      A pointer to the variable the caller wishes to have the memory pool handle kept in. *PoolID is the memory pool handle.
**
** \param[in]   MemPtr         A Pointer to the pool of memory created by the calling application. This address must
**                             be on a 32-bit boundary.
**
** \param[in]   Size           The size of the pool of memory.  Note that this must be an integral number of 32 bit words.
**
** \param[in]   NumBlockSizes  The number of different block sizes specified in the \c BlockSizes array. If set equal to
**                             zero or if greater than 17, then default block sizes are used.
**
** \param[in]   BlockSizes     Pointer to an array of sizes to be used instead of the default block sizes specified by
**                             #CFE_PLATFORM_ES_MEM_BLOCK_SIZE_01 through #CFE_PLATFORM_ES_MAX_BLOCK_SIZE.  If the pointer is equal to NULL,
**                             the default block sizes are used.
**
** \param[in]   UseMutex       Flag indicating whether the new memory pool will be processing with mutex handling or not.
**                             Valid parameter values are #CFE_ES_USE_MUTEX and #CFE_ES_NO_MUTEX
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS           \copybrief CFE_SUCCESS
** \retval #CFE_ES_BAD_ARGUMENT   \copybrief CFE_ES_BAD_ARGUMENT
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateNoSem, #CFE_ES_GetPoolBuf, #CFE_ES_PutPoolBuf, #CFE_ES_GetMemPoolStats
**
******************************************************************************/
CFE_Status_t CFE_ES_PoolCreateEx(CFE_ES_MemHandle_t        *PoolID,
                                 uint8                     *MemPtr,
                                 CFE_ES_MemOffset_t         Size,
                                 uint16                     NumBlockSizes,
                                 const CFE_ES_MemOffset_t  *BlockSizes,
                                 uint16                     UseMutex );


/*****************************************************************************/
/**
** \brief Deletes a memory pool that was previously created
**
** \par Description
**        This routine removes the pool ID and frees the global table
**        entry for future re-use.
**
** \par Assumptions, External Events, and Notes:
**        All buffers associated with the pool become invalid after this call.
**        The application should ensure that buffers/references to the
**        pool are returned before deleting the pool.
**
** \param[in]   PoolID         The ID of the pool to delete
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS           \copybrief CFE_SUCCESS
** \retval #CFE_ES_ERR_MEM_HANDLE \copybrief CFE_ES_ERR_MEM_HANDLE
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateNoSem, #CFE_ES_GetPoolBuf, #CFE_ES_PutPoolBuf, #CFE_ES_GetMemPoolStats
**
******************************************************************************/
int32 CFE_ES_PoolDelete(CFE_ES_MemHandle_t PoolID);


/*****************************************************************************/
/**
** \brief Gets a buffer from the memory pool created by #CFE_ES_PoolCreate or #CFE_ES_PoolCreateNoSem
**
** \par Description
**        This routine obtains a block of memory from the memory pool supplied by the calling application.    
**
** \par Assumptions, External Events, and Notes:
**        -# The size allocated from the memory pool is, at a minimum, 12 bytes more than requested.
**
** \param[in, out]   BufPtr      A pointer to the Application's pointer in which will be stored the address of the allocated memory buffer. *BufPtr is the address of the requested buffer.
**
** \param[in]   PoolID   The handle to the memory pool as returned by #CFE_ES_PoolCreate or #CFE_ES_PoolCreateNoSem.
**
** \param[in]   Size        The size of the buffer requested.  NOTE: The size allocated may be larger.
**
** \return Bytes Allocated, or error code \ref CFEReturnCodes
** \retval #CFE_ES_ERR_MEM_HANDLE      \copybrief CFE_ES_ERR_MEM_HANDLE
** \retval #CFE_ES_ERR_MEM_BLOCK_SIZE  \copybrief CFE_ES_ERR_MEM_BLOCK_SIZE
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateNoSem, #CFE_ES_PoolCreateEx, #CFE_ES_PutPoolBuf, #CFE_ES_GetMemPoolStats, #CFE_ES_GetPoolBufInfo
**
******************************************************************************/
int32 CFE_ES_GetPoolBuf(uint32 **BufPtr, CFE_ES_MemHandle_t PoolID, CFE_ES_MemOffset_t Size);

/*****************************************************************************/
/**
** \brief Gets info on a buffer previously allocated via #CFE_ES_GetPoolBuf
**
** \par Description
**        This routine gets info on a buffer in the memory pool.    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   PoolID   The handle to the memory pool as returned by #CFE_ES_PoolCreate or #CFE_ES_PoolCreateNoSem.
**
** \param[in]   BufPtr      A pointer to the memory buffer to provide status for.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                \copybrief CFE_SUCCESS
** \retval #CFE_ES_ERR_MEM_HANDLE      \copybrief CFE_ES_ERR_MEM_HANDLE
** \retval #CFE_ES_BUFFER_NOT_IN_POOL  \copybrief CFE_ES_BUFFER_NOT_IN_POOL
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateNoSem, #CFE_ES_PoolCreateEx, #CFE_ES_GetPoolBuf, #CFE_ES_GetMemPoolStats, #CFE_ES_PutPoolBuf
**
******************************************************************************/
CFE_Status_t CFE_ES_GetPoolBufInfo(CFE_ES_MemHandle_t PoolID, uint32 *BufPtr);

/*****************************************************************************/
/**
** \brief Releases a buffer from the memory pool that was previously allocated via #CFE_ES_GetPoolBuf
**
** \par Description
**        This routine releases a buffer back into the memory pool.    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   PoolID   The handle to the memory pool as returned by #CFE_ES_PoolCreate or #CFE_ES_PoolCreateNoSem.
**
** \param[in]   BufPtr      A pointer to the memory buffer to be released.
**
** \return Bytes released, or error code \ref CFEReturnCodes
** \retval #CFE_ES_ERR_MEM_HANDLE  \copybrief CFE_ES_ERR_MEM_HANDLE
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateNoSem, #CFE_ES_PoolCreateEx, #CFE_ES_GetPoolBuf, #CFE_ES_GetMemPoolStats, #CFE_ES_GetPoolBufInfo
**
******************************************************************************/
int32 CFE_ES_PutPoolBuf(CFE_ES_MemHandle_t PoolID, uint32 *BufPtr);

/*****************************************************************************/
/**
** \brief Extracts the statistics maintained by the memory pool software
**
** \par Description
**        This routine fills the #CFE_ES_MemPoolStats_t data structure with the statistics
**        maintained by the memory pool software.  These statistics can then be telemetered
**        by the calling Application.    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in, out]   BufPtr      Pointer to #CFE_ES_MemPoolStats_t data structure to be 
**                          filled with memory statistics. *BufPtr is the Memory Pool Statistics stored in given data structure.
**
** \param[in]   Handle      The handle to the memory pool whose statistics are desired.
**
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                \copybrief CFE_SUCCESS
** \retval #CFE_ES_ERR_MEM_HANDLE      \copybrief CFE_ES_ERR_MEM_HANDLE
**
** \sa #CFE_ES_PoolCreate, #CFE_ES_PoolCreateNoSem, #CFE_ES_PoolCreateEx, #CFE_ES_GetPoolBuf, #CFE_ES_PutPoolBuf
**
******************************************************************************/
CFE_Status_t CFE_ES_GetMemPoolStats(CFE_ES_MemPoolStats_t *BufPtr, CFE_ES_MemHandle_t  Handle);
/**@}*/

/** @defgroup CFEAPIESPerfMon cFE Performance Monitor APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Entry marker for use with Software Performance Analysis Tool.
**
** \par Description
**        This macro logs the entry or start event/marker for the specified 
**        entry \c id. This macro, in conjunction with the #CFE_ES_PerfLogExit, 
**        is used by the Software Performance Analysis tool (see section 5.15).    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   id          Identifier of the specific event or marker.
**
** \sa #CFE_ES_PerfLogExit, #CFE_ES_PerfLogAdd
**
******************************************************************************/
#define CFE_ES_PerfLogEntry(id) (CFE_ES_PerfLogAdd(id, 0))

/*****************************************************************************/
/**
** \brief Exit marker for use with Software Performance Analysis Tool.
**
** \par Description
**        This macro logs the exit or end event/marker for the specified 
**        entry \c id. This macro, in conjunction with the #CFE_ES_PerfLogEntry, 
**        is used by the Software Performance Analysis tool (see section 5.15).    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   id          Identifier of the specific event or marker.
**
** \sa #CFE_ES_PerfLogEntry, #CFE_ES_PerfLogAdd
**
******************************************************************************/
#define CFE_ES_PerfLogExit(id) (CFE_ES_PerfLogAdd(id, 1))


/*****************************************************************************/
/**
** \brief Function called by #CFE_ES_PerfLogEntry and #CFE_ES_PerfLogExit macros
**
** \par Description
**        This function logs the entry and exit marker for the specified 
**        \c id. This function is used by the Software Performance Analysis 
**        tool (see section 5.15).    
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]   Marker          Identifier of the specific event or marker.
** \param[in]   EntryExit       Used to specify Entry(0) or Exit(1)
**
** \sa #CFE_ES_PerfLogEntry, #CFE_ES_PerfLogExit
**
******************************************************************************/
void CFE_ES_PerfLogAdd(uint32 Marker, uint32 EntryExit);
/**@}*/

/** @defgroup CFEAPIESGenCount cFE Generic Counter APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Register a generic counter
**
** \par Description
**        This routine registers a generic counter. 
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]  *CounterName   The Name of the generic counter.
**
** \param[out] *CounterIdPtr  The Counter Id of the newly created counter.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_ES_BAD_ARGUMENT \copybrief CFE_ES_BAD_ARGUMENT
**
** \sa #CFE_ES_IncrementGenCounter, #CFE_ES_DeleteGenCounter, #CFE_ES_SetGenCount, #CFE_ES_GetGenCount, #CFE_ES_GetGenCounterIDByName
**
******************************************************************************/
CFE_Status_t CFE_ES_RegisterGenCounter(CFE_ES_ResourceID_t *CounterIdPtr, const char *CounterName);

/*****************************************************************************/
/**
** \brief Delete a generic counter
**
** \par Description
**        This routine deletes a previously registered generic counter. 
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]  CounterId     The Counter Id of the newly created counter.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS          \copybrief CFE_SUCCESS
** \retval #CFE_ES_BAD_ARGUMENT  \copybrief CFE_ES_BAD_ARGUMENT
**
** \sa #CFE_ES_IncrementGenCounter, #CFE_ES_RegisterGenCounter, #CFE_ES_SetGenCount, #CFE_ES_GetGenCount, #CFE_ES_GetGenCounterIDByName
**
******************************************************************************/
CFE_Status_t CFE_ES_DeleteGenCounter(CFE_ES_ResourceID_t CounterId);

/*****************************************************************************/
/**
** \brief Increments the specified generic counter
**
** \par Description
**        This routine increments the specified generic counter. 
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]   CounterId    The Counter to be incremented.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_ES_BAD_ARGUMENT \copybrief CFE_ES_BAD_ARGUMENT
**
** \sa #CFE_ES_RegisterGenCounter, #CFE_ES_DeleteGenCounter, #CFE_ES_SetGenCount, #CFE_ES_GetGenCount, #CFE_ES_GetGenCounterIDByName
**
******************************************************************************/
CFE_Status_t CFE_ES_IncrementGenCounter(CFE_ES_ResourceID_t CounterId);

/*****************************************************************************/
/**
** \brief Set the specified generic counter
**
** \par Description
**        This routine sets the specified generic counter to the specified value. 
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]   CounterId    The Counter to be set.
**
** \param[in]   Count        The new value of the Counter.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS          \copybrief CFE_SUCCESS
** \retval #CFE_ES_BAD_ARGUMENT  \copybrief CFE_ES_BAD_ARGUMENT
**
** \sa #CFE_ES_RegisterGenCounter, #CFE_ES_DeleteGenCounter, #CFE_ES_IncrementGenCounter, #CFE_ES_GetGenCount, #CFE_ES_GetGenCounterIDByName
**
******************************************************************************/
CFE_Status_t CFE_ES_SetGenCount(CFE_ES_ResourceID_t CounterId, uint32 Count);

/*****************************************************************************/
/**
** \brief Get the specified generic counter count
**
** \par Description
**        This routine gets the value of a generic counter. 
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]   CounterId    The Counter to get the value from.
**
** \param[in]   *Count     The value of the Counter.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS          \copybrief CFE_SUCCESS
** \retval #CFE_ES_BAD_ARGUMENT  \copybrief CFE_ES_BAD_ARGUMENT
**
** \sa #CFE_ES_RegisterGenCounter, #CFE_ES_DeleteGenCounter, #CFE_ES_SetGenCount, #CFE_ES_IncrementGenCounter, #CFE_ES_GetGenCounterIDByName
**
******************************************************************************/
CFE_Status_t CFE_ES_GetGenCount(CFE_ES_ResourceID_t CounterId, uint32 *Count);


/*****************************************************************************/
/**
** \brief Get the Id associated with a generic counter name
**
** \par Description
**        This routine gets the Counter Id for a generic counter specified by name.
**
** \par Assumptions, External Events, and Notes:
**        None.
**
** \param[in]   *CounterName     The name of the Counter.
**
** \param[out]  *CounterIdPtr    The Counter Id for the given name.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS          \copybrief CFE_SUCCESS
** \retval #CFE_ES_BAD_ARGUMENT  \copybrief CFE_ES_BAD_ARGUMENT
**
** \sa #CFE_ES_RegisterGenCounter, #CFE_ES_DeleteGenCounter, #CFE_ES_SetGenCount, #CFE_ES_IncrementGenCounter, #CFE_ES_GetGenCount
******************************************************************************/
CFE_Status_t CFE_ES_GetGenCounterIDByName(CFE_ES_ResourceID_t *CounterIdPtr, const char *CounterName);
/**@}*/

#endif  /* _cfe_es_ */
