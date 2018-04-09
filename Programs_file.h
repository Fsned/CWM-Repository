/**********************************************************************************************
 * Header file : Programs_file.h
 * Author : Frederik Snedevind
 *		    Frs@Jeros.com
 *
 * Company: Jeros A/S
 *		   HTTP://www.Jeros.com/
 *
 * Date created : 9/4 - 2018
 **********************************************************************************************
 * Description:
 * 
 *  
 *	
 *
 *********************************************************************************************/
 #include "stdutils.h"
 
// ****************************************************************************************
//
//					Constants
//
// ****************************************************************************************

#define READY_FOR_NEW_PROGRAM					1
#define OCCUPIED_WITH_PROGRAM					2
#define FINISHING_UP									3
#define STOPPED_FOR_SENSORS						4


#define MAX_NUMBER_OF_OPERATIONS 			16


char CONSTANT_STRINGS[16][25] = {	{"Ready for new program"},			// S0
																	{"Occupied with program"},			// S1
																	{"Finishing up"},								// S2
																	{"Stopped by sensors"},					// S3
																	{"N/A"},												// S4
																	{"N/A"},												// S5													
																	{"N/A"},												// S6
																	{"N/A"},												// S7
																	{"N/A"},												// S8
																	{"N/A"},												// S9
																	{"N/A"},												// S10
																	{"N/A"},												// S11
																	{"N/A"},												// S12
																	{"N/A"},												// S13
																	{"N/A"},												// S14
																	{"N/A"}}; 											// S15
																	

// ****************************************************************************************
//	Type		: 	YES_RETURN Functions
//	Example		:	yXxXxX();
//	Description	:	Returns true (1) or false (0) depending on the success of the function
// ***************************************************************************************
extern void yWashProgram_1( void );
																	
extern void yWashProgram_2( void );
	
extern void yWashProgram_3( void );

// ****************************************************************************************
//	Type		: 	NO_RETURN Functions
//	Example		:	nXxXxX();
//	Description	:	Does not return anything.
// ****************************************************************************************
extern void nFillTanksOperation( void );

extern void nFillSoapOperation ( void );

extern void nWashOperation		 ( void );

extern void nRinseOperation	 	 ( void );

extern void nWaitOperation		 ( void );

extern void nFillSoftenerOperation ( void );
// ****************************************************************************************
//	Type		: 	VALUE_RETURN Functions
//	Example		:	vXxXxX();
//	Description	:	Returns a value, no confirmation if successful or not. can be a handle for e.g. ADC pin
// ****************************************************************************************

