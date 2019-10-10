/*******************************************************************************
* File Name: Drift_Check_Interrupt.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_Drift_Check_Interrupt_H)
#define CY_ISR_Drift_Check_Interrupt_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void Drift_Check_Interrupt_Start(void);
void Drift_Check_Interrupt_StartEx(cyisraddress address);
void Drift_Check_Interrupt_Stop(void);

CY_ISR_PROTO(Drift_Check_Interrupt_Interrupt);

void Drift_Check_Interrupt_SetVector(cyisraddress address);
cyisraddress Drift_Check_Interrupt_GetVector(void);

void Drift_Check_Interrupt_SetPriority(uint8 priority);
uint8 Drift_Check_Interrupt_GetPriority(void);

void Drift_Check_Interrupt_Enable(void);
uint8 Drift_Check_Interrupt_GetState(void);
void Drift_Check_Interrupt_Disable(void);

void Drift_Check_Interrupt_SetPending(void);
void Drift_Check_Interrupt_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the Drift_Check_Interrupt ISR. */
#define Drift_Check_Interrupt_INTC_VECTOR            ((reg32 *) Drift_Check_Interrupt__INTC_VECT)

/* Address of the Drift_Check_Interrupt ISR priority. */
#define Drift_Check_Interrupt_INTC_PRIOR             ((reg8 *) Drift_Check_Interrupt__INTC_PRIOR_REG)

/* Priority of the Drift_Check_Interrupt interrupt. */
#define Drift_Check_Interrupt_INTC_PRIOR_NUMBER      Drift_Check_Interrupt__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable Drift_Check_Interrupt interrupt. */
#define Drift_Check_Interrupt_INTC_SET_EN            ((reg32 *) Drift_Check_Interrupt__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the Drift_Check_Interrupt interrupt. */
#define Drift_Check_Interrupt_INTC_CLR_EN            ((reg32 *) Drift_Check_Interrupt__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the Drift_Check_Interrupt interrupt state to pending. */
#define Drift_Check_Interrupt_INTC_SET_PD            ((reg32 *) Drift_Check_Interrupt__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the Drift_Check_Interrupt interrupt. */
#define Drift_Check_Interrupt_INTC_CLR_PD            ((reg32 *) Drift_Check_Interrupt__INTC_CLR_PD_REG)


#endif /* CY_ISR_Drift_Check_Interrupt_H */


/* [] END OF FILE */
