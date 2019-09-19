/*******************************************************************************
* File Name: control_photodiode_PM.c
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

#include "control_photodiode.h"

/* Check for removal by optimization */
#if !defined(control_photodiode_Sync_ctrl_reg__REMOVED)

static control_photodiode_BACKUP_STRUCT  control_photodiode_backup = {0u};

    
/*******************************************************************************
* Function Name: control_photodiode_SaveConfig
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
void control_photodiode_SaveConfig(void) 
{
    control_photodiode_backup.controlState = control_photodiode_Control;
}


/*******************************************************************************
* Function Name: control_photodiode_RestoreConfig
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
void control_photodiode_RestoreConfig(void) 
{
     control_photodiode_Control = control_photodiode_backup.controlState;
}


/*******************************************************************************
* Function Name: control_photodiode_Sleep
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
void control_photodiode_Sleep(void) 
{
    control_photodiode_SaveConfig();
}


/*******************************************************************************
* Function Name: control_photodiode_Wakeup
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
void control_photodiode_Wakeup(void)  
{
    control_photodiode_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
