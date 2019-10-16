/*******************************************************************************
* File Name: Motor_Right_Driver_PM.c
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

#include "Motor_Right_Driver.h"

static Motor_Right_Driver_backupStruct Motor_Right_Driver_backup;


/*******************************************************************************
* Function Name: Motor_Right_Driver_SaveConfig
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
*  Motor_Right_Driver_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Motor_Right_Driver_SaveConfig(void) 
{

    #if(!Motor_Right_Driver_UsingFixedFunction)
        #if(!Motor_Right_Driver_PWMModeIsCenterAligned)
            Motor_Right_Driver_backup.PWMPeriod = Motor_Right_Driver_ReadPeriod();
        #endif /* (!Motor_Right_Driver_PWMModeIsCenterAligned) */
        Motor_Right_Driver_backup.PWMUdb = Motor_Right_Driver_ReadCounter();
        #if (Motor_Right_Driver_UseStatus)
            Motor_Right_Driver_backup.InterruptMaskValue = Motor_Right_Driver_STATUS_MASK;
        #endif /* (Motor_Right_Driver_UseStatus) */

        #if(Motor_Right_Driver_DeadBandMode == Motor_Right_Driver__B_PWM__DBM_256_CLOCKS || \
            Motor_Right_Driver_DeadBandMode == Motor_Right_Driver__B_PWM__DBM_2_4_CLOCKS)
            Motor_Right_Driver_backup.PWMdeadBandValue = Motor_Right_Driver_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(Motor_Right_Driver_KillModeMinTime)
             Motor_Right_Driver_backup.PWMKillCounterPeriod = Motor_Right_Driver_ReadKillTime();
        #endif /* (Motor_Right_Driver_KillModeMinTime) */

        #if(Motor_Right_Driver_UseControl)
            Motor_Right_Driver_backup.PWMControlRegister = Motor_Right_Driver_ReadControlRegister();
        #endif /* (Motor_Right_Driver_UseControl) */
    #endif  /* (!Motor_Right_Driver_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Motor_Right_Driver_RestoreConfig
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
*  Motor_Right_Driver_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Motor_Right_Driver_RestoreConfig(void) 
{
        #if(!Motor_Right_Driver_UsingFixedFunction)
            #if(!Motor_Right_Driver_PWMModeIsCenterAligned)
                Motor_Right_Driver_WritePeriod(Motor_Right_Driver_backup.PWMPeriod);
            #endif /* (!Motor_Right_Driver_PWMModeIsCenterAligned) */

            Motor_Right_Driver_WriteCounter(Motor_Right_Driver_backup.PWMUdb);

            #if (Motor_Right_Driver_UseStatus)
                Motor_Right_Driver_STATUS_MASK = Motor_Right_Driver_backup.InterruptMaskValue;
            #endif /* (Motor_Right_Driver_UseStatus) */

            #if(Motor_Right_Driver_DeadBandMode == Motor_Right_Driver__B_PWM__DBM_256_CLOCKS || \
                Motor_Right_Driver_DeadBandMode == Motor_Right_Driver__B_PWM__DBM_2_4_CLOCKS)
                Motor_Right_Driver_WriteDeadTime(Motor_Right_Driver_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(Motor_Right_Driver_KillModeMinTime)
                Motor_Right_Driver_WriteKillTime(Motor_Right_Driver_backup.PWMKillCounterPeriod);
            #endif /* (Motor_Right_Driver_KillModeMinTime) */

            #if(Motor_Right_Driver_UseControl)
                Motor_Right_Driver_WriteControlRegister(Motor_Right_Driver_backup.PWMControlRegister);
            #endif /* (Motor_Right_Driver_UseControl) */
        #endif  /* (!Motor_Right_Driver_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: Motor_Right_Driver_Sleep
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
*  Motor_Right_Driver_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Motor_Right_Driver_Sleep(void) 
{
    #if(Motor_Right_Driver_UseControl)
        if(Motor_Right_Driver_CTRL_ENABLE == (Motor_Right_Driver_CONTROL & Motor_Right_Driver_CTRL_ENABLE))
        {
            /*Component is enabled */
            Motor_Right_Driver_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            Motor_Right_Driver_backup.PWMEnableState = 0u;
        }
    #endif /* (Motor_Right_Driver_UseControl) */

    /* Stop component */
    Motor_Right_Driver_Stop();

    /* Save registers configuration */
    Motor_Right_Driver_SaveConfig();
}


/*******************************************************************************
* Function Name: Motor_Right_Driver_Wakeup
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
*  Motor_Right_Driver_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Motor_Right_Driver_Wakeup(void) 
{
     /* Restore registers values */
    Motor_Right_Driver_RestoreConfig();

    if(Motor_Right_Driver_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        Motor_Right_Driver_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
