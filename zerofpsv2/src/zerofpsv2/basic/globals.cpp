/**	\file 
	Contains Global Functions.
	Global functions are support functions that are usefull put don't fit
	well into any class or are waiting to be moved into a class.
*/

#include "globals.h"
#include "zfobjectmanger.h"
#include <cstdio>
#include <cstdarg>
using namespace std;


#define NRDEG_IN_ONE_RAD	57.29577951
#define NRRAD_IN_ONE_DEG	0.0174532952

extern ZFSystem g_ZFObjSys;

void Gemens(char* aText) 
{
	int i=0;
	
	while(aText[i]!='\0'){
		//gems  96>x<123     vers  64>x<91
		int code=int(aText[i]);
		
		if(code>64 && code<91)
			aText[i]=char(code+32);

		i++;
	}
};

// Compares strings and ignore case
bool IsSameIgnoreCase(const char* szString1,const char* szString2)
{
	char szCheck1[256];
	char szCheck2[256];

/*	if(strlen(szString1) >= 256)
		assert(0);
	if(strlen(szString2) >= 256)
		assert(0);
*/
	strcpy(szCheck1, szString1);
	strcpy(szCheck2, szString2);

	Gemens(szCheck1);
	Gemens(szCheck2);
	
	if(strcmp(szCheck1, szCheck2) == 0)
		return true;

	return false;
}


void IntToChar(char* aText,int iValue)
{
	if(iValue == 0)
		strcpy(aText,"0");
	else
		strcpy(aText,"");	
	bool found=false;
	
	for(int i=10000000;i>0;i/=10) {
		if(iValue/i>0){
			int nr=int(iValue/i);
//			cout<<nr;
			iValue-=i*nr;
			found=true;
			nr+=48;
			strncat(aText,(char*)&(nr),1);
		} else {
			if(found==true)
				strcat(aText,"0");			
//			cout<<0;		
		}
	}
}
	
float BASIC_API DegToRad(float fAngle)
{
	return float((fAngle * NRRAD_IN_ONE_DEG));
}

float BASIC_API RadToDeg(float fAngle)
{
	return float((fAngle * NRDEG_IN_ONE_RAD));
}

int BASIC_API PowerOf2(int iVal)
{
	int list[] = {1,2,4,8,16,32,64,128};

	int smalest=999999999;
	int tal;
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
Vector3 GetYawVector2(float fAngleDeg)
{
	Vector3 kYaw;
	kYaw.x = float(cos(DegToRad(fAngleDeg)));	
	kYaw.y = 0;
	kYaw.z = float(sin(DegToRad(fAngleDeg)));	
	return kYaw;
}


float GetYawAngle(Vector3 kDir)
{
	float fAngle =  float(atan2(kDir.x, - kDir.z));
	return RadToDeg(fAngle);
}

//char LogFormatTxt[4096];	
char g_szFormatText[4096];	

void g_Logf(const char *fmt, ...)
{
	va_list		ap;							// Pointer To List Of Arguments

	// Make sure we got something to work with.
	if (fmt == NULL)	return;					

	va_start(ap, fmt);						// Parses The String For Variables
		vsprintf(g_szFormatText, fmt, ap);		// And Convert Symbols
	va_end(ap);								// 

	// Now call our print function.
	g_ZFObjSys.Log(g_szFormatText);
}

bool equal_f(float a, float b)
{
	return( (a > (b - epsilon) ) && (a < (b + epsilon)) );
}

float Clamp(float fValue,float fBotom,float fTop)
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

Vector3 RndColor()
{
	Vector3 kColor;
	kColor.x = rand()%255;
	kColor.y = rand()%255;
	kColor.z = rand()%255;
	kColor.Normalize();

	return kColor;
}

