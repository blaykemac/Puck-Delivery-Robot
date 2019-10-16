/*******************************************************************************
* File Name: Motor_Right_Decoder_PM.c
* Version 3.0
*
* Description:
*  This file contains the setup, control and status commands to support 
*  component operations in low power mode.  
*
* Note:
*  None.
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Motor_Right_Decoder.h"

static Motor_Right_Decoder_BACKUP_STRUCT Motor_Right_Decoder_backup = {0u};


/*******************************************************************************
* Function Name: Motor_Right_Decoder_SaveConfig
********************************************************************************
* Summary:
*  Saves the current user configuration of the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Motor_Right_Decoder_SaveConfig(void) 
{
    #if (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_8_BIT)
        Motor_Right_Decoder_Cnt8_SaveConfig();
    #else 
        /* (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_16_BIT) || 
         * (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT)
         */
        Motor_Right_Decoder_Cnt16_SaveConfig();
    #endif /* (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_8_BIT) */
}


/*******************************************************************************
* Function Name: Motor_Right_Decoder_RestoreConfig
********************************************************************************
* Summary:
*  Restores the current user configuration of the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Motor_Right_Decoder_RestoreConfig(void) 
{
    #if (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_8_BIT)
        Motor_Right_Decoder_Cnt8_RestoreConfig();
    #else 
        /* (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_16_BIT) || 
         * (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT) 
         */
        Motor_Right_Decoder_Cnt16_RestoreConfig();
    #endif /* (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_8_BIT) */
}


/*******************************************************************************
* Function Name: Motor_Right_Decoder_Sleep
********************************************************************************
* 
* Summary:
*  Prepare Quadrature Decoder Component goes to sleep.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Motor_Right_Decoder_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Motor_Right_Decoder_Sleep(void) 
{
    if (0u != (Motor_Right_Decoder_SR_AUX_CONTROL & Motor_Right_Decoder_INTERRUPTS_ENABLE))
    {
        Motor_Right_Decoder_backup.enableState = 1u;
    }
    else /* The Quadrature Decoder Component is disabled */
    {
        Motor_Right_Decoder_backup.enableState = 0u;
    }

    Motor_Right_Decoder_Stop();
    Motor_Right_Decoder_SaveConfig();
}


/*******************************************************************************
* Function Name: Motor_Right_Decoder_Wakeup
********************************************************************************
*
* Summary:
*  Prepare Quadrature Decoder Component to wake up.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Motor_Right_Decoder_backup - used when non-retention registers are restored.
*
*******************************************************************************/
void Motor_Right_Decoder_Wakeup(void) 
{
    Motor_Right_Decoder_RestoreConfig();

    if (Motor_Right_Decoder_backup.enableState != 0u)
    {
        #if (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_8_BIT)
            Motor_Right_Decoder_Cnt8_Enable();
        #else 
            /* (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_16_BIT) || 
            *  (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT) 
            */
            Motor_Right_Decoder_Cnt16_Enable();
        #endif /* (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_8_BIT) */

        /* Enable component's operation */
        Motor_Right_Decoder_Enable();
    } /* Do nothing if component's block was disabled before */
}


/* [] END OF FILE */

