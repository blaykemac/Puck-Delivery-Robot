/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
//#ifndef _COLOUR_H
//#define _COLOUR_H

// Colour Sensing Constants
#define BLANK 0
#define RED 1
#define GREEN 2
#define BLUE 3

// * INSTANTIATING FUNCTIONS * //

void RGB1Initialise(void);
void ColourSensingCalibrationRGB1();
void ColourSensingCalibrationRGB2();
void RGB1Initialise();
void RGB2Initialise();
void ColourSensingRGB1();
void ColourSensingRGB2();
void ColourOutput();
void ColourDebug();


void ColourIntialiseViaHardware();
void ColourSensingViaHardware();

int redDetection(void);



/* [] END OF FILE */
