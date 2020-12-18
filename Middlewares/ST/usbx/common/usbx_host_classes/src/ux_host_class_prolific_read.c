/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** USBX Component                                                        */ 
/**                                                                       */
/**   Prolific Class                                                      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include "ux_api.h"
#include "ux_host_class_prolific.h"
#include "ux_host_stack.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _ux_host_class_prolific_read                        PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function reads from the prolific interface. The call is        */ 
/*    blocking and only returns when there is either an error or when     */ 
/*    the transfer is complete.                                           */
/*                                                                        */
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    prolific                              Pointer to prolific class     */ 
/*    data_pointer                          Pointer to buffer             */
/*    requested_length                      Requested data read           */
/*    actual_length                         Actual data read              */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    Completion Status                                                   */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _ux_host_stack_transfer_request       Process transfer request      */ 
/*    _ux_host_stack_transfer_request_abort Abort transfer request        */ 
/*    _ux_utility_semaphore_get             Get protection semaphore      */ 
/*    _ux_utility_semaphore_put             Release protection semaphore  */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application                                                         */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            prefixed UX to MS_TO_TICK,  */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  _ux_host_class_prolific_read (UX_HOST_CLASS_PROLIFIC *prolific, UCHAR *data_pointer, 
                                    ULONG requested_length, ULONG *actual_length)
{

UX_TRANSFER     *transfer_request;
UINT            status;
ULONG           transfer_request_length;
    
    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_HOST_CLASS_PROLIFIC_READ, prolific, data_pointer, requested_length, 0, UX_TRACE_HOST_CLASS_EVENTS, 0, 0)

    /* Ensure the instance is valid.  */
    if (prolific -> ux_host_class_prolific_state !=  UX_HOST_CLASS_INSTANCE_LIVE)
    {        

        /* Error trap. */
        _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_CLASS, UX_HOST_CLASS_INSTANCE_UNKNOWN);

        /* If trace is enabled, insert this event into the trace buffer.  */
        UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_HOST_CLASS_INSTANCE_UNKNOWN, prolific, 0, 0, UX_TRACE_ERRORS, 0, 0)

        return(UX_HOST_CLASS_INSTANCE_UNKNOWN);
    }

    /* Start by resetting the actual length of the transfer to zero.  */
    *actual_length =  0;

    /* Get the pointer to the bulk in endpoint in the transfer_request.  */
    transfer_request =  &prolific -> ux_host_class_prolific_bulk_in_endpoint -> ux_endpoint_transfer_request;
    
    /* Perform a transfer on the bulk in endpoint until either the transfer is
       completed or until there is an error.  */
    while (requested_length)
    {

        /* Program the maximum authorized length for this transfer request.  */
        if (requested_length > transfer_request -> ux_transfer_request_maximum_length)
            transfer_request_length =  transfer_request -> ux_transfer_request_maximum_length;
        else
            transfer_request_length =  requested_length;
                    
        /* Initialize the transfer request.  */
        transfer_request -> ux_transfer_request_data_pointer =      data_pointer;
        transfer_request -> ux_transfer_request_requested_length =  transfer_request_length;
        
        /* Perform the transfer.  */
        status =  _ux_host_stack_transfer_request(transfer_request);

        /* If the transfer is successful, we need to wait for the transfer request to be completed.  */
        if (status == UX_SUCCESS)
        {
            
            /* Wait for the completion of the transfer_request.  */
            status =  _ux_utility_semaphore_get(&transfer_request -> ux_transfer_request_semaphore, UX_MS_TO_TICK(UX_HOST_CLASS_PROLIFIC_CLASS_TRANSFER_TIMEOUT));

            /* If the semaphore did not succeed we probably have a time out.  */
            if (status != UX_SUCCESS)
            {

                /* All transfers pending need to abort. There may have been a partial transfer.  */
                _ux_host_stack_transfer_request_abort(transfer_request);
                
                /* Update the length of the actual data transferred. We do this after the 
                   abort of the transfer request in case some data was actually received.  */
                *actual_length +=  transfer_request -> ux_transfer_request_actual_length;
            
                /* Set the completion code.  */
                transfer_request -> ux_transfer_request_completion_code =  UX_TRANSFER_TIMEOUT;
        
                /* Error trap. */
                _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_CLASS, UX_TRANSFER_TIMEOUT);

                /* If trace is enabled, insert this event into the trace buffer.  */
                UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_TRANSFER_TIMEOUT, transfer_request, 0, 0, UX_TRACE_ERRORS, 0, 0)
        
                /* There was an error, return to the caller */
                return(UX_TRANSFER_TIMEOUT);
            }            
        }
        else
        {

            /* There was a non transfer error, no partial transfer to be checked.  */
            return(status);
        }

        /* Update the length of the transfer. Normally all the data has to be received.  */
        *actual_length +=  transfer_request -> ux_transfer_request_actual_length;

        /* Check for completion status.  */
        if (transfer_request -> ux_transfer_request_completion_code != UX_SUCCESS)
            return(transfer_request -> ux_transfer_request_completion_code);
        
        /* Check for completion of transfer. If the transfer is partial, return to caller.
           The transfer is marked as successful but the caller will need to check the length
           actually received and determine if a partial transfer is OK.  */
        if (transfer_request_length != transfer_request -> ux_transfer_request_actual_length)
        {
        
            /* Return success to caller.  */
            return(UX_SUCCESS);
        }

        /* Update the data pointer for next transfer. */        
        data_pointer +=  transfer_request_length;
        
        /* Update what is left to receive.  */
        requested_length -=  transfer_request_length;          
    }    

    /* We get here when all the transfers went through without errors.  */
    return(UX_SUCCESS); 
}

