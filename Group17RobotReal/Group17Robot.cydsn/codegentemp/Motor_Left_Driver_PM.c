/*******************************************************************************
* File Name: Motor_Left_Driver_PM.c
* Version 3.30
*
* Description:
*  This file provides the power management source code to API for the
*  PWM.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Motor_Left_Driver.h"

static Motor_Left_Driver_backupStruct Motor_Left_Driver_backup;


/*******************************************************************************
* Function Name: Motor_Left_Driver_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  Motor_Left_Driver_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Motor_Left_Driver_SaveConfig(void) 
{

    #if(!Motor_Left_Driver_UsingFixedFunction)
        #if(!Motor_Left_Driver_PWMModeIsCenterAligned)
            Motor_Left_Driver_backup.PWMPeriod = Motor_Left_Driver_ReadPeriod();
        #endif /* (!Motor_Left_Driver_PWMModeIsCenterAligned) */
        Motor_Left_Driver_backup.PWMUdb = Motor_Left_Driver_ReadCounter();
        #if (Motor_Left_Driver_UseStatus)
            Motor_Left_Driver_backup.InterruptMaskValue = Motor_Left_Driver_STATUS_MASK;
        #endif /* (Motor_Left_Driver_UseStatus) */

        #if(Motor_Left_Driver_DeadBandMode == Motor_Left_Driver__B_PWM__DBM_256_CLOCKS || \
            Motor_Left_Driver_DeadBandMode == Motor_Left_Driver__B_PWM__DBM_2_4_CLOCKS)
            Motor_Left_Driver_backup.PWMdeadBandValue = Motor_Left_Driver_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(Motor_Left_Driver_KillModeMinTime)
             Motor_Left_Driver_backup.PWMKillCounterPeriod = Motor_Left_Driver_ReadKillTime();
        #endif /* (Motor_Left_Driver_KillModeMinTime) */

        #if(Motor_Left_Driver_UseControl)
            Motor_Left_Driver_backup.PWMControlRegister = Motor_Left_Driver_ReadControlRegister();
        #endif /* (Motor_Left_Driver_UseControl) */
    #endif  /* (!Motor_Left_Driver_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Motor_Left_Driver_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  Motor_Left_Driver_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Motor_Left_Driver_RestoreConfig(void) 
{
        #if(!Motor_Left_Driver_UsingFixedFunction)
            #if(!Motor_Left_Driver_PWMModeIsCenterAligned)
                Motor_Left_Driver_WritePeriod(Motor_Left_Driver_backup.PWMPeriod);
            #endif /* (!Motor_Left_Driver_PWMModeIsCenterAligned) */

            Motor_Left_Driver_WriteCounter(Motor_Left_Driver_backup.PWMUdb);

            #if (Motor_Left_Driver_UseStatus)
                Motor_Left_Driver_STATUS_MASK = Motor_Left_Driver_backup.InterruptMaskValue;
            #endif /* (Motor_Left_Driver_UseStatus) */

            #if(Motor_Left_Driver_DeadBandMode == Motor_Left_Driver__B_PWM__DBM_256_CLOCKS || \
                Motor_Left_Driver_DeadBandMode == Motor_Left_Driver__B_PWM__DBM_2_4_CLOCKS)
                Motor_Left_Driver_WriteDeadTime(Motor_Left_Driver_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(Motor_Left_Driver_KillModeMinTime)
                Motor_Left_Driver_WriteKillTime(Motor_Left_Driver_backup.PWMKillCounterPeriod);
            #endif /* (Motor_Left_Driver_KillModeMinTime) */

            #if(Motor_Left_Driver_UseControl)
                Motor_Left_Driver_WriteControlRegister(Motor_Left_Driver_backup.PWMControlRegister);
            #endif /* (Motor_Left_Driver_UseControl) */
        #endif  /* (!Motor_Left_Driver_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: Motor_Left_Driver_Sleep
********************************************************************************
*
* Summary:
*  Disables block's operation and saves the user configuration. Should be called
*  just prior to entering sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  Motor_Left_Driver_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Motor_Left_Driver_Sleep(void) 
{
    #if(Motor_Left_Driver_UseControl)
        if(Motor_Left_Driver_CTRL_ENABLE == (Motor_Left_Driver_CONTROL & Motor_Left_Driver_CTRL_ENABLE))
        {
            /*Component is enabled */
            Motor_Left_Driver_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            Motor_Left_Driver_backup.PWMEnableState = 0u;
        }
    #endif /* (Motor_Left_Driver_UseControl) */

    /* Stop component */
    Motor_Left_Driver_Stop();

    /* Save registers configuration */
    Motor_Left_Driver_SaveConfig();
}


/*******************************************************************************
* Function Name: Motor_Left_Driver_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration. Should be called just after
*  awaking from sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  Motor_Left_Driver_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Motor_Left_Driver_Wakeup(void) 
{
     /* Restore registers values */
    Motor_Left_Driver_RestoreConfig();

    if(Motor_Left_Driver_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        Motor_Left_Driver_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
