//
//  C700DSP.cpp
//  C700
//
//  Created by osoumen on 2014/11/30.
//
//

#include "C700DSP.h"
#include "gauss.h"

#define filter1(a1)	(( a1 >> 1 ) + ( ( -a1 ) >> 5 ))
#define filter2(a1,a2)	(a1 + ( ( -( a1 + ( a1 >> 1 ) ) ) >> 5 ) - ( a2 >> 1 ) + ( a2 >> 5 ))
#define filter3(a1,a2)	(a1  + ( ( -( a1 + ( a1 << 2 ) + ( a1 << 3 ) ) ) >> 7 )  - ( a2 >> 1 )  + ( ( a2 + ( a2 >> 1 ) ) >> 4 ))

static const int	*G1 = &gauss[256];
static const int	*G2 = &gauss[512];
static const int	*G3 = &gauss[255];
static const int	*G4 = &gauss[0];

static const int	CNT_INIT = 0x7800;
static const int	ENVCNT[32]
= {
    0x0000, 0x000F, 0x0014, 0x0018, 0x001E, 0x0028, 0x0030, 0x003C,
    0x0050, 0x0060, 0x0078, 0x00A0, 0x00C0, 0x00F0, 0x0140, 0x0180,
    0x01E0, 0x0280, 0x0300, 0x03C0, 0x0500, 0x0600, 0x0780, 0x0A00,
    0x0C00, 0x0F00, 0x1400, 0x1800, 0x1E00, 0x2800, 0x3C00, 0x7800
};

static unsigned char silence_brr[] = {
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//-----------------------------------------------------------------------------
void C700DSP::VoiceState::Reset()
{
	smp1=0;
	smp2=0;
	sampbuf[0]=0;
	sampbuf[1]=0;
	sampbuf[2]=0;
	sampbuf[3]=0;
	
	pb = 0;
	reg_pmod = 0;
	vibdepth = 0;
	vibPhase = 0.0f;
    portaPitch = .0f;
    pan = 64;
	
	brrdata = silence_brr;
	loopPoint = 0;
	loop = false;
	pitch = 0;
	memPtr = 0;
	headerCnt = 0;
	half = 0;
	envx = 0;
	end = 0;
	sampptr = 0;
	mixfrac=0;
	envcnt = CNT_INIT;
	envstate = RELEASE;
}

C700DSP::C700DSP() : mNewADPCM( false )
{
    //Initialize
	mMainVolume_L = 127;
	mMainVolume_R = 127;
}

void C700DSP::ResetVoice(int voice)
{
    mVoice[voice].Reset();
}

void C700DSP::ResetEcho()
{
    mEcho[0].Reset();
	mEcho[1].Reset();
}

void C700DSP::SetVoiceLimit(int value)
{
    mVoiceLimit = value;
}

void C700DSP::SetNewADPCM(bool value)
{
    mNewADPCM = value;
}

void C700DSP::SetMainVolumeL(int value)
{
    mMainVolume_L = value;
}

void C700DSP::SetMainVolumeR(int value)
{
    mMainVolume_R = value;
}

void C700DSP::SetEchoVol_L( int value )
{
	mEcho[0].SetEchoVol( value );
}

void C700DSP::SetEchoVol_R( int value )
{
	mEcho[1].SetEchoVol( value );
}

void C700DSP::SetFeedBackLevel( int value )
{
	mEcho[0].SetFBLevel( value );
	mEcho[1].SetFBLevel( value );
}

void C700DSP::SetDelayTime( int value )
{
	mEcho[0].SetDelayTime( value );
	mEcho[1].SetDelayTime( value );
}

void C700DSP::SetFIRTap( int tap, int value )
{
	mEcho[0].SetFIRTap(tap, value);
	mEcho[1].SetFIRTap(tap, value);
}

void C700DSP::KeyOffVoice(int v)
{
    mVoice[v].envstate = RELEASE;
}

void C700DSP::KeyOnVoice(int v)
{
    mVoice[v].memPtr = 0;
    mVoice[v].headerCnt = 0;
    mVoice[v].half = 0;
    mVoice[v].envx = 0;
    mVoice[v].end = 0;
    mVoice[v].sampptr = 0;
    mVoice[v].mixfrac = 3 * 4096;
    mVoice[v].envcnt = CNT_INIT;
    mVoice[v].envstate = ATTACK;
}

void C700DSP::SetAR(int v, int value)
{
    mVoice[v].ar = value;
}
void C700DSP::SetDR(int v, int value)
{
    mVoice[v].dr = value;
}
void C700DSP::SetSL(int v, int value)
{
    mVoice[v].sl = value;
}
void C700DSP::SetSR(int v, int value)
{
    mVoice[v].sr = value;
}
