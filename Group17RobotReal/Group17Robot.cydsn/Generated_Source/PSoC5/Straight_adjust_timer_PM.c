/*******************************************************************************
* File Name: Straight_adjust_timer_PM.c
* Version 2.80
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Straight_adjust_timer.h"

static Straight_adjust_timer_backupStruct Straight_adjust_timer_backup;


/*******************************************************************************
* Function Name: Straight_adjust_timer_SaveConfig
********************************************************************************
*
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
*  Straight_adjust_timer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Straight_adjust_timer_SaveConfig(void) 
{
    #if (!Straight_adjust_timer_UsingFixedFunction)
        Straight_adjust_timer_backup.TimerUdb = Straight_adjust_timer_ReadCounter();
        Straight_adjust_timer_backup.InterruptMaskValue = Straight_adjust_timer_STATUS_MASK;
        #if (Straight_adjust_timer_UsingHWCaptureCounter)
            Straight_adjust_timer_backup.TimerCaptureCounter = Straight_adjust_timer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Straight_adjust_timer_UDB_CONTROL_REG_REMOVED)
            Straight_adjust_timer_backup.TimerControlRegister = Straight_adjust_timer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Straight_adjust_timer_RestoreConfig
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
*  Straight_adjust_timer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Straight_adjust_timer_RestoreConfig(void) 
{   
    #if (!Straight_adjust_timer_UsingFixedFunction)

        Straight_adjust_timer_WriteCounter(Straight_adjust_timer_backup.TimerUdb);
        Straight_adjust_timer_STATUS_MASK =Straight_adjust_timer_backup.InterruptMaskValue;
        #if (Straight_adjust_timer_UsingHWCaptureCounter)
            Straight_adjust_timer_SetCaptureCount(Straight_adjust_timer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Straight_adjust_timer_UDB_CONTROL_REG_REMOVED)
            Straight_adjust_timer_WriteControlRegister(Straight_adjust_timer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Straight_adjust_timer_Sleep
********************************************************************************
*
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
*  Straight_adjust_timer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Straight_adjust_timer_Sleep(void) 
{
    #if(!Straight_adjust_timer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Straight_adjust_timer_CTRL_ENABLE == (Straight_adjust_timer_CONTROL & Straight_adjust_timer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Straight_adjust_timer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Straight_adjust_timer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Straight_adjust_timer_Stop();
    Straight_adjust_timer_SaveConfig();
}


/*******************************************************************************
* Function Name: Straight_adjust_timer_Wakeup
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
*  Straight_adjust_timer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Straight_adjust_timer_Wakeup(void) 
{
    Straight_adjust_timer_RestoreConfig();
    #if(!Straight_adjust_timer_UDB_CONTROL_REG_REMOVED)
        if(Straight_adjust_timer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Straight_adjust_timer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
