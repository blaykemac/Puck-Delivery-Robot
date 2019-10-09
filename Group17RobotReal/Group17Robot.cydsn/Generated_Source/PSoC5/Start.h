/*******************************************************************************
* File Name: Start.h
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
#if !defined(CY_ISR_Start_H)
#define CY_ISR_Start_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void Start_Start(void);
void Start_StartEx(cyisraddress address);
void Start_Stop(void);

CY_ISR_PROTO(Start_Interrupt);

void Start_SetVector(cyisraddress address);
cyisraddress Start_GetVector(void);

void Start_SetPriority(uint8 priority);
uint8 Start_GetPriority(void);

void Start_Enable(void);
uint8 Start_GetState(void);
void Start_Disable(void);

void Start_SetPending(void);
void Start_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the Start ISR. */
#define Start_INTC_VECTOR            ((reg32 *) Start__INTC_VECT)

/* Address of the Start ISR priority. */
#define Start_INTC_PRIOR             ((reg8 *) Start__INTC_PRIOR_REG)

/* Priority of the Start interrupt. */
#define Start_INTC_PRIOR_NUMBER      Start__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable Start interrupt. */
#define Start_INTC_SET_EN            ((reg32 *) Start__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the Start interrupt. */
#define Start_INTC_CLR_EN            ((reg32 *) Start__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the Start interrupt state to pending. */
#define Start_INTC_SET_PD            ((reg32 *) Start__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the Start interrupt. */
#define Start_INTC_CLR_PD            ((reg32 *) Start__INTC_CLR_PD_REG)


#endif /* CY_ISR_Start_H */


/* [] END OF FILE */
