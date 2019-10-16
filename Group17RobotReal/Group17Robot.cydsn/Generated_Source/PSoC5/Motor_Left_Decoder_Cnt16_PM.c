/*******************************************************************************
* File Name: Motor_Left_Decoder_Cnt16_PM.c  
* Version 3.0
*
*  Description:
*    This file provides the power management source code to API for the
*    Counter.  
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "Motor_Left_Decoder_Cnt16.h"

static Motor_Left_Decoder_Cnt16_backupStruct Motor_Left_Decoder_Cnt16_backup;


/*******************************************************************************
* Function Name: Motor_Left_Decoder_Cnt16_SaveConfig
********************************************************************************
* Summary:
*     Save the current user configuration
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  Motor_Left_Decoder_Cnt16_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void Motor_Left_Decoder_Cnt16_SaveConfig(void) 
{
    #if (!Motor_Left_Decoder_Cnt16_UsingFixedFunction)

        Motor_Left_Decoder_Cnt16_backup.CounterUdb = Motor_Left_Decoder_Cnt16_ReadCounter();

        #if(!Motor_Left_Decoder_Cnt16_ControlRegRemoved)
            Motor_Left_Decoder_Cnt16_backup.CounterControlRegister = Motor_Left_Decoder_Cnt16_ReadControlRegister();
        #endif /* (!Motor_Left_Decoder_Cnt16_ControlRegRemoved) */

    #endif /* (!Motor_Left_Decoder_Cnt16_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Motor_Left_Decoder_Cnt16_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  Motor_Left_Decoder_Cnt16_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Motor_Left_Decoder_Cnt16_RestoreConfig(void) 
{      
    #if (!Motor_Left_Decoder_Cnt16_UsingFixedFunction)

       Motor_Left_Decoder_Cnt16_WriteCounter(Motor_Left_Decoder_Cnt16_backup.CounterUdb);

        #if(!Motor_Left_Decoder_Cnt16_ControlRegRemoved)
            Motor_Left_Decoder_Cnt16_WriteControlRegister(Motor_Left_Decoder_Cnt16_backup.CounterControlRegister);
        #endif /* (!Motor_Left_Decoder_Cnt16_ControlRegRemoved) */

    #endif /* (!Motor_Left_Decoder_Cnt16_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Motor_Left_Decoder_Cnt16_Sleep
********************************************************************************
* Summary:
*     Stop and Save the user configuration
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  Motor_Left_Decoder_Cnt16_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Motor_Left_Decoder_Cnt16_Sleep(void) 
{
    #if(!Motor_Left_Decoder_Cnt16_ControlRegRemoved)
        /* Save Counter's enable state */
        if(Motor_Left_Decoder_Cnt16_CTRL_ENABLE == (Motor_Left_Decoder_Cnt16_CONTROL & Motor_Left_Decoder_Cnt16_CTRL_ENABLE))
        {
            /* Counter is enabled */
            Motor_Left_Decoder_Cnt16_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            Motor_Left_Decoder_Cnt16_backup.CounterEnableState = 0u;
        }
    #else
        Motor_Left_Decoder_Cnt16_backup.CounterEnableState = 1u;
        if(Motor_Left_Decoder_Cnt16_backup.CounterEnableState != 0u)
        {
            Motor_Left_Decoder_Cnt16_backup.CounterEnableState = 0u;
        }
    #endif /* (!Motor_Left_Decoder_Cnt16_ControlRegRemoved) */
    
    Motor_Left_Decoder_Cnt16_Stop();
    Motor_Left_Decoder_Cnt16_SaveConfig();
}


/*******************************************************************************
* Function Name: Motor_Left_Decoder_Cnt16_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*  
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  Motor_Left_Decoder_Cnt16_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Motor_Left_Decoder_Cnt16_Wakeup(void) 
{
    Motor_Left_Decoder_Cnt16_RestoreConfig();
    #if(!Motor_Left_Decoder_Cnt16_ControlRegRemoved)
        if(Motor_Left_Decoder_Cnt16_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            Motor_Left_Decoder_Cnt16_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!Motor_Left_Decoder_Cnt16_ControlRegRemoved) */
    
}


/* [] END OF FILE */
