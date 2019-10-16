/*******************************************************************************
* File Name: MUX_control.h  
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

#if !defined(CY_PINS_MUX_control_H) /* Pins MUX_control_H */
#define CY_PINS_MUX_control_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "MUX_control_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 MUX_control__PORT == 15 && ((MUX_control__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    MUX_control_Write(uint8 value);
void    MUX_control_SetDriveMode(uint8 mode);
uint8   MUX_control_ReadDataReg(void);
uint8   MUX_control_Read(void);
void    MUX_control_SetInterruptMode(uint16 position, uint16 mode);
uint8   MUX_control_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the MUX_control_SetDriveMode() function.
     *  @{
     */
        #define MUX_control_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define MUX_control_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define MUX_control_DM_RES_UP          PIN_DM_RES_UP
        #define MUX_control_DM_RES_DWN         PIN_DM_RES_DWN
        #define MUX_control_DM_OD_LO           PIN_DM_OD_LO
        #define MUX_control_DM_OD_HI           PIN_DM_OD_HI
        #define MUX_control_DM_STRONG          PIN_DM_STRONG
        #define MUX_control_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define MUX_control_MASK               MUX_control__MASK
#define MUX_control_SHIFT              MUX_control__SHIFT
#define MUX_control_WIDTH              1u

/* Interrupt constants */
#if defined(MUX_control__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in MUX_control_SetInterruptMode() function.
     *  @{
     */
        #define MUX_control_INTR_NONE      (uint16)(0x0000u)
        #define MUX_control_INTR_RISING    (uint16)(0x0001u)
        #define MUX_control_INTR_FALLING   (uint16)(0x0002u)
        #define MUX_control_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define MUX_control_INTR_MASK      (0x01u) 
#endif /* (MUX_control__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define MUX_control_PS                     (* (reg8 *) MUX_control__PS)
/* Data Register */
#define MUX_control_DR                     (* (reg8 *) MUX_control__DR)
/* Port Number */
#define MUX_control_PRT_NUM                (* (reg8 *) MUX_control__PRT) 
/* Connect to Analog Globals */                                                  
#define MUX_control_AG                     (* (reg8 *) MUX_control__AG)                       
/* Analog MUX bux enable */
#define MUX_control_AMUX                   (* (reg8 *) MUX_control__AMUX) 
/* Bidirectional Enable */                                                        
#define MUX_control_BIE                    (* (reg8 *) MUX_control__BIE)
/* Bit-mask for Aliased Register Access */
#define MUX_control_BIT_MASK               (* (reg8 *) MUX_control__BIT_MASK)
/* Bypass Enable */
#define MUX_control_BYP                    (* (reg8 *) MUX_control__BYP)
/* Port wide control signals */                                                   
#define MUX_control_CTL                    (* (reg8 *) MUX_control__CTL)
/* Drive Modes */
#define MUX_control_DM0                    (* (reg8 *) MUX_control__DM0) 
#define MUX_control_DM1                    (* (reg8 *) MUX_control__DM1)
#define MUX_control_DM2                    (* (reg8 *) MUX_control__DM2) 
/* Input Buffer Disable Override */
#define MUX_control_INP_DIS                (* (reg8 *) MUX_control__INP_DIS)
/* LCD Common or Segment Drive */
#define MUX_control_LCD_COM_SEG            (* (reg8 *) MUX_control__LCD_COM_SEG)
/* Enable Segment LCD */
#define MUX_control_LCD_EN                 (* (reg8 *) MUX_control__LCD_EN)
/* Slew Rate Control */
#define MUX_control_SLW                    (* (reg8 *) MUX_control__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define MUX_control_PRTDSI__CAPS_SEL       (* (reg8 *) MUX_control__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define MUX_control_PRTDSI__DBL_SYNC_IN    (* (reg8 *) MUX_control__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define MUX_control_PRTDSI__OE_SEL0        (* (reg8 *) MUX_control__PRTDSI__OE_SEL0) 
#define MUX_control_PRTDSI__OE_SEL1        (* (reg8 *) MUX_control__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define MUX_control_PRTDSI__OUT_SEL0       (* (reg8 *) MUX_control__PRTDSI__OUT_SEL0) 
#define MUX_control_PRTDSI__OUT_SEL1       (* (reg8 *) MUX_control__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define MUX_control_PRTDSI__SYNC_OUT       (* (reg8 *) MUX_control__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(MUX_control__SIO_CFG)
    #define MUX_control_SIO_HYST_EN        (* (reg8 *) MUX_control__SIO_HYST_EN)
    #define MUX_control_SIO_REG_HIFREQ     (* (reg8 *) MUX_control__SIO_REG_HIFREQ)
    #define MUX_control_SIO_CFG            (* (reg8 *) MUX_control__SIO_CFG)
    #define MUX_control_SIO_DIFF           (* (reg8 *) MUX_control__SIO_DIFF)
#endif /* (MUX_control__SIO_CFG) */

/* Interrupt Registers */
#if defined(MUX_control__INTSTAT)
    #define MUX_control_INTSTAT            (* (reg8 *) MUX_control__INTSTAT)
    #define MUX_control_SNAP               (* (reg8 *) MUX_control__SNAP)
    
	#define MUX_control_0_INTTYPE_REG 		(* (reg8 *) MUX_control__0__INTTYPE)
#endif /* (MUX_control__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_MUX_control_H */


/* [] END OF FILE */
