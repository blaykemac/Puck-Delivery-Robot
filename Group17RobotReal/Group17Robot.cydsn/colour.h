
/* ========================================
 *
 * Copyright Group 17, 2019
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Blayke, Misha, Nidhin.
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

void colourSensingInitialise();
void colourSensingCalibration();
void colourSensingDebug();
int colourSensingOutput(void);

extern int colour_flag;                 // determines which PHOTODIODE is being used 
extern int ColourSensingAlgorithm;      // determines which coloursensing algorithm to use 
extern char output[32];


/* [] END OF FILE */
