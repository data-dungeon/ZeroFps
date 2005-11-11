#include "globals.h"
#include "zfsystem.h"
#include "zfassert.h"
#include <cstdio>
#include <cstdarg>
#include "math.h"

using namespace std;

extern ZFSystem g_ZFObjSys;

#define NRDEG_IN_ONE_RAD	57.29577951
#define NRRAD_IN_ONE_DEG	0.0174532952

// const float Math::PI			= 3.1415926535897932384626433832795f;
// const float Math::TWO_PI	= 2.0 * Math::PI;
// const float Math::HALF_PI  = 0.5 * Math::PI;
// const float Math::EPSILON  = 1.0e-4f;

float BASIC_API Math::DegToRad(float fAngle)
{
	return float((fAngle * NRRAD_IN_ONE_DEG));
}

float BASIC_API Math::RadToDeg(float fAngle)
{
	return float((fAngle * NRDEG_IN_ONE_RAD));
}


int BASIC_API Math::PowerOf2(int iVal)
{
	int list[] = {1,2,4,8,16,32,64,128};

	int smalest=999999999;
	int tal=0; // 040701: int tal;
	int blub;

	for(int i=0;i<7;i++)
	{
		blub=abs(iVal-list[i]);
		
		if( blub < smalest)
		{
			smalest=blub;
			tal=list[i];
		}		
	}
	
	return tal;
}

// Base Changes, Tarnsformations and other functions the nobody in the zerofps really understand ;).
Vector3 Math::GetYawVector2(float fAngleDeg)
{
	Vector3 kYaw;
	kYaw.x = float(cos(DegToRad(fAngleDeg)));	
	kYaw.y = 0;
	kYaw.z = float(sin(DegToRad(fAngleDeg)));	
	return kYaw;
}


float Math::GetYawAngle(Vector3 kDir)
{
	float fAngle =  float(atan2(kDir.x, - kDir.z));
	return RadToDeg(fAngle);
}

float Math::round2(float val)
{
/*	int lower = (int) val;
	if( (val - lower) > 0.499f)
		return (float) lower + 1.0f;
	return (float) lower;*/

/*	if(val < 0)
		return -((float) (int) (fabs(val) + 0.5f));
	else
		return (float) (int) (val + 0.5f);
*/		

	if(val < 0)
		return (float) (int) (val - 0.5f);
	else
		return (float) (int) (val + 0.5f);

}


float Math::Randomf(float fInterval)
{
	float r = (float)rand() / (float)(RAND_MAX);
	return r*(fInterval);
}

int Math::Randomi(int iInterval)
{
	float r = (float)rand() / (float)(RAND_MAX);
	return int(r * iInterval );
}


bool Math::equal_f(float a, float b)
{
	return( (a > (b - EPSILON) ) && (a < (b + EPSILON)) );
}

float Math::Clamp(float fValue,float fBotom,float fTop)
{
	bool bChanged=true;
	float fDiff = fTop - fBotom;
	
	while(bChanged)
	{
		bChanged = false;
		
		if(fValue < fBotom)
		{	
			fValue+=fDiff;
			bChanged=true;
		}
		if(fValue > fTop)
		{	
			fValue-=fDiff;
			bChanged=true;
		}
	}

	return fValue;
}

