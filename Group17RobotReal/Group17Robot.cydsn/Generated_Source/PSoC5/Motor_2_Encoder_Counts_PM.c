/*******************************************************************************
* File Name: Motor_2_Encoder_Counts_PM.c  
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

#include "Motor_2_Encoder_Counts.h"

static Motor_2_Encoder_Counts_backupStruct Motor_2_Encoder_Counts_backup;


/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_SaveConfig
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
*  Motor_2_Encoder_Counts_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void Motor_2_Encoder_Counts_SaveConfig(void) 
{
    #if (!Motor_2_Encoder_Counts_UsingFixedFunction)

        Motor_2_Encoder_Counts_backup.CounterUdb = Motor_2_Encoder_Counts_ReadCounter();

        #if(!Motor_2_Encoder_Counts_ControlRegRemoved)
            Motor_2_Encoder_Counts_backup.CounterControlRegister = Motor_2_Encoder_Counts_ReadControlRegister();
        #endif /* (!Motor_2_Encoder_Counts_ControlRegRemoved) */

    #endif /* (!Motor_2_Encoder_Counts_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_RestoreConfig
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
*  Motor_2_Encoder_Counts_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Motor_2_Encoder_Counts_RestoreConfig(void) 
{      
    #if (!Motor_2_Encoder_Counts_UsingFixedFunction)

       Motor_2_Encoder_Counts_WriteCounter(Motor_2_Encoder_Counts_backup.CounterUdb);

        #if(!Motor_2_Encoder_Counts_ControlRegRemoved)
            Motor_2_Encoder_Counts_WriteControlRegister(Motor_2_Encoder_Counts_backup.CounterControlRegister);
        #endif /* (!Motor_2_Encoder_Counts_ControlRegRemoved) */

    #endif /* (!Motor_2_Encoder_Counts_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_Sleep
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
*  Motor_2_Encoder_Counts_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Motor_2_Encoder_Counts_Sleep(void) 
{
    #if(!Motor_2_Encoder_Counts_ControlRegRemoved)
        /* Save Counter's enable state */
        if(Motor_2_Encoder_Counts_CTRL_ENABLE == (Motor_2_Encoder_Counts_CONTROL & Motor_2_Encoder_Counts_CTRL_ENABLE))
        {
            /* Counter is enabled */
            Motor_2_Encoder_Counts_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            Motor_2_Encoder_Counts_backup.CounterEnableState = 0u;
        }
    #else
        Motor_2_Encoder_Counts_backup.CounterEnableState = 1u;
        if(Motor_2_Encoder_Counts_backup.CounterEnableState != 0u)
        {
            Motor_2_Encoder_Counts_backup.CounterEnableState = 0u;
        }
    #endif /* (!Motor_2_Encoder_Counts_ControlRegRemoved) */
    
    Motor_2_Encoder_Counts_Stop();
    Motor_2_Encoder_Counts_SaveConfig();
}


/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_Wakeup
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
*  Motor_2_Encoder_Counts_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Motor_2_Encoder_Counts_Wakeup(void) 
{
    Motor_2_Encoder_Counts_RestoreConfig();
    #if(!Motor_2_Encoder_Counts_ControlRegRemoved)
        if(Motor_2_Encoder_Counts_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            Motor_2_Encoder_Counts_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!Motor_2_Encoder_Counts_ControlRegRemoved) */
    
}


/* [] END OF FILE */
