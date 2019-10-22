/*******************************************************************************
* File Name: Gripper_Servo_PWM_PM.c
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

#include "Gripper_Servo_PWM.h"

static Gripper_Servo_PWM_backupStruct Gripper_Servo_PWM_backup;


/*******************************************************************************
* Function Name: Gripper_Servo_PWM_SaveConfig
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
*  Gripper_Servo_PWM_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Gripper_Servo_PWM_SaveConfig(void) 
{

    #if(!Gripper_Servo_PWM_UsingFixedFunction)
        #if(!Gripper_Servo_PWM_PWMModeIsCenterAligned)
            Gripper_Servo_PWM_backup.PWMPeriod = Gripper_Servo_PWM_ReadPeriod();
        #endif /* (!Gripper_Servo_PWM_PWMModeIsCenterAligned) */
        Gripper_Servo_PWM_backup.PWMUdb = Gripper_Servo_PWM_ReadCounter();
        #if (Gripper_Servo_PWM_UseStatus)
            Gripper_Servo_PWM_backup.InterruptMaskValue = Gripper_Servo_PWM_STATUS_MASK;
        #endif /* (Gripper_Servo_PWM_UseStatus) */

        #if(Gripper_Servo_PWM_DeadBandMode == Gripper_Servo_PWM__B_PWM__DBM_256_CLOCKS || \
            Gripper_Servo_PWM_DeadBandMode == Gripper_Servo_PWM__B_PWM__DBM_2_4_CLOCKS)
            Gripper_Servo_PWM_backup.PWMdeadBandValue = Gripper_Servo_PWM_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(Gripper_Servo_PWM_KillModeMinTime)
             Gripper_Servo_PWM_backup.PWMKillCounterPeriod = Gripper_Servo_PWM_ReadKillTime();
        #endif /* (Gripper_Servo_PWM_KillModeMinTime) */

        #if(Gripper_Servo_PWM_UseControl)
            Gripper_Servo_PWM_backup.PWMControlRegister = Gripper_Servo_PWM_ReadControlRegister();
        #endif /* (Gripper_Servo_PWM_UseControl) */
    #endif  /* (!Gripper_Servo_PWM_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Gripper_Servo_PWM_RestoreConfig
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
*  Gripper_Servo_PWM_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Gripper_Servo_PWM_RestoreConfig(void) 
{
        #if(!Gripper_Servo_PWM_UsingFixedFunction)
            #if(!Gripper_Servo_PWM_PWMModeIsCenterAligned)
                Gripper_Servo_PWM_WritePeriod(Gripper_Servo_PWM_backup.PWMPeriod);
            #endif /* (!Gripper_Servo_PWM_PWMModeIsCenterAligned) */

            Gripper_Servo_PWM_WriteCounter(Gripper_Servo_PWM_backup.PWMUdb);

            #if (Gripper_Servo_PWM_UseStatus)
                Gripper_Servo_PWM_STATUS_MASK = Gripper_Servo_PWM_backup.InterruptMaskValue;
            #endif /* (Gripper_Servo_PWM_UseStatus) */

            #if(Gripper_Servo_PWM_DeadBandMode == Gripper_Servo_PWM__B_PWM__DBM_256_CLOCKS || \
                Gripper_Servo_PWM_DeadBandMode == Gripper_Servo_PWM__B_PWM__DBM_2_4_CLOCKS)
                Gripper_Servo_PWM_WriteDeadTime(Gripper_Servo_PWM_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(Gripper_Servo_PWM_KillModeMinTime)
                Gripper_Servo_PWM_WriteKillTime(Gripper_Servo_PWM_backup.PWMKillCounterPeriod);
            #endif /* (Gripper_Servo_PWM_KillModeMinTime) */

            #if(Gripper_Servo_PWM_UseControl)
                Gripper_Servo_PWM_WriteControlRegister(Gripper_Servo_PWM_backup.PWMControlRegister);
            #endif /* (Gripper_Servo_PWM_UseControl) */
        #endif  /* (!Gripper_Servo_PWM_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: Gripper_Servo_PWM_Sleep
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
*  Gripper_Servo_PWM_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Gripper_Servo_PWM_Sleep(void) 
{
    #if(Gripper_Servo_PWM_UseControl)
        if(Gripper_Servo_PWM_CTRL_ENABLE == (Gripper_Servo_PWM_CONTROL & Gripper_Servo_PWM_CTRL_ENABLE))
        {
            /*Component is enabled */
            Gripper_Servo_PWM_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            Gripper_Servo_PWM_backup.PWMEnableState = 0u;
        }
    #endif /* (Gripper_Servo_PWM_UseControl) */

    /* Stop component */
    Gripper_Servo_PWM_Stop();

    /* Save registers configuration */
    Gripper_Servo_PWM_SaveConfig();
}


/*******************************************************************************
* Function Name: Gripper_Servo_PWM_Wakeup
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
*  Gripper_Servo_PWM_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Gripper_Servo_PWM_Wakeup(void) 
{
     /* Restore registers values */
    Gripper_Servo_PWM_RestoreConfig();

    if(Gripper_Servo_PWM_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        Gripper_Servo_PWM_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
