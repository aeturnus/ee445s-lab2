// Welch, Wright, & Morrow, 
// Real-time Digital Signal Processing, 2011

///////////////////////////////////////////////////////////////////////
// Filename: ISRs.c
//
// Synopsis: Interrupt service routine for codec data transmit/receive
//
///////////////////////////////////////////////////////////////////////

#include "DSP_Config.h" 
#include <math.h>   
  
// Data is received as 2 16-bit words (left/right) packed into one
// 32-bit word.  The union allows the data to be accessed as a single 
// entity when transferring to and from the serial port, but still be 
// able to manipulate the left and right channels independently.

#define LEFT  0
#define RIGHT 1

volatile union {
	Uint32 UINT;
	Int16 Channel[2];
} CodecDataIn, CodecDataOut;


/* add any global variables here */
float A = 32000;		/* signal's amplitude */
float fDesired_l = 2000;  /* signal's frequency */
float fDesired_r = 6000;  /* signal's frequency */
float phase = 0;        /* signal's initial phase */

float pi = 3.1415927;	/* value of pi */
float phaseIncrement;   /* incremental phase */

Int32 fs =  8000;       /* sample frequency */

static float x_1_coeff_l = 0.0f;
static float y_1_coeff_l = 0.0f;
static float x_1_coeff_r = 0.0f;
static float y_1_coeff_r = 0.0f;

void Codec_ISR_Init()
{
	x_1_coeff_l = cosf(2 * pi * fDesired_l/((float)fs));
	y_1_coeff_l = 2 * x_1_coeff_l;	// we only to compute once

	x_1_coeff_r = cosf(2 * pi * fDesired_r/((float)fs));
	y_1_coeff_r = 2 * x_1_coeff_r;	// we only to compute once
}

interrupt void Codec_ISR()
///////////////////////////////////////////////////////////////////////
// Purpose:   Codec interface interrupt service routine  
//
// Input:     None
//
// Returns:   Nothing
//
// Calls:     CheckForOverrun, ReadCodecData, WriteCodecData
//
// Notes:     None
///////////////////////////////////////////////////////////////////////
{                    
	/* add any local variables here */
	//static float x_1_coeff = 0.707106781186547524400844362104849039284835937688474036588f;
	//static float y_1_coeff = 2 * 0.707106781186547524400844362104849039284835937688474036588f;	// we only to compute once

 	if(CheckForOverrun())					// overrun error occurred (i.e. halted DSP)
		return;								// so serial port is reset to recover

  	CodecDataIn.UINT = ReadCodecData();		// get input data samples
	
	/* algorithm begins here */
	//phaseIncrement = 2*pi*fDesired/fs;  /* calculate the phase increment */
	//phase += phaseIncrement;            /* calculate the next phase */
	
	//if (phase >= 2*pi) phase -= 2*pi;    /* modulus 2*pi operation */

	static float y_l[3] = {0.0f,0.0f,0.0f};
	static float y_r[3] = {0.0f,0.0f,0.0f};
	static float x[2] = {1.0f,0.0f};

	y_l[0] = y_1_coeff_l * y_l[1] - y_l[2] + x[0] - x_1_coeff_l * x[1];
	y_r[0] = y_1_coeff_r * y_r[1] - y_r[2] + x[0] - x_1_coeff_r * x[1];
	
	y_l[2] = y_l[1];
	y_l[1] = y_l[0];
	y_r[2] = y_r[1];
	y_r[1] = y_r[0];
	x[1] = x[0];
	x[0] = 0.0f;

	CodecDataOut.Channel[ LEFT] = A*y_l[0]; /* scaled L output */
	CodecDataOut.Channel[RIGHT] = A*y_r[0]; /* scaled R output */
	/* algorithm ends here */

	WriteCodecData(CodecDataOut.UINT);		// send output data to  port
}

