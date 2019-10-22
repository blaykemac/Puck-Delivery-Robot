/*******************************************************************************
* File Name: Straight.h
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
#if !defined(CY_ISR_Straight_H)
#define CY_ISR_Straight_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void Straight_Start(void);
void Straight_StartEx(cyisraddress address);
void Straight_Stop(void);

CY_ISR_PROTO(Straight_Interrupt);

void Straight_SetVector(cyisraddress address);
cyisraddress Straight_GetVector(void);

void Straight_SetPriority(uint8 priority);
uint8 Straight_GetPriority(void);

void Straight_Enable(void);
uint8 Straight_GetState(void);
void Straight_Disable(void);

void Straight_SetPending(void);
void Straight_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the Straight ISR. */
#define Straight_INTC_VECTOR            ((reg32 *) Straight__INTC_VECT)

/* Address of the Straight ISR priority. */
#define Straight_INTC_PRIOR             ((reg8 *) Straight__INTC_PRIOR_REG)

/* Priority of the Straight interrupt. */
#define Straight_INTC_PRIOR_NUMBER      Straight__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable Straight interrupt. */
#define Straight_INTC_SET_EN            ((reg32 *) Straight__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the Straight interrupt. */
#define Straight_INTC_CLR_EN            ((reg32 *) Straight__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the Straight interrupt state to pending. */
#define Straight_INTC_SET_PD            ((reg32 *) Straight__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the Straight interrupt. */
#define Straight_INTC_CLR_PD            ((reg32 *) Straight__INTC_CLR_PD_REG)


#endif /* CY_ISR_Straight_H */


/* [] END OF FILE */
