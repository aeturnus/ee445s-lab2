// Welch, Wright, & Morrow, 
// Real-time Digital Signal Processing, 2011

///////////////////////////////////////////////////////////////////////
// Filename: StartUp.c
//
// Synopsis: Placeholder for code run after DSP_Init()
//
///////////////////////////////////////////////////////////////////////

#include "DSP_Config.h"

extern void Codec_ISR_Init(void);
void StartUp()
{
	Codec_ISR_Init();
}
