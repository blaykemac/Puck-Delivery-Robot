/*******************************************************************************
* File Name: Motor_2_driver_PM.c
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

#include "Motor_2_driver.h"

static Motor_2_driver_backupStruct Motor_2_driver_backup;


/*******************************************************************************
* Function Name: Motor_2_driver_SaveConfig
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
*  Motor_2_driver_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Motor_2_driver_SaveConfig(void) 
{

    #if(!Motor_2_driver_UsingFixedFunction)
        #if(!Motor_2_driver_PWMModeIsCenterAligned)
            Motor_2_driver_backup.PWMPeriod = Motor_2_driver_ReadPeriod();
        #endif /* (!Motor_2_driver_PWMModeIsCenterAligned) */
        Motor_2_driver_backup.PWMUdb = Motor_2_driver_ReadCounter();
        #if (Motor_2_driver_UseStatus)
            Motor_2_driver_backup.InterruptMaskValue = Motor_2_driver_STATUS_MASK;
        #endif /* (Motor_2_driver_UseStatus) */

        #if(Motor_2_driver_DeadBandMode == Motor_2_driver__B_PWM__DBM_256_CLOCKS || \
            Motor_2_driver_DeadBandMode == Motor_2_driver__B_PWM__DBM_2_4_CLOCKS)
            Motor_2_driver_backup.PWMdeadBandValue = Motor_2_driver_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(Motor_2_driver_KillModeMinTime)
             Motor_2_driver_backup.PWMKillCounterPeriod = Motor_2_driver_ReadKillTime();
        #endif /* (Motor_2_driver_KillModeMinTime) */

        #if(Motor_2_driver_UseControl)
            Motor_2_driver_backup.PWMControlRegister = Motor_2_driver_ReadControlRegister();
        #endif /* (Motor_2_driver_UseControl) */
    #endif  /* (!Motor_2_driver_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Motor_2_driver_RestoreConfig
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
*  Motor_2_driver_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Motor_2_driver_RestoreConfig(void) 
{
        #if(!Motor_2_driver_UsingFixedFunction)
            #if(!Motor_2_driver_PWMModeIsCenterAligned)
                Motor_2_driver_WritePeriod(Motor_2_driver_backup.PWMPeriod);
            #endif /* (!Motor_2_driver_PWMModeIsCenterAligned) */

            Motor_2_driver_WriteCounter(Motor_2_driver_backup.PWMUdb);

            #if (Motor_2_driver_UseStatus)
                Motor_2_driver_STATUS_MASK = Motor_2_driver_backup.InterruptMaskValue;
            #endif /* (Motor_2_driver_UseStatus) */

            #if(Motor_2_driver_DeadBandMode == Motor_2_driver__B_PWM__DBM_256_CLOCKS || \
                Motor_2_driver_DeadBandMode == Motor_2_driver__B_PWM__DBM_2_4_CLOCKS)
                Motor_2_driver_WriteDeadTime(Motor_2_driver_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(Motor_2_driver_KillModeMinTime)
                Motor_2_driver_WriteKillTime(Motor_2_driver_backup.PWMKillCounterPeriod);
            #endif /* (Motor_2_driver_KillModeMinTime) */

            #if(Motor_2_driver_UseControl)
                Motor_2_driver_WriteControlRegister(Motor_2_driver_backup.PWMControlRegister);
            #endif /* (Motor_2_driver_UseControl) */
        #endif  /* (!Motor_2_driver_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: Motor_2_driver_Sleep
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
*  Motor_2_driver_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Motor_2_driver_Sleep(void) 
{
    #if(Motor_2_driver_UseControl)
        if(Motor_2_driver_CTRL_ENABLE == (Motor_2_driver_CONTROL & Motor_2_driver_CTRL_ENABLE))
        {
            /*Component is enabled */
            Motor_2_driver_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            Motor_2_driver_backup.PWMEnableState = 0u;
        }
    #endif /* (Motor_2_driver_UseControl) */

    /* Stop component */
    Motor_2_driver_Stop();

    /* Save registers configuration */
    Motor_2_driver_SaveConfig();
}


/*******************************************************************************
* Function Name: Motor_2_driver_Wakeup
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
*  Motor_2_driver_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Motor_2_driver_Wakeup(void) 
{
     /* Restore registers values */
    Motor_2_driver_RestoreConfig();

    if(Motor_2_driver_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        Motor_2_driver_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
