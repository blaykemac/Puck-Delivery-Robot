/*******************************************************************************
* File Name: control_led_PM.c
* Version 1.80
*
* Description:
*  This file contains the setup, control, and status commands to support 
*  the component operation in the low power mode. 
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "control_led.h"

/* Check for removal by optimization */
#if !defined(control_led_Sync_ctrl_reg__REMOVED)

static control_led_BACKUP_STRUCT  control_led_backup = {0u};

    
/*******************************************************************************
* Function Name: control_led_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void control_led_SaveConfig(void) 
{
    control_led_backup.controlState = control_led_Control;
}


/*******************************************************************************
* Function Name: control_led_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*
*******************************************************************************/
void control_led_RestoreConfig(void) 
{
     control_led_Control = control_led_backup.controlState;
}


/*******************************************************************************
* Function Name: control_led_Sleep
********************************************************************************
*
* Summary:
*  Prepares the component for entering the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void control_led_Sleep(void) 
{
    control_led_SaveConfig();
}


/*******************************************************************************
* Function Name: control_led_Wakeup
********************************************************************************
*
* Summary:
*  Restores the component after waking up from the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void control_led_Wakeup(void)  
{
    control_led_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
