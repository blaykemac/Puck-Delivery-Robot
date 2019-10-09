/*******************************************************************************
* File Name: Gripper_Servo_Disable.h  
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

#if !defined(CY_PINS_Gripper_Servo_Disable_H) /* Pins Gripper_Servo_Disable_H */
#define CY_PINS_Gripper_Servo_Disable_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Gripper_Servo_Disable_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Gripper_Servo_Disable__PORT == 15 && ((Gripper_Servo_Disable__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Gripper_Servo_Disable_Write(uint8 value);
void    Gripper_Servo_Disable_SetDriveMode(uint8 mode);
uint8   Gripper_Servo_Disable_ReadDataReg(void);
uint8   Gripper_Servo_Disable_Read(void);
void    Gripper_Servo_Disable_SetInterruptMode(uint16 position, uint16 mode);
uint8   Gripper_Servo_Disable_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Gripper_Servo_Disable_SetDriveMode() function.
     *  @{
     */
        #define Gripper_Servo_Disable_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Gripper_Servo_Disable_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Gripper_Servo_Disable_DM_RES_UP          PIN_DM_RES_UP
        #define Gripper_Servo_Disable_DM_RES_DWN         PIN_DM_RES_DWN
        #define Gripper_Servo_Disable_DM_OD_LO           PIN_DM_OD_LO
        #define Gripper_Servo_Disable_DM_OD_HI           PIN_DM_OD_HI
        #define Gripper_Servo_Disable_DM_STRONG          PIN_DM_STRONG
        #define Gripper_Servo_Disable_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Gripper_Servo_Disable_MASK               Gripper_Servo_Disable__MASK
#define Gripper_Servo_Disable_SHIFT              Gripper_Servo_Disable__SHIFT
#define Gripper_Servo_Disable_WIDTH              1u

/* Interrupt constants */
#if defined(Gripper_Servo_Disable__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Gripper_Servo_Disable_SetInterruptMode() function.
     *  @{
     */
        #define Gripper_Servo_Disable_INTR_NONE      (uint16)(0x0000u)
        #define Gripper_Servo_Disable_INTR_RISING    (uint16)(0x0001u)
        #define Gripper_Servo_Disable_INTR_FALLING   (uint16)(0x0002u)
        #define Gripper_Servo_Disable_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Gripper_Servo_Disable_INTR_MASK      (0x01u) 
#endif /* (Gripper_Servo_Disable__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Gripper_Servo_Disable_PS                     (* (reg8 *) Gripper_Servo_Disable__PS)
/* Data Register */
#define Gripper_Servo_Disable_DR                     (* (reg8 *) Gripper_Servo_Disable__DR)
/* Port Number */
#define Gripper_Servo_Disable_PRT_NUM                (* (reg8 *) Gripper_Servo_Disable__PRT) 
/* Connect to Analog Globals */                                                  
#define Gripper_Servo_Disable_AG                     (* (reg8 *) Gripper_Servo_Disable__AG)                       
/* Analog MUX bux enable */
#define Gripper_Servo_Disable_AMUX                   (* (reg8 *) Gripper_Servo_Disable__AMUX) 
/* Bidirectional Enable */                                                        
#define Gripper_Servo_Disable_BIE                    (* (reg8 *) Gripper_Servo_Disable__BIE)
/* Bit-mask for Aliased Register Access */
#define Gripper_Servo_Disable_BIT_MASK               (* (reg8 *) Gripper_Servo_Disable__BIT_MASK)
/* Bypass Enable */
#define Gripper_Servo_Disable_BYP                    (* (reg8 *) Gripper_Servo_Disable__BYP)
/* Port wide control signals */                                                   
#define Gripper_Servo_Disable_CTL                    (* (reg8 *) Gripper_Servo_Disable__CTL)
/* Drive Modes */
#define Gripper_Servo_Disable_DM0                    (* (reg8 *) Gripper_Servo_Disable__DM0) 
#define Gripper_Servo_Disable_DM1                    (* (reg8 *) Gripper_Servo_Disable__DM1)
#define Gripper_Servo_Disable_DM2                    (* (reg8 *) Gripper_Servo_Disable__DM2) 
/* Input Buffer Disable Override */
#define Gripper_Servo_Disable_INP_DIS                (* (reg8 *) Gripper_Servo_Disable__INP_DIS)
/* LCD Common or Segment Drive */
#define Gripper_Servo_Disable_LCD_COM_SEG            (* (reg8 *) Gripper_Servo_Disable__LCD_COM_SEG)
/* Enable Segment LCD */
#define Gripper_Servo_Disable_LCD_EN                 (* (reg8 *) Gripper_Servo_Disable__LCD_EN)
/* Slew Rate Control */
#define Gripper_Servo_Disable_SLW                    (* (reg8 *) Gripper_Servo_Disable__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Gripper_Servo_Disable_PRTDSI__CAPS_SEL       (* (reg8 *) Gripper_Servo_Disable__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Gripper_Servo_Disable_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Gripper_Servo_Disable__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Gripper_Servo_Disable_PRTDSI__OE_SEL0        (* (reg8 *) Gripper_Servo_Disable__PRTDSI__OE_SEL0) 
#define Gripper_Servo_Disable_PRTDSI__OE_SEL1        (* (reg8 *) Gripper_Servo_Disable__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Gripper_Servo_Disable_PRTDSI__OUT_SEL0       (* (reg8 *) Gripper_Servo_Disable__PRTDSI__OUT_SEL0) 
#define Gripper_Servo_Disable_PRTDSI__OUT_SEL1       (* (reg8 *) Gripper_Servo_Disable__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Gripper_Servo_Disable_PRTDSI__SYNC_OUT       (* (reg8 *) Gripper_Servo_Disable__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Gripper_Servo_Disable__SIO_CFG)
    #define Gripper_Servo_Disable_SIO_HYST_EN        (* (reg8 *) Gripper_Servo_Disable__SIO_HYST_EN)
    #define Gripper_Servo_Disable_SIO_REG_HIFREQ     (* (reg8 *) Gripper_Servo_Disable__SIO_REG_HIFREQ)
    #define Gripper_Servo_Disable_SIO_CFG            (* (reg8 *) Gripper_Servo_Disable__SIO_CFG)
    #define Gripper_Servo_Disable_SIO_DIFF           (* (reg8 *) Gripper_Servo_Disable__SIO_DIFF)
#endif /* (Gripper_Servo_Disable__SIO_CFG) */

/* Interrupt Registers */
#if defined(Gripper_Servo_Disable__INTSTAT)
    #define Gripper_Servo_Disable_INTSTAT            (* (reg8 *) Gripper_Servo_Disable__INTSTAT)
    #define Gripper_Servo_Disable_SNAP               (* (reg8 *) Gripper_Servo_Disable__SNAP)
    
	#define Gripper_Servo_Disable_0_INTTYPE_REG 		(* (reg8 *) Gripper_Servo_Disable__0__INTTYPE)
#endif /* (Gripper_Servo_Disable__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Gripper_Servo_Disable_H */


/* [] END OF FILE */
