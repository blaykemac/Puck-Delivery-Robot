/*******************************************************************************
* File Name: Motor_Right_Decoder.c  
* Version 3.0
*
* Description:
*  This file provides the source code to the API for the Quadrature Decoder
*  component.
*
* Note:
*  None.
*   
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "Motor_Right_Decoder.h"

#if (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT)
    #include "Motor_Right_Decoder_PVT.h"
#endif /* Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT */

uint8 Motor_Right_Decoder_initVar = 0u;


/*******************************************************************************
* Function Name: Motor_Right_Decoder_Init
********************************************************************************
*
* Summary:
*  Inits/Restores default QuadDec configuration provided with customizer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Motor_Right_Decoder_Init(void) 
{
    #if (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT)
        /* Disable Interrupt. */
        CyIntDisable(Motor_Right_Decoder_ISR_NUMBER);
        /* Set the ISR to point to the Motor_Right_Decoder_isr Interrupt. */
        (void) CyIntSetVector(Motor_Right_Decoder_ISR_NUMBER, & Motor_Right_Decoder_ISR);
        /* Set the priority. */
        CyIntSetPriority(Motor_Right_Decoder_ISR_NUMBER, Motor_Right_Decoder_ISR_PRIORITY);
    #endif /* Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT */
}


/*******************************************************************************
* Function Name: Motor_Right_Decoder_Enable
********************************************************************************
*
* Summary:
*  This function enable interrupts from Component and also enable Component's
*  ISR in case of 32-bit counter.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Motor_Right_Decoder_Enable(void) 
{
    uint8 enableInterrupts;

    Motor_Right_Decoder_SetInterruptMask(Motor_Right_Decoder_INIT_INT_MASK);

    /* Clear pending interrupts. */
    (void) Motor_Right_Decoder_GetEvents();
    
    enableInterrupts = CyEnterCriticalSection();

    /* Enable interrupts from Statusi register */
    Motor_Right_Decoder_SR_AUX_CONTROL |= Motor_Right_Decoder_INTERRUPTS_ENABLE;

    CyExitCriticalSection(enableInterrupts);        

    #if (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT)
        /* Enable Component interrupts */
        CyIntEnable(Motor_Right_Decoder_ISR_NUMBER);
    #endif /* Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT */
}


/*******************************************************************************
* Function Name: Motor_Right_Decoder_Start
********************************************************************************
*
* Summary:
*  Initializes UDBs and other relevant hardware.
*  Resets counter, enables or disables all relevant interrupts.
*  Starts monitoring the inputs and counting.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  Motor_Right_Decoder_initVar - used to check initial configuration, modified on
*  first function call.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Motor_Right_Decoder_Start(void) 
{
    #if (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_8_BIT)
        Motor_Right_Decoder_Cnt8_Start();
        Motor_Right_Decoder_Cnt8_WriteCounter(Motor_Right_Decoder_COUNTER_INIT_VALUE);
    #else
        /* (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_16_BIT) || 
        *  (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT) 
        */
        Motor_Right_Decoder_Cnt16_Start();
        Motor_Right_Decoder_Cnt16_WriteCounter(Motor_Right_Decoder_COUNTER_INIT_VALUE);
    #endif /* Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_8_BIT */
    
    #if (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT)        
       Motor_Right_Decoder_count32SoftPart = 0;
    #endif /* Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT */

    if (Motor_Right_Decoder_initVar == 0u)
    {
        Motor_Right_Decoder_Init();
        Motor_Right_Decoder_initVar = 1u;
    }

    Motor_Right_Decoder_Enable();
}


/*******************************************************************************
* Function Name: Motor_Right_Decoder_Stop
********************************************************************************
*
* Summary:
*  Turns off UDBs and other relevant hardware.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Motor_Right_Decoder_Stop(void) 
{
    uint8 enableInterrupts;

    #if (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_8_BIT)
        Motor_Right_Decoder_Cnt8_Stop();
    #else 
        /* (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_16_BIT) ||
        *  (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT)
        */
        Motor_Right_Decoder_Cnt16_Stop();    /* counter disable */
    #endif /* (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_8_BIT) */
 
    enableInterrupts = CyEnterCriticalSection();

    /* Disable interrupts interrupts from Statusi register */
    Motor_Right_Decoder_SR_AUX_CONTROL &= (uint8) (~Motor_Right_Decoder_INTERRUPTS_ENABLE);

    CyExitCriticalSection(enableInterrupts);

    #if (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT)
        CyIntDisable(Motor_Right_Decoder_ISR_NUMBER);    /* interrupt disable */
    #endif /* Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT */
}


/*******************************************************************************
* Function Name: Motor_Right_Decoder_GetCounter
********************************************************************************
*
* Summary:
*  Reports the current value of the counter.
*
* Parameters:
*  None.
*
* Return:
*  The counter value. Return type is signed and per the counter size setting.
*  A positive value indicates clockwise movement (B before A).
*
* Global variables:
*  Motor_Right_Decoder_count32SoftPart - used to get hi 16 bit for current value
*  of the 32-bit counter, when Counter size equal 32-bit.
*
*******************************************************************************/
int16 Motor_Right_Decoder_GetCounter(void) 
{
    int16 count;
    uint16 tmpCnt;

    #if (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT)
        int16 hwCount;

        CyIntDisable(Motor_Right_Decoder_ISR_NUMBER);

        tmpCnt = Motor_Right_Decoder_Cnt16_ReadCounter();
        hwCount = (int16) ((int32) tmpCnt - (int32) Motor_Right_Decoder_COUNTER_INIT_VALUE);
        count = Motor_Right_Decoder_count32SoftPart + hwCount;

        CyIntEnable(Motor_Right_Decoder_ISR_NUMBER);
    #else 
        /* (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_8_BIT) || 
        *  (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_16_BIT)
        */
        #if (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_8_BIT)
            tmpCnt = Motor_Right_Decoder_Cnt8_ReadCounter();
        #else /* (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_16_BIT) */
            tmpCnt = Motor_Right_Decoder_Cnt16_ReadCounter();
        #endif  /* Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_8_BIT */

        count = (int16) ((int32) tmpCnt -
                (int32) Motor_Right_Decoder_COUNTER_INIT_VALUE);

    #endif /* Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT */ 

    return (count);
}


/*******************************************************************************
* Function Name: Motor_Right_Decoder_SetCounter
********************************************************************************
*
* Summary:
*  Sets the current value of the counter.
*
* Parameters:
*  value:  The new value. Parameter type is signed and per the counter size
*  setting.
*
* Return:
*  None.
*
* Global variables:
*  Motor_Right_Decoder_count32SoftPart - modified to set hi 16 bit for current
*  value of the 32-bit counter, when Counter size equal 32-bit.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Motor_Right_Decoder_SetCounter(int16 value) 
{
    #if ((Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_8_BIT) || \
         (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_16_BIT))
        uint16 count;
        
        if (value >= 0)
        {
            count = (uint16) value + Motor_Right_Decoder_COUNTER_INIT_VALUE;
        }
        else
        {
            count = Motor_Right_Decoder_COUNTER_INIT_VALUE - (uint16)(-value);
        }
        #if (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_8_BIT)
            Motor_Right_Decoder_Cnt8_WriteCounter(count);
        #else /* (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_16_BIT) */
            Motor_Right_Decoder_Cnt16_WriteCounter(count);
        #endif  /* Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_8_BIT */
    #else /* (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT) */
        CyIntDisable(Motor_Right_Decoder_ISR_NUMBER);

        Motor_Right_Decoder_Cnt16_WriteCounter(Motor_Right_Decoder_COUNTER_INIT_VALUE);
        Motor_Right_Decoder_count32SoftPart = value;

        CyIntEnable(Motor_Right_Decoder_ISR_NUMBER);
    #endif  /* (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_8_BIT) ||
             * (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_16_BIT)
             */
}


/*******************************************************************************
* Function Name: Motor_Right_Decoder_GetEvents
********************************************************************************
* 
* Summary:
*   Reports the current status of events. This function clears the bits of the 
*   status register.
*
* Parameters:
*  None.
*
* Return:
*  The events, as bits in an unsigned 8-bit value:
*    Bit      Description
*     0        Counter overflow.
*     1        Counter underflow.
*     2        Counter reset due to index, if index input is used.
*     3        Invalid A, B inputs state transition.
*
*******************************************************************************/
uint8 Motor_Right_Decoder_GetEvents(void) 
{
    return (Motor_Right_Decoder_STATUS_REG & Motor_Right_Decoder_INIT_INT_MASK);
}


/*******************************************************************************
* Function Name: Motor_Right_Decoder_SetInterruptMask
********************************************************************************
*
* Summary:
*  Enables / disables interrupts due to the events.
*  For the 32-bit counter, the overflow, underflow and reset interrupts cannot
*  be disabled, these bits are ignored.
*
* Parameters:
*  mask: Enable / disable bits in an 8-bit value, where 1 enables the interrupt.
*
* Return:
*  None.
*
*******************************************************************************/
void Motor_Right_Decoder_SetInterruptMask(uint8 mask) 
{
    #if (Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT)
        /* Underflow, Overflow and Reset interrupts for 32-bit Counter are always enable */
        mask |= (Motor_Right_Decoder_COUNTER_OVERFLOW | Motor_Right_Decoder_COUNTER_UNDERFLOW |
                 Motor_Right_Decoder_COUNTER_RESET);
    #endif /* Motor_Right_Decoder_COUNTER_SIZE == Motor_Right_Decoder_COUNTER_SIZE_32_BIT */

    Motor_Right_Decoder_STATUS_MASK = mask;
}


/*******************************************************************************
* Function Name: Motor_Right_Decoder_GetInterruptMask
********************************************************************************
*
* Summary:
*  Reports the current interrupt mask settings.
*
* Parameters:
*  None.
*
* Return:
*  Enable / disable bits in an 8-bit value, where 1 enables the interrupt.
*  For the 32-bit counter, the overflow, underflow and reset enable bits are
*  always set.
*
*******************************************************************************/
uint8 Motor_Right_Decoder_GetInterruptMask(void) 
{
    return (Motor_Right_Decoder_STATUS_MASK & Motor_Right_Decoder_INIT_INT_MASK);
}


/* [] END OF FILE */
