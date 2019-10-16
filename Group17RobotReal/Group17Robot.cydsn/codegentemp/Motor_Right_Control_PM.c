/*******************************************************************************
* File Name: Motor_Right_Control_PM.c
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

#include "Motor_Right_Control.h"

/* Check for removal by optimization */
#if !defined(Motor_Right_Control_Sync_ctrl_reg__REMOVED)

static Motor_Right_Control_BACKUP_STRUCT  Motor_Right_Control_backup = {0u};

    
/*******************************************************************************
* Function Name: Motor_Right_Control_SaveConfig
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
void Motor_Right_Control_SaveConfig(void) 
{
    Motor_Right_Control_backup.controlState = Motor_Right_Control_Control;
}


/*******************************************************************************
* Function Name: Motor_Right_Control_RestoreConfig
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
void Motor_Right_Control_RestoreConfig(void) 
{
     Motor_Right_Control_Control = Motor_Right_Control_backup.controlState;
}


/*******************************************************************************
* Function Name: Motor_Right_Control_Sleep
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
void Motor_Right_Control_Sleep(void) 
{
    Motor_Right_Control_SaveConfig();
}


/*******************************************************************************
* Function Name: Motor_Right_Control_Wakeup
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
void Motor_Right_Control_Wakeup(void)  
{
    Motor_Right_Control_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
