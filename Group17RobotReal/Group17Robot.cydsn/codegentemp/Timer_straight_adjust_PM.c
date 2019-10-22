/*******************************************************************************
* File Name: Timer_straight_adjust_PM.c
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

#include "Timer_straight_adjust.h"

static Timer_straight_adjust_backupStruct Timer_straight_adjust_backup;


/*******************************************************************************
* Function Name: Timer_straight_adjust_SaveConfig
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
*  Timer_straight_adjust_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_straight_adjust_SaveConfig(void) 
{
    #if (!Timer_straight_adjust_UsingFixedFunction)
        Timer_straight_adjust_backup.TimerUdb = Timer_straight_adjust_ReadCounter();
        Timer_straight_adjust_backup.InterruptMaskValue = Timer_straight_adjust_STATUS_MASK;
        #if (Timer_straight_adjust_UsingHWCaptureCounter)
            Timer_straight_adjust_backup.TimerCaptureCounter = Timer_straight_adjust_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_straight_adjust_UDB_CONTROL_REG_REMOVED)
            Timer_straight_adjust_backup.TimerControlRegister = Timer_straight_adjust_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_straight_adjust_RestoreConfig
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
*  Timer_straight_adjust_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_straight_adjust_RestoreConfig(void) 
{   
    #if (!Timer_straight_adjust_UsingFixedFunction)

        Timer_straight_adjust_WriteCounter(Timer_straight_adjust_backup.TimerUdb);
        Timer_straight_adjust_STATUS_MASK =Timer_straight_adjust_backup.InterruptMaskValue;
        #if (Timer_straight_adjust_UsingHWCaptureCounter)
            Timer_straight_adjust_SetCaptureCount(Timer_straight_adjust_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_straight_adjust_UDB_CONTROL_REG_REMOVED)
            Timer_straight_adjust_WriteControlRegister(Timer_straight_adjust_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_straight_adjust_Sleep
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
*  Timer_straight_adjust_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_straight_adjust_Sleep(void) 
{
    #if(!Timer_straight_adjust_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_straight_adjust_CTRL_ENABLE == (Timer_straight_adjust_CONTROL & Timer_straight_adjust_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_straight_adjust_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_straight_adjust_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_straight_adjust_Stop();
    Timer_straight_adjust_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_straight_adjust_Wakeup
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
*  Timer_straight_adjust_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_straight_adjust_Wakeup(void) 
{
    Timer_straight_adjust_RestoreConfig();
    #if(!Timer_straight_adjust_UDB_CONTROL_REG_REMOVED)
        if(Timer_straight_adjust_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_straight_adjust_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
