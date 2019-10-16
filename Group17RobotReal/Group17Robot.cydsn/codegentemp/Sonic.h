/*******************************************************************************
* File Name: Sonic.h
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
#if !defined(CY_ISR_Sonic_H)
#define CY_ISR_Sonic_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void Sonic_Start(void);
void Sonic_StartEx(cyisraddress address);
void Sonic_Stop(void);

CY_ISR_PROTO(Sonic_Interrupt);

void Sonic_SetVector(cyisraddress address);
cyisraddress Sonic_GetVector(void);

void Sonic_SetPriority(uint8 priority);
uint8 Sonic_GetPriority(void);

void Sonic_Enable(void);
uint8 Sonic_GetState(void);
void Sonic_Disable(void);

void Sonic_SetPending(void);
void Sonic_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the Sonic ISR. */
#define Sonic_INTC_VECTOR            ((reg32 *) Sonic__INTC_VECT)

/* Address of the Sonic ISR priority. */
#define Sonic_INTC_PRIOR             ((reg8 *) Sonic__INTC_PRIOR_REG)

/* Priority of the Sonic interrupt. */
#define Sonic_INTC_PRIOR_NUMBER      Sonic__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable Sonic interrupt. */
#define Sonic_INTC_SET_EN            ((reg32 *) Sonic__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the Sonic interrupt. */
#define Sonic_INTC_CLR_EN            ((reg32 *) Sonic__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the Sonic interrupt state to pending. */
#define Sonic_INTC_SET_PD            ((reg32 *) Sonic__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the Sonic interrupt. */
#define Sonic_INTC_CLR_PD            ((reg32 *) Sonic__INTC_CLR_PD_REG)


#endif /* CY_ISR_Sonic_H */


/* [] END OF FILE */
