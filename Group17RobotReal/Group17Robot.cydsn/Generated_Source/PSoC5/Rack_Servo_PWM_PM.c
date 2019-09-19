/*******************************************************************************
* File Name: Rack_Servo_PWM_PM.c
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

#include "Rack_Servo_PWM.h"

static Rack_Servo_PWM_backupStruct Rack_Servo_PWM_backup;


/*******************************************************************************
* Function Name: Rack_Servo_PWM_SaveConfig
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
*  Rack_Servo_PWM_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Rack_Servo_PWM_SaveConfig(void) 
{

    #if(!Rack_Servo_PWM_UsingFixedFunction)
        #if(!Rack_Servo_PWM_PWMModeIsCenterAligned)
            Rack_Servo_PWM_backup.PWMPeriod = Rack_Servo_PWM_ReadPeriod();
        #endif /* (!Rack_Servo_PWM_PWMModeIsCenterAligned) */
        Rack_Servo_PWM_backup.PWMUdb = Rack_Servo_PWM_ReadCounter();
        #if (Rack_Servo_PWM_UseStatus)
            Rack_Servo_PWM_backup.InterruptMaskValue = Rack_Servo_PWM_STATUS_MASK;
        #endif /* (Rack_Servo_PWM_UseStatus) */

        #if(Rack_Servo_PWM_DeadBandMode == Rack_Servo_PWM__B_PWM__DBM_256_CLOCKS || \
            Rack_Servo_PWM_DeadBandMode == Rack_Servo_PWM__B_PWM__DBM_2_4_CLOCKS)
            Rack_Servo_PWM_backup.PWMdeadBandValue = Rack_Servo_PWM_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(Rack_Servo_PWM_KillModeMinTime)
             Rack_Servo_PWM_backup.PWMKillCounterPeriod = Rack_Servo_PWM_ReadKillTime();
        #endif /* (Rack_Servo_PWM_KillModeMinTime) */

        #if(Rack_Servo_PWM_UseControl)
            Rack_Servo_PWM_backup.PWMControlRegister = Rack_Servo_PWM_ReadControlRegister();
        #endif /* (Rack_Servo_PWM_UseControl) */
    #endif  /* (!Rack_Servo_PWM_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Rack_Servo_PWM_RestoreConfig
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
*  Rack_Servo_PWM_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Rack_Servo_PWM_RestoreConfig(void) 
{
        #if(!Rack_Servo_PWM_UsingFixedFunction)
            #if(!Rack_Servo_PWM_PWMModeIsCenterAligned)
                Rack_Servo_PWM_WritePeriod(Rack_Servo_PWM_backup.PWMPeriod);
            #endif /* (!Rack_Servo_PWM_PWMModeIsCenterAligned) */

            Rack_Servo_PWM_WriteCounter(Rack_Servo_PWM_backup.PWMUdb);

            #if (Rack_Servo_PWM_UseStatus)
                Rack_Servo_PWM_STATUS_MASK = Rack_Servo_PWM_backup.InterruptMaskValue;
            #endif /* (Rack_Servo_PWM_UseStatus) */

            #if(Rack_Servo_PWM_DeadBandMode == Rack_Servo_PWM__B_PWM__DBM_256_CLOCKS || \
                Rack_Servo_PWM_DeadBandMode == Rack_Servo_PWM__B_PWM__DBM_2_4_CLOCKS)
                Rack_Servo_PWM_WriteDeadTime(Rack_Servo_PWM_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(Rack_Servo_PWM_KillModeMinTime)
                Rack_Servo_PWM_WriteKillTime(Rack_Servo_PWM_backup.PWMKillCounterPeriod);
            #endif /* (Rack_Servo_PWM_KillModeMinTime) */

            #if(Rack_Servo_PWM_UseControl)
                Rack_Servo_PWM_WriteControlRegister(Rack_Servo_PWM_backup.PWMControlRegister);
            #endif /* (Rack_Servo_PWM_UseControl) */
        #endif  /* (!Rack_Servo_PWM_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: Rack_Servo_PWM_Sleep
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
*  Rack_Servo_PWM_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Rack_Servo_PWM_Sleep(void) 
{
    #if(Rack_Servo_PWM_UseControl)
        if(Rack_Servo_PWM_CTRL_ENABLE == (Rack_Servo_PWM_CONTROL & Rack_Servo_PWM_CTRL_ENABLE))
        {
            /*Component is enabled */
            Rack_Servo_PWM_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            Rack_Servo_PWM_backup.PWMEnableState = 0u;
        }
    #endif /* (Rack_Servo_PWM_UseControl) */

    /* Stop component */
    Rack_Servo_PWM_Stop();

    /* Save registers configuration */
    Rack_Servo_PWM_SaveConfig();
}


/*******************************************************************************
* Function Name: Rack_Servo_PWM_Wakeup
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
*  Rack_Servo_PWM_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Rack_Servo_PWM_Wakeup(void) 
{
     /* Restore registers values */
    Rack_Servo_PWM_RestoreConfig();

    if(Rack_Servo_PWM_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        Rack_Servo_PWM_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
