/*******************************************************************************
* File Name: Rack_Servo.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Rack_Servo_H) /* Pins Rack_Servo_H */
#define CY_PINS_Rack_Servo_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Rack_Servo_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Rack_Servo__PORT == 15 && ((Rack_Servo__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Rack_Servo_Write(uint8 value);
void    Rack_Servo_SetDriveMode(uint8 mode);
uint8   Rack_Servo_ReadDataReg(void);
uint8   Rack_Servo_Read(void);
void    Rack_Servo_SetInterruptMode(uint16 position, uint16 mode);
uint8   Rack_Servo_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Rack_Servo_SetDriveMode() function.
     *  @{
     */
        #define Rack_Servo_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Rack_Servo_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Rack_Servo_DM_RES_UP          PIN_DM_RES_UP
        #define Rack_Servo_DM_RES_DWN         PIN_DM_RES_DWN
        #define Rack_Servo_DM_OD_LO           PIN_DM_OD_LO
        #define Rack_Servo_DM_OD_HI           PIN_DM_OD_HI
        #define Rack_Servo_DM_STRONG          PIN_DM_STRONG
        #define Rack_Servo_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Rack_Servo_MASK               Rack_Servo__MASK
#define Rack_Servo_SHIFT              Rack_Servo__SHIFT
#define Rack_Servo_WIDTH              1u

/* Interrupt constants */
#if defined(Rack_Servo__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Rack_Servo_SetInterruptMode() function.
     *  @{
     */
        #define Rack_Servo_INTR_NONE      (uint16)(0x0000u)
        #define Rack_Servo_INTR_RISING    (uint16)(0x0001u)
        #define Rack_Servo_INTR_FALLING   (uint16)(0x0002u)
        #define Rack_Servo_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Rack_Servo_INTR_MASK      (0x01u) 
#endif /* (Rack_Servo__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Rack_Servo_PS                     (* (reg8 *) Rack_Servo__PS)
/* Data Register */
#define Rack_Servo_DR                     (* (reg8 *) Rack_Servo__DR)
/* Port Number */
#define Rack_Servo_PRT_NUM                (* (reg8 *) Rack_Servo__PRT) 
/* Connect to Analog Globals */                                                  
#define Rack_Servo_AG                     (* (reg8 *) Rack_Servo__AG)                       
/* Analog MUX bux enable */
#define Rack_Servo_AMUX                   (* (reg8 *) Rack_Servo__AMUX) 
/* Bidirectional Enable */                                                        
#define Rack_Servo_BIE                    (* (reg8 *) Rack_Servo__BIE)
/* Bit-mask for Aliased Register Access */
#define Rack_Servo_BIT_MASK               (* (reg8 *) Rack_Servo__BIT_MASK)
/* Bypass Enable */
#define Rack_Servo_BYP                    (* (reg8 *) Rack_Servo__BYP)
/* Port wide control signals */                                                   
#define Rack_Servo_CTL                    (* (reg8 *) Rack_Servo__CTL)
/* Drive Modes */
#define Rack_Servo_DM0                    (* (reg8 *) Rack_Servo__DM0) 
#define Rack_Servo_DM1                    (* (reg8 *) Rack_Servo__DM1)
#define Rack_Servo_DM2                    (* (reg8 *) Rack_Servo__DM2) 
/* Input Buffer Disable Override */
#define Rack_Servo_INP_DIS                (* (reg8 *) Rack_Servo__INP_DIS)
/* LCD Common or Segment Drive */
#define Rack_Servo_LCD_COM_SEG            (* (reg8 *) Rack_Servo__LCD_COM_SEG)
/* Enable Segment LCD */
#define Rack_Servo_LCD_EN                 (* (reg8 *) Rack_Servo__LCD_EN)
/* Slew Rate Control */
#define Rack_Servo_SLW                    (* (reg8 *) Rack_Servo__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Rack_Servo_PRTDSI__CAPS_SEL       (* (reg8 *) Rack_Servo__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Rack_Servo_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Rack_Servo__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Rack_Servo_PRTDSI__OE_SEL0        (* (reg8 *) Rack_Servo__PRTDSI__OE_SEL0) 
#define Rack_Servo_PRTDSI__OE_SEL1        (* (reg8 *) Rack_Servo__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Rack_Servo_PRTDSI__OUT_SEL0       (* (reg8 *) Rack_Servo__PRTDSI__OUT_SEL0) 
#define Rack_Servo_PRTDSI__OUT_SEL1       (* (reg8 *) Rack_Servo__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Rack_Servo_PRTDSI__SYNC_OUT       (* (reg8 *) Rack_Servo__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Rack_Servo__SIO_CFG)
    #define Rack_Servo_SIO_HYST_EN        (* (reg8 *) Rack_Servo__SIO_HYST_EN)
    #define Rack_Servo_SIO_REG_HIFREQ     (* (reg8 *) Rack_Servo__SIO_REG_HIFREQ)
    #define Rack_Servo_SIO_CFG            (* (reg8 *) Rack_Servo__SIO_CFG)
    #define Rack_Servo_SIO_DIFF           (* (reg8 *) Rack_Servo__SIO_DIFF)
#endif /* (Rack_Servo__SIO_CFG) */

/* Interrupt Registers */
#if defined(Rack_Servo__INTSTAT)
    #define Rack_Servo_INTSTAT            (* (reg8 *) Rack_Servo__INTSTAT)
    #define Rack_Servo_SNAP               (* (reg8 *) Rack_Servo__SNAP)
    
	#define Rack_Servo_0_INTTYPE_REG 		(* (reg8 *) Rack_Servo__0__INTTYPE)
#endif /* (Rack_Servo__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Rack_Servo_H */


/* [] END OF FILE */
