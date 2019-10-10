/*******************************************************************************
* File Name: Drift_Check_Timer_PM.c
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

#include "Drift_Check_Timer.h"

static Drift_Check_Timer_backupStruct Drift_Check_Timer_backup;


/*******************************************************************************
* Function Name: Drift_Check_Timer_SaveConfig
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
*  Drift_Check_Timer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Drift_Check_Timer_SaveConfig(void) 
{
    #if (!Drift_Check_Timer_UsingFixedFunction)
        Drift_Check_Timer_backup.TimerUdb = Drift_Check_Timer_ReadCounter();
        Drift_Check_Timer_backup.InterruptMaskValue = Drift_Check_Timer_STATUS_MASK;
        #if (Drift_Check_Timer_UsingHWCaptureCounter)
            Drift_Check_Timer_backup.TimerCaptureCounter = Drift_Check_Timer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Drift_Check_Timer_UDB_CONTROL_REG_REMOVED)
            Drift_Check_Timer_backup.TimerControlRegister = Drift_Check_Timer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Drift_Check_Timer_RestoreConfig
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
*  Drift_Check_Timer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Drift_Check_Timer_RestoreConfig(void) 
{   
    #if (!Drift_Check_Timer_UsingFixedFunction)

        Drift_Check_Timer_WriteCounter(Drift_Check_Timer_backup.TimerUdb);
        Drift_Check_Timer_STATUS_MASK =Drift_Check_Timer_backup.InterruptMaskValue;
        #if (Drift_Check_Timer_UsingHWCaptureCounter)
            Drift_Check_Timer_SetCaptureCount(Drift_Check_Timer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Drift_Check_Timer_UDB_CONTROL_REG_REMOVED)
            Drift_Check_Timer_WriteControlRegister(Drift_Check_Timer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Drift_Check_Timer_Sleep
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
*  Drift_Check_Timer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Drift_Check_Timer_Sleep(void) 
{
    #if(!Drift_Check_Timer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Drift_Check_Timer_CTRL_ENABLE == (Drift_Check_Timer_CONTROL & Drift_Check_Timer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Drift_Check_Timer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Drift_Check_Timer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Drift_Check_Timer_Stop();
    Drift_Check_Timer_SaveConfig();
}


/*******************************************************************************
* Function Name: Drift_Check_Timer_Wakeup
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
*  Drift_Check_Timer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Drift_Check_Timer_Wakeup(void) 
{
    Drift_Check_Timer_RestoreConfig();
    #if(!Drift_Check_Timer_UDB_CONTROL_REG_REMOVED)
        if(Drift_Check_Timer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Drift_Check_Timer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
