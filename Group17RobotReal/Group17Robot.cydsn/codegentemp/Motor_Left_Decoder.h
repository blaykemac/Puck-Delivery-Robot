/*******************************************************************************
* File Name: Motor_Left_Decoder.h  
* Version 3.0
*
* Description:
*  This file provides constants and parameter values for the Quadrature
*  Decoder component.
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

#if !defined(CY_QUADRATURE_DECODER_Motor_Left_Decoder_H)
#define CY_QUADRATURE_DECODER_Motor_Left_Decoder_H

#include "cyfitter.h"
#include "CyLib.h"
#include "cytypes.h"

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component QuadDec_v3_0 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#define Motor_Left_Decoder_COUNTER_SIZE               (16u)
#define Motor_Left_Decoder_COUNTER_SIZE_8_BIT         (8u)
#define Motor_Left_Decoder_COUNTER_SIZE_16_BIT        (16u)
#define Motor_Left_Decoder_COUNTER_SIZE_32_BIT        (32u)

#if (Motor_Left_Decoder_COUNTER_SIZE == Motor_Left_Decoder_COUNTER_SIZE_8_BIT)
    #include "Motor_Left_Decoder_Cnt8.h"
#else 
    /* (Motor_Left_Decoder_COUNTER_SIZE == Motor_Left_Decoder_COUNTER_SIZE_16_BIT) || 
    *  (Motor_Left_Decoder_COUNTER_SIZE == Motor_Left_Decoder_COUNTER_SIZE_32_BIT) 
    */
    #include "Motor_Left_Decoder_Cnt16.h"
#endif /* Motor_Left_Decoder_COUNTER_SIZE == Motor_Left_Decoder_COUNTER_SIZE_8_BIT */

extern uint8 Motor_Left_Decoder_initVar;


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define Motor_Left_Decoder_COUNTER_RESOLUTION         (1u)


/***************************************
*       Data Struct Definition
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState;
} Motor_Left_Decoder_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void  Motor_Left_Decoder_Init(void) ;
void  Motor_Left_Decoder_Start(void) ;
void  Motor_Left_Decoder_Stop(void) ;
void  Motor_Left_Decoder_Enable(void) ;
uint8 Motor_Left_Decoder_GetEvents(void) ;
void  Motor_Left_Decoder_SetInterruptMask(uint8 mask) ;
uint8 Motor_Left_Decoder_GetInterruptMask(void) ;
int16 Motor_Left_Decoder_GetCounter(void) ;
void  Motor_Left_Decoder_SetCounter(int16 value)
;
void  Motor_Left_Decoder_Sleep(void) ;
void  Motor_Left_Decoder_Wakeup(void) ;
void  Motor_Left_Decoder_SaveConfig(void) ;
void  Motor_Left_Decoder_RestoreConfig(void) ;

#if (Motor_Left_Decoder_COUNTER_SIZE == Motor_Left_Decoder_COUNTER_SIZE_32_BIT)
    CY_ISR_PROTO(Motor_Left_Decoder_ISR);
#endif /* Motor_Left_Decoder_COUNTER_SIZE == Motor_Left_Decoder_COUNTER_SIZE_32_BIT */


/***************************************
*           API Constants
***************************************/

#if (Motor_Left_Decoder_COUNTER_SIZE == Motor_Left_Decoder_COUNTER_SIZE_32_BIT)
    #define Motor_Left_Decoder_ISR_NUMBER             ((uint8) Motor_Left_Decoder_isr__INTC_NUMBER)
    #define Motor_Left_Decoder_ISR_PRIORITY           ((uint8) Motor_Left_Decoder_isr__INTC_PRIOR_NUM)
#endif /* Motor_Left_Decoder_COUNTER_SIZE == Motor_Left_Decoder_COUNTER_SIZE_32_BIT */


/***************************************
*    Enumerated Types and Parameters
***************************************/

#define Motor_Left_Decoder_GLITCH_FILTERING           (1u)
#define Motor_Left_Decoder_INDEX_INPUT                (0u)


/***************************************
*    Initial Parameter Constants
***************************************/

#if (Motor_Left_Decoder_COUNTER_SIZE == Motor_Left_Decoder_COUNTER_SIZE_8_BIT)
    #define Motor_Left_Decoder_COUNTER_INIT_VALUE    (0x80u)
#else 
    /* (Motor_Left_Decoder_COUNTER_SIZE == Motor_Left_Decoder_COUNTER_SIZE_16_BIT) ||
    *  (Motor_Left_Decoder_COUNTER_SIZE == Motor_Left_Decoder_COUNTER_SIZE_32_BIT)
    */
    #define Motor_Left_Decoder_COUNTER_INIT_VALUE    (0x8000u)
    #define Motor_Left_Decoder_COUNTER_MAX_VALUE     (0x7FFFu)
#endif /* Motor_Left_Decoder_COUNTER_SIZE == Motor_Left_Decoder_COUNTER_SIZE_8_BIT */


/***************************************
*             Registers
***************************************/

#define Motor_Left_Decoder_STATUS_REG                 (* (reg8 *) Motor_Left_Decoder_bQuadDec_Stsreg__STATUS_REG)
#define Motor_Left_Decoder_STATUS_PTR                 (  (reg8 *) Motor_Left_Decoder_bQuadDec_Stsreg__STATUS_REG)
#define Motor_Left_Decoder_STATUS_MASK                (* (reg8 *) Motor_Left_Decoder_bQuadDec_Stsreg__MASK_REG)
#define Motor_Left_Decoder_STATUS_MASK_PTR            (  (reg8 *) Motor_Left_Decoder_bQuadDec_Stsreg__MASK_REG)
#define Motor_Left_Decoder_SR_AUX_CONTROL             (* (reg8 *) Motor_Left_Decoder_bQuadDec_Stsreg__STATUS_AUX_CTL_REG)
#define Motor_Left_Decoder_SR_AUX_CONTROL_PTR         (  (reg8 *) Motor_Left_Decoder_bQuadDec_Stsreg__STATUS_AUX_CTL_REG)


/***************************************
*        Register Constants
***************************************/

#define Motor_Left_Decoder_COUNTER_OVERFLOW_SHIFT     (0x00u)
#define Motor_Left_Decoder_COUNTER_UNDERFLOW_SHIFT    (0x01u)
#define Motor_Left_Decoder_COUNTER_RESET_SHIFT        (0x02u)
#define Motor_Left_Decoder_INVALID_IN_SHIFT           (0x03u)
#define Motor_Left_Decoder_COUNTER_OVERFLOW           ((uint8) (0x01u << Motor_Left_Decoder_COUNTER_OVERFLOW_SHIFT))
#define Motor_Left_Decoder_COUNTER_UNDERFLOW          ((uint8) (0x01u << Motor_Left_Decoder_COUNTER_UNDERFLOW_SHIFT))
#define Motor_Left_Decoder_COUNTER_RESET              ((uint8) (0x01u << Motor_Left_Decoder_COUNTER_RESET_SHIFT))
#define Motor_Left_Decoder_INVALID_IN                 ((uint8) (0x01u << Motor_Left_Decoder_INVALID_IN_SHIFT))

#define Motor_Left_Decoder_INTERRUPTS_ENABLE_SHIFT    (0x04u)
#define Motor_Left_Decoder_INTERRUPTS_ENABLE          ((uint8)(0x01u << Motor_Left_Decoder_INTERRUPTS_ENABLE_SHIFT))
#define Motor_Left_Decoder_INIT_INT_MASK              (0x0Fu)


/******************************************************************************************
* Following code are OBSOLETE and must not be used starting from Quadrature Decoder 2.20
******************************************************************************************/
#define Motor_Left_Decoder_DISABLE                    (0x00u)


#endif /* CY_QUADRATURE_DECODER_Motor_Left_Decoder_H */


/* [] END OF FILE */
