/*******************************************************************************
* File Name: Motor_2_Encoder_Counts.c  
* Version 3.0
*
*  Description:
*     The Counter component consists of a 8, 16, 24 or 32-bit counter with
*     a selectable period between 2 and 2^Width - 1.  
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "Motor_2_Encoder_Counts.h"

uint8 Motor_2_Encoder_Counts_initVar = 0u;


/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_Init
********************************************************************************
* Summary:
*     Initialize to the schematic state
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void Motor_2_Encoder_Counts_Init(void) 
{
        #if (!Motor_2_Encoder_Counts_UsingFixedFunction && !Motor_2_Encoder_Counts_ControlRegRemoved)
            uint8 ctrl;
        #endif /* (!Motor_2_Encoder_Counts_UsingFixedFunction && !Motor_2_Encoder_Counts_ControlRegRemoved) */
        
        #if(!Motor_2_Encoder_Counts_UsingFixedFunction) 
            /* Interrupt State Backup for Critical Region*/
            uint8 Motor_2_Encoder_Counts_interruptState;
        #endif /* (!Motor_2_Encoder_Counts_UsingFixedFunction) */
        
        #if (Motor_2_Encoder_Counts_UsingFixedFunction)
            /* Clear all bits but the enable bit (if it's already set for Timer operation */
            Motor_2_Encoder_Counts_CONTROL &= Motor_2_Encoder_Counts_CTRL_ENABLE;
            
            /* Clear the mode bits for continuous run mode */
            #if (CY_PSOC5A)
                Motor_2_Encoder_Counts_CONTROL2 &= ((uint8)(~Motor_2_Encoder_Counts_CTRL_MODE_MASK));
            #endif /* (CY_PSOC5A) */
            #if (CY_PSOC3 || CY_PSOC5LP)
                Motor_2_Encoder_Counts_CONTROL3 &= ((uint8)(~Motor_2_Encoder_Counts_CTRL_MODE_MASK));                
            #endif /* (CY_PSOC3 || CY_PSOC5LP) */
            /* Check if One Shot mode is enabled i.e. RunMode !=0*/
            #if (Motor_2_Encoder_Counts_RunModeUsed != 0x0u)
                /* Set 3rd bit of Control register to enable one shot mode */
                Motor_2_Encoder_Counts_CONTROL |= Motor_2_Encoder_Counts_ONESHOT;
            #endif /* (Motor_2_Encoder_Counts_RunModeUsed != 0x0u) */
            
            /* Set the IRQ to use the status register interrupts */
            Motor_2_Encoder_Counts_CONTROL2 |= Motor_2_Encoder_Counts_CTRL2_IRQ_SEL;
            
            /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
            Motor_2_Encoder_Counts_RT1 &= ((uint8)(~Motor_2_Encoder_Counts_RT1_MASK));
            Motor_2_Encoder_Counts_RT1 |= Motor_2_Encoder_Counts_SYNC;     
                    
            /*Enable DSI Sync all all inputs of the Timer*/
            Motor_2_Encoder_Counts_RT1 &= ((uint8)(~Motor_2_Encoder_Counts_SYNCDSI_MASK));
            Motor_2_Encoder_Counts_RT1 |= Motor_2_Encoder_Counts_SYNCDSI_EN;

        #else
            #if(!Motor_2_Encoder_Counts_ControlRegRemoved)
            /* Set the default compare mode defined in the parameter */
            ctrl = Motor_2_Encoder_Counts_CONTROL & ((uint8)(~Motor_2_Encoder_Counts_CTRL_CMPMODE_MASK));
            Motor_2_Encoder_Counts_CONTROL = ctrl | Motor_2_Encoder_Counts_DEFAULT_COMPARE_MODE;
            
            /* Set the default capture mode defined in the parameter */
            ctrl = Motor_2_Encoder_Counts_CONTROL & ((uint8)(~Motor_2_Encoder_Counts_CTRL_CAPMODE_MASK));
            
            #if( 0 != Motor_2_Encoder_Counts_CAPTURE_MODE_CONF)
                Motor_2_Encoder_Counts_CONTROL = ctrl | Motor_2_Encoder_Counts_DEFAULT_CAPTURE_MODE;
            #else
                Motor_2_Encoder_Counts_CONTROL = ctrl;
            #endif /* 0 != Motor_2_Encoder_Counts_CAPTURE_MODE */ 
            
            #endif /* (!Motor_2_Encoder_Counts_ControlRegRemoved) */
        #endif /* (Motor_2_Encoder_Counts_UsingFixedFunction) */
        
        /* Clear all data in the FIFO's */
        #if (!Motor_2_Encoder_Counts_UsingFixedFunction)
            Motor_2_Encoder_Counts_ClearFIFO();
        #endif /* (!Motor_2_Encoder_Counts_UsingFixedFunction) */
        
        /* Set Initial values from Configuration */
        Motor_2_Encoder_Counts_WritePeriod(Motor_2_Encoder_Counts_INIT_PERIOD_VALUE);
        #if (!(Motor_2_Encoder_Counts_UsingFixedFunction && (CY_PSOC5A)))
            Motor_2_Encoder_Counts_WriteCounter(Motor_2_Encoder_Counts_INIT_COUNTER_VALUE);
        #endif /* (!(Motor_2_Encoder_Counts_UsingFixedFunction && (CY_PSOC5A))) */
        Motor_2_Encoder_Counts_SetInterruptMode(Motor_2_Encoder_Counts_INIT_INTERRUPTS_MASK);
        
        #if (!Motor_2_Encoder_Counts_UsingFixedFunction)
            /* Read the status register to clear the unwanted interrupts */
            (void)Motor_2_Encoder_Counts_ReadStatusRegister();
            /* Set the compare value (only available to non-fixed function implementation */
            Motor_2_Encoder_Counts_WriteCompare(Motor_2_Encoder_Counts_INIT_COMPARE_VALUE);
            /* Use the interrupt output of the status register for IRQ output */
            
            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            Motor_2_Encoder_Counts_interruptState = CyEnterCriticalSection();
            
            Motor_2_Encoder_Counts_STATUS_AUX_CTRL |= Motor_2_Encoder_Counts_STATUS_ACTL_INT_EN_MASK;
            
            /* Exit Critical Region*/
            CyExitCriticalSection(Motor_2_Encoder_Counts_interruptState);
            
        #endif /* (!Motor_2_Encoder_Counts_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_Enable
********************************************************************************
* Summary:
*     Enable the Counter
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: 
*   If the Enable mode is set to Hardware only then this function has no effect 
*   on the operation of the counter.
*
*******************************************************************************/
void Motor_2_Encoder_Counts_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (Motor_2_Encoder_Counts_UsingFixedFunction)
        Motor_2_Encoder_Counts_GLOBAL_ENABLE |= Motor_2_Encoder_Counts_BLOCK_EN_MASK;
        Motor_2_Encoder_Counts_GLOBAL_STBY_ENABLE |= Motor_2_Encoder_Counts_BLOCK_STBY_EN_MASK;
    #endif /* (Motor_2_Encoder_Counts_UsingFixedFunction) */  
        
    /* Enable the counter from the control register  */
    /* If Fixed Function then make sure Mode is set correctly */
    /* else make sure reset is clear */
    #if(!Motor_2_Encoder_Counts_ControlRegRemoved || Motor_2_Encoder_Counts_UsingFixedFunction)
        Motor_2_Encoder_Counts_CONTROL |= Motor_2_Encoder_Counts_CTRL_ENABLE;                
    #endif /* (!Motor_2_Encoder_Counts_ControlRegRemoved || Motor_2_Encoder_Counts_UsingFixedFunction) */
    
}


/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_Start
********************************************************************************
* Summary:
*  Enables the counter for operation 
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Global variables:
*  Motor_2_Encoder_Counts_initVar: Is modified when this function is called for the  
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void Motor_2_Encoder_Counts_Start(void) 
{
    if(Motor_2_Encoder_Counts_initVar == 0u)
    {
        Motor_2_Encoder_Counts_Init();
        
        Motor_2_Encoder_Counts_initVar = 1u; /* Clear this bit for Initialization */        
    }
    
    /* Enable the Counter */
    Motor_2_Encoder_Counts_Enable();        
}


/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_Stop
********************************************************************************
* Summary:
* Halts the counter, but does not change any modes or disable interrupts.
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: If the Enable mode is set to Hardware only then this function
*               has no effect on the operation of the counter.
*
*******************************************************************************/
void Motor_2_Encoder_Counts_Stop(void) 
{
    /* Disable Counter */
    #if(!Motor_2_Encoder_Counts_ControlRegRemoved || Motor_2_Encoder_Counts_UsingFixedFunction)
        Motor_2_Encoder_Counts_CONTROL &= ((uint8)(~Motor_2_Encoder_Counts_CTRL_ENABLE));        
    #endif /* (!Motor_2_Encoder_Counts_ControlRegRemoved || Motor_2_Encoder_Counts_UsingFixedFunction) */
    
    /* Globally disable the Fixed Function Block chosen */
    #if (Motor_2_Encoder_Counts_UsingFixedFunction)
        Motor_2_Encoder_Counts_GLOBAL_ENABLE &= ((uint8)(~Motor_2_Encoder_Counts_BLOCK_EN_MASK));
        Motor_2_Encoder_Counts_GLOBAL_STBY_ENABLE &= ((uint8)(~Motor_2_Encoder_Counts_BLOCK_STBY_EN_MASK));
    #endif /* (Motor_2_Encoder_Counts_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_SetInterruptMode
********************************************************************************
* Summary:
* Configures which interrupt sources are enabled to generate the final interrupt
*
* Parameters:  
*  InterruptsMask: This parameter is an or'd collection of the status bits
*                   which will be allowed to generate the counters interrupt.   
*
* Return: 
*  void
*
*******************************************************************************/
void Motor_2_Encoder_Counts_SetInterruptMode(uint8 interruptsMask) 
{
    Motor_2_Encoder_Counts_STATUS_MASK = interruptsMask;
}


/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_ReadStatusRegister
********************************************************************************
* Summary:
*   Reads the status register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the status register
*
* Side Effects:
*   Status register bits may be clear on read. 
*
*******************************************************************************/
uint8   Motor_2_Encoder_Counts_ReadStatusRegister(void) 
{
    return Motor_2_Encoder_Counts_STATUS;
}


#if(!Motor_2_Encoder_Counts_ControlRegRemoved)
/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_ReadControlRegister
********************************************************************************
* Summary:
*   Reads the control register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
uint8   Motor_2_Encoder_Counts_ReadControlRegister(void) 
{
    return Motor_2_Encoder_Counts_CONTROL;
}


/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_WriteControlRegister
********************************************************************************
* Summary:
*   Sets the bit-field of the control register.  This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
void    Motor_2_Encoder_Counts_WriteControlRegister(uint8 control) 
{
    Motor_2_Encoder_Counts_CONTROL = control;
}

#endif  /* (!Motor_2_Encoder_Counts_ControlRegRemoved) */


#if (!(Motor_2_Encoder_Counts_UsingFixedFunction && (CY_PSOC5A)))
/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_WriteCounter
********************************************************************************
* Summary:
*   This funtion is used to set the counter to a specific value
*
* Parameters:  
*  counter:  New counter value. 
*
* Return: 
*  void 
*
*******************************************************************************/
void Motor_2_Encoder_Counts_WriteCounter(uint16 counter) \
                                   
{
    #if(Motor_2_Encoder_Counts_UsingFixedFunction)
        /* assert if block is already enabled */
        CYASSERT (0u == (Motor_2_Encoder_Counts_GLOBAL_ENABLE & Motor_2_Encoder_Counts_BLOCK_EN_MASK));
        /* If block is disabled, enable it and then write the counter */
        Motor_2_Encoder_Counts_GLOBAL_ENABLE |= Motor_2_Encoder_Counts_BLOCK_EN_MASK;
        CY_SET_REG16(Motor_2_Encoder_Counts_COUNTER_LSB_PTR, (uint16)counter);
        Motor_2_Encoder_Counts_GLOBAL_ENABLE &= ((uint8)(~Motor_2_Encoder_Counts_BLOCK_EN_MASK));
    #else
        CY_SET_REG16(Motor_2_Encoder_Counts_COUNTER_LSB_PTR, counter);
    #endif /* (Motor_2_Encoder_Counts_UsingFixedFunction) */
}
#endif /* (!(Motor_2_Encoder_Counts_UsingFixedFunction && (CY_PSOC5A))) */


/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_ReadCounter
********************************************************************************
* Summary:
* Returns the current value of the counter.  It doesn't matter
* if the counter is enabled or running.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint16) The present value of the counter.
*
*******************************************************************************/
uint16 Motor_2_Encoder_Counts_ReadCounter(void) 
{
    /* Force capture by reading Accumulator */
    /* Must first do a software capture to be able to read the counter */
    /* It is up to the user code to make sure there isn't already captured data in the FIFO */
    #if(Motor_2_Encoder_Counts_UsingFixedFunction)
		(void)CY_GET_REG16(Motor_2_Encoder_Counts_COUNTER_LSB_PTR);
	#else
		(void)CY_GET_REG8(Motor_2_Encoder_Counts_COUNTER_LSB_PTR_8BIT);
	#endif/* (Motor_2_Encoder_Counts_UsingFixedFunction) */
    
    /* Read the data from the FIFO (or capture register for Fixed Function)*/
    #if(Motor_2_Encoder_Counts_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(Motor_2_Encoder_Counts_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(Motor_2_Encoder_Counts_STATICCOUNT_LSB_PTR));
    #endif /* (Motor_2_Encoder_Counts_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_ReadCapture
********************************************************************************
* Summary:
*   This function returns the last value captured.
*
* Parameters:  
*  void
*
* Return: 
*  (uint16) Present Capture value.
*
*******************************************************************************/
uint16 Motor_2_Encoder_Counts_ReadCapture(void) 
{
    #if(Motor_2_Encoder_Counts_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(Motor_2_Encoder_Counts_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(Motor_2_Encoder_Counts_STATICCOUNT_LSB_PTR));
    #endif /* (Motor_2_Encoder_Counts_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_WritePeriod
********************************************************************************
* Summary:
* Changes the period of the counter.  The new period 
* will be loaded the next time terminal count is detected.
*
* Parameters:  
*  period: (uint16) A value of 0 will result in
*         the counter remaining at zero.  
*
* Return: 
*  void
*
*******************************************************************************/
void Motor_2_Encoder_Counts_WritePeriod(uint16 period) 
{
    #if(Motor_2_Encoder_Counts_UsingFixedFunction)
        CY_SET_REG16(Motor_2_Encoder_Counts_PERIOD_LSB_PTR,(uint16)period);
    #else
        CY_SET_REG16(Motor_2_Encoder_Counts_PERIOD_LSB_PTR, period);
    #endif /* (Motor_2_Encoder_Counts_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_ReadPeriod
********************************************************************************
* Summary:
* Reads the current period value without affecting counter operation.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint16) Present period value.
*
*******************************************************************************/
uint16 Motor_2_Encoder_Counts_ReadPeriod(void) 
{
    #if(Motor_2_Encoder_Counts_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(Motor_2_Encoder_Counts_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG16(Motor_2_Encoder_Counts_PERIOD_LSB_PTR));
    #endif /* (Motor_2_Encoder_Counts_UsingFixedFunction) */
}


#if (!Motor_2_Encoder_Counts_UsingFixedFunction)
/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_WriteCompare
********************************************************************************
* Summary:
* Changes the compare value.  The compare output will 
* reflect the new value on the next UDB clock.  The compare output will be 
* driven high when the present counter value compares true based on the 
* configured compare mode setting. 
*
* Parameters:  
*  Compare:  New compare value. 
*
* Return: 
*  void
*
*******************************************************************************/
void Motor_2_Encoder_Counts_WriteCompare(uint16 compare) \
                                   
{
    #if(Motor_2_Encoder_Counts_UsingFixedFunction)
        CY_SET_REG16(Motor_2_Encoder_Counts_COMPARE_LSB_PTR, (uint16)compare);
    #else
        CY_SET_REG16(Motor_2_Encoder_Counts_COMPARE_LSB_PTR, compare);
    #endif /* (Motor_2_Encoder_Counts_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_ReadCompare
********************************************************************************
* Summary:
* Returns the compare value.
*
* Parameters:  
*  void:
*
* Return: 
*  (uint16) Present compare value.
*
*******************************************************************************/
uint16 Motor_2_Encoder_Counts_ReadCompare(void) 
{
    return (CY_GET_REG16(Motor_2_Encoder_Counts_COMPARE_LSB_PTR));
}


#if (Motor_2_Encoder_Counts_COMPARE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_SetCompareMode
********************************************************************************
* Summary:
*  Sets the software controlled Compare Mode.
*
* Parameters:
*  compareMode:  Compare Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void Motor_2_Encoder_Counts_SetCompareMode(uint8 compareMode) 
{
    /* Clear the compare mode bits in the control register */
    Motor_2_Encoder_Counts_CONTROL &= ((uint8)(~Motor_2_Encoder_Counts_CTRL_CMPMODE_MASK));
    
    /* Write the new setting */
    Motor_2_Encoder_Counts_CONTROL |= compareMode;
}
#endif  /* (Motor_2_Encoder_Counts_COMPARE_MODE_SOFTWARE) */


#if (Motor_2_Encoder_Counts_CAPTURE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_SetCaptureMode
********************************************************************************
* Summary:
*  Sets the software controlled Capture Mode.
*
* Parameters:
*  captureMode:  Capture Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void Motor_2_Encoder_Counts_SetCaptureMode(uint8 captureMode) 
{
    /* Clear the capture mode bits in the control register */
    Motor_2_Encoder_Counts_CONTROL &= ((uint8)(~Motor_2_Encoder_Counts_CTRL_CAPMODE_MASK));
    
    /* Write the new setting */
    Motor_2_Encoder_Counts_CONTROL |= ((uint8)((uint8)captureMode << Motor_2_Encoder_Counts_CTRL_CAPMODE0_SHIFT));
}
#endif  /* (Motor_2_Encoder_Counts_CAPTURE_MODE_SOFTWARE) */


/*******************************************************************************
* Function Name: Motor_2_Encoder_Counts_ClearFIFO
********************************************************************************
* Summary:
*   This function clears all capture data from the capture FIFO
*
* Parameters:  
*  void:
*
* Return: 
*  None
*
*******************************************************************************/
void Motor_2_Encoder_Counts_ClearFIFO(void) 
{

    while(0u != (Motor_2_Encoder_Counts_ReadStatusRegister() & Motor_2_Encoder_Counts_STATUS_FIFONEMP))
    {
        (void)Motor_2_Encoder_Counts_ReadCapture();
    }

}
#endif  /* (!Motor_2_Encoder_Counts_UsingFixedFunction) */


/* [] END OF FILE */

