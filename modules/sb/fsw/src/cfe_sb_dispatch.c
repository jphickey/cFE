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

/******************************************************************************
** File: cfe_sb_task.c
**
** Purpose:
**      This file contains the source code for the SB task.
**
** Author:   R.McGraw/SSI
**
******************************************************************************/

/* Include Files */

#include "cfe_sb_module_all.h"
#include "cfe_sb_eds_dictionary.h"
#include "cfe_sb_eds_dispatcher.h"
#include "cfe_msg.h"

/*
 * Define a lookup table for SB command codes
 */
const CFE_SB_Application_Component_Telecommand_DispatchTable_t CFE_SB_TC_DISPATCH_TABLE = {
    .CMD = {.DisableRouteCmd_indication     = CFE_SB_DisableRouteCmd,
            .EnableRouteCmd_indication      = CFE_SB_EnableRouteCmd,
            .NoopCmd_indication             = CFE_SB_NoopCmd,
            .ResetCountersCmd_indication    = CFE_SB_ResetCountersCmd,
            .WriteMapInfoCmd_indication     = CFE_SB_WriteMapInfoCmd,
            .WritePipeInfoCmd_indication    = CFE_SB_WritePipeInfoCmd,
            .WriteRoutingInfoCmd_indication = CFE_SB_WriteRoutingInfoCmd,
            .SendSbStatsCmd_indication      = CFE_SB_SendStatsCmd},
    .SUB_RPT_CTRL =
        {
            .DisableSubReportingCmd_indication = CFE_SB_DisableSubReportingCmd,
            .EnableSubReportingCmd_indication  = CFE_SB_EnableSubReportingCmd,
            .SendPrevSubsCmd_indication        = CFE_SB_SendPrevSubsCmd,
        },
    .SEND_HK = {.indication = CFE_SB_SendHKTlmCmd}};

/*----------------------------------------------------------------
 *
 * Function: CFE_SB_ProcessCmdPipePkt
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_ProcessCmdPipePkt(CFE_SB_Buffer_t *SBBufPtr)
{
    int32             Status;
    CFE_SB_MsgId_t    MsgId;
    CFE_MSG_FcnCode_t MsgFc;

    Status = CFE_SB_Application_Component_Telecommand_Dispatch(CFE_SB_Telecommand_indication_Command_ID, SBBufPtr,
                                                               &CFE_SB_TC_DISPATCH_TABLE);

    if (Status != CFE_SUCCESS)
    {
        CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);
        CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &MsgFc);

        if (Status == CFE_STATUS_BAD_COMMAND_CODE)
        {
            CFE_EVS_SendEvent(CFE_SB_BAD_CMD_CODE_EID, CFE_EVS_EventType_ERROR,
                              "Invalid Cmd, Unexpected Command Code %d", (int)MsgFc);
            CFE_SB_Global.HKTlmMsg.Payload.CommandErrorCounter++;
        }
        else if (Status == CFE_STATUS_WRONG_MSG_LENGTH)
        {
            CFE_EVS_SendEvent(CFE_SB_LEN_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid cmd length: ID = 0x%X, CC = %d",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId), (int)MsgFc);
            ++CFE_SB_Global.HKTlmMsg.Payload.CommandErrorCounter;
        }
        else
        {
            CFE_EVS_SendEvent(CFE_SB_BAD_MSGID_EID, CFE_EVS_EventType_ERROR, "Invalid Cmd, Unexpected Msg Id: 0x%04x",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            CFE_SB_Global.HKTlmMsg.Payload.CommandErrorCounter++;
        }
    }
}