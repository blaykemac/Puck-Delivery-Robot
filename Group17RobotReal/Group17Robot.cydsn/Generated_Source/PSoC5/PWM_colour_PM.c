/*******************************************************************************
* File Name: PWM_colour_PM.c
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

#include "PWM_colour.h"

static PWM_colour_backupStruct PWM_colour_backup;


/*******************************************************************************
* Function Name: PWM_colour_SaveConfig
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
*  PWM_colour_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void PWM_colour_SaveConfig(void) 
{

    #if(!PWM_colour_UsingFixedFunction)
        #if(!PWM_colour_PWMModeIsCenterAligned)
            PWM_colour_backup.PWMPeriod = PWM_colour_ReadPeriod();
        #endif /* (!PWM_colour_PWMModeIsCenterAligned) */
        PWM_colour_backup.PWMUdb = PWM_colour_ReadCounter();
        #if (PWM_colour_UseStatus)
            PWM_colour_backup.InterruptMaskValue = PWM_colour_STATUS_MASK;
        #endif /* (PWM_colour_UseStatus) */

        #if(PWM_colour_DeadBandMode == PWM_colour__B_PWM__DBM_256_CLOCKS || \
            PWM_colour_DeadBandMode == PWM_colour__B_PWM__DBM_2_4_CLOCKS)
            PWM_colour_backup.PWMdeadBandValue = PWM_colour_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(PWM_colour_KillModeMinTime)
             PWM_colour_backup.PWMKillCounterPeriod = PWM_colour_ReadKillTime();
        #endif /* (PWM_colour_KillModeMinTime) */

        #if(PWM_colour_UseControl)
            PWM_colour_backup.PWMControlRegister = PWM_colour_ReadControlRegister();
        #endif /* (PWM_colour_UseControl) */
    #endif  /* (!PWM_colour_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: PWM_colour_RestoreConfig
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
*  PWM_colour_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void PWM_colour_RestoreConfig(void) 
{
        #if(!PWM_colour_UsingFixedFunction)
            #if(!PWM_colour_PWMModeIsCenterAligned)
                PWM_colour_WritePeriod(PWM_colour_backup.PWMPeriod);
            #endif /* (!PWM_colour_PWMModeIsCenterAligned) */

            PWM_colour_WriteCounter(PWM_colour_backup.PWMUdb);

            #if (PWM_colour_UseStatus)
                PWM_colour_STATUS_MASK = PWM_colour_backup.InterruptMaskValue;
            #endif /* (PWM_colour_UseStatus) */

            #if(PWM_colour_DeadBandMode == PWM_colour__B_PWM__DBM_256_CLOCKS || \
                PWM_colour_DeadBandMode == PWM_colour__B_PWM__DBM_2_4_CLOCKS)
                PWM_colour_WriteDeadTime(PWM_colour_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(PWM_colour_KillModeMinTime)
                PWM_colour_WriteKillTime(PWM_colour_backup.PWMKillCounterPeriod);
            #endif /* (PWM_colour_KillModeMinTime) */

            #if(PWM_colour_UseControl)
                PWM_colour_WriteControlRegister(PWM_colour_backup.PWMControlRegister);
            #endif /* (PWM_colour_UseControl) */
        #endif  /* (!PWM_colour_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: PWM_colour_Sleep
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
*  PWM_colour_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void PWM_colour_Sleep(void) 
{
    #if(PWM_colour_UseControl)
        if(PWM_colour_CTRL_ENABLE == (PWM_colour_CONTROL & PWM_colour_CTRL_ENABLE))
        {
            /*Component is enabled */
            PWM_colour_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            PWM_colour_backup.PWMEnableState = 0u;
        }
    #endif /* (PWM_colour_UseControl) */

    /* Stop component */
    PWM_colour_Stop();

    /* Save registers configuration */
    PWM_colour_SaveConfig();
}


/*******************************************************************************
* Function Name: PWM_colour_Wakeup
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
*  PWM_colour_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void PWM_colour_Wakeup(void) 
{
     /* Restore registers values */
    PWM_colour_RestoreConfig();

    if(PWM_colour_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        PWM_colour_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
