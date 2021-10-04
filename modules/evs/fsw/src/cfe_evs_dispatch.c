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
**  File: cfe_evs_task.c
**
**  Title: Event Service API Management Control Interfaces
**
**  Purpose: This module defines the top level functions of the
**           cFE Event Service task defining the control, command,
**           and telemetry interfaces
**
*/

/* Include Files */
#include "cfe_evs_module_all.h" /* EVS internal definitions */

#include "cfe_evs_eds_dictionary.h"
#include "cfe_evs_eds_dispatcher.h"
#include "cfe_msg.h"

/*
 * Define a lookup table for EVS command codes
 */
static const CFE_EVS_Application_Component_Telecommand_DispatchTable_t CFE_EVS_TC_DISPATCH_TABLE = {
    .CMD     = {.AddEventFilterCmd_indication      = CFE_EVS_AddEventFilterCmd,
            .ClearLogCmd_indication            = CFE_EVS_ClearLogCmd,
            .DeleteEventFilterCmd_indication   = CFE_EVS_DeleteEventFilterCmd,
            .DisableAppEventTypeCmd_indication = CFE_EVS_DisableAppEventTypeCmd,
            .DisableAppEventsCmd_indication    = CFE_EVS_DisableAppEventsCmd,
            .DisableEventTypeCmd_indication    = CFE_EVS_DisableEventTypeCmd,
            .DisablePortsCmd_indication        = CFE_EVS_DisablePortsCmd,
            .EnableAppEventTypeCmd_indication  = CFE_EVS_EnableAppEventTypeCmd,
            .EnableAppEventsCmd_indication     = CFE_EVS_EnableAppEventsCmd,
            .EnableEventTypeCmd_indication     = CFE_EVS_EnableEventTypeCmd,
            .EnablePortsCmd_indication         = CFE_EVS_EnablePortsCmd,
            .WriteAppDataFileCmd_indication    = CFE_EVS_WriteAppDataFileCmd,
            .WriteLogDataFileCmd_indication    = CFE_EVS_WriteLogDataFileCmd,
            .NoopCmd_indication                = CFE_EVS_NoopCmd,
            .ResetAllFiltersCmd_indication     = CFE_EVS_ResetAllFiltersCmd,
            .ResetAppCounterCmd_indication     = CFE_EVS_ResetAppCounterCmd,
            .ResetCountersCmd_indication       = CFE_EVS_ResetCountersCmd,
            .ResetFilterCmd_indication         = CFE_EVS_ResetFilterCmd,
            .SetEventFormatModeCmd_indication  = CFE_EVS_SetEventFormatModeCmd,
            .SetFilterCmd_indication           = CFE_EVS_SetFilterCmd,
            .SetLogModeCmd_indication          = CFE_EVS_SetLogModeCmd},
    .SEND_HK = {.indication = CFE_EVS_ReportHousekeepingCmd}};

/*----------------------------------------------------------------
 *
 * Function: CFE_EVS_ProcessCommandPacket
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_EVS_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    int32             Status;
    CFE_SB_MsgId_t    MsgId;
    CFE_MSG_Size_t    MsgSize;
    CFE_MSG_FcnCode_t MsgFc;

    Status = CFE_EVS_Application_Component_Telecommand_Dispatch(CFE_SB_Telecommand_indication_Command_ID, SBBufPtr,
                                                                &CFE_EVS_TC_DISPATCH_TABLE);

    if (Status != CFE_SUCCESS)
    {
        CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

        if (Status == CFE_STATUS_BAD_COMMAND_CODE)
        {
            CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &MsgFc);
            EVS_SendEvent(CFE_EVS_ERR_CC_EID, CFE_EVS_EventType_ERROR, "Invalid command code -- ID = 0x%08x, CC = %d",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (int)MsgFc);
        }
        else if (Status == CFE_STATUS_WRONG_MSG_LENGTH)
        {
            CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &MsgFc);
            CFE_MSG_GetSize(&SBBufPtr->Msg, &MsgSize);
            EVS_SendEvent(CFE_EVS_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid cmd length: ID = 0x%X, CC = %d, Len = %d", (unsigned int)CFE_SB_MsgIdToValue(MsgId),
                          (int)MsgFc, (int)MsgSize);
        }
        else if (Status == CFE_STATUS_UNKNOWN_MSG_ID)
        {
            EVS_SendEvent(CFE_EVS_ERR_MSGID_EID, CFE_EVS_EventType_ERROR, "Invalid command packet, Message ID = 0x%08X",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId));
        }
    }

    if (Status == CFE_SUCCESS)
    {
        CFE_EVS_Global.EVS_TlmPkt.Payload.CommandCounter++;
    }
    else if (Status < 0) /* Negative values indicate errors */
    {
        CFE_EVS_Global.EVS_TlmPkt.Payload.CommandErrorCounter++;
    }
}