/**	\file 
	Contains Global Functions.
	Global functions are support functions that are usefull put don't fit
	well into any class or are waiting to be moved into a class.
*/

#include "globals.h"
#include "zfsystem.h"
#include "zfassert.h"
#include <cstdio>
#include <cstdarg>
//#include <conio.h>
using namespace std;


#define NRDEG_IN_ONE_RAD	57.29577951
#define NRRAD_IN_ONE_DEG	0.0174532952

extern ZFSystem g_ZFObjSys;

void BASIC_API StringReverse(char* czString)
{
	int iLength = strlen(czString);
	
	char cBakup;
	for(int i = 0;i<iLength;i++)
	{
		cBakup = czString[i];
		czString[i] = czString[ (iLength-1) - i];
		czString[ (iLength-1) - i] = cBakup;
	}

}



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

void ConvertToLower(string& strText)
{
	//gems  96>x<123     vers  64>x<91
	
	int iCode;
	for(int i = 0;i<strText.length();i++)
	{
		iCode = int(strText[i]);		
		if(iCode>64 && iCode<91)
			strText[i]=char(iCode+32);
	}	
}

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
			iValue-=i*nr;
			found=true;
			nr+=48;
			strncat(aText,(char*)&(nr),1);
		} else {
			if(found==true)
				strcat(aText,"0");			
		}
	}
}

string BASIC_API IntToString(int iValue)
{
	static char temp[32];
	sprintf(temp,"%d",iValue);	
	
	return temp;
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
	kColor.x = float( rand()%255 );
	kColor.y = float( rand()%255 );
	kColor.z = float( rand()%255 );
	kColor.Normalize();

	return kColor;
}

float round2(float val)
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


float Randomf(float fInterval)
{
	float r = (float)rand() / (float)(RAND_MAX);
	return r*(fInterval);
}

int Randomi(int iInterval)
{
	float r = (float)rand() / (float)(RAND_MAX);
	return int(r * iInterval );
}


void ZFError(const char *fmt, ...)
{
	va_list		ap;							// Pointer To List Of Arguments

	// Make sure we got something to work with.
	if (fmt == NULL)	return;					

	va_start(ap, fmt);						// Parses The String For Variables
		vsprintf(g_szFormatText, fmt, ap);		// And Convert Symbols
	va_end(ap);								// 

	// Now call our print function.
	cout << "ERROR: " <<  g_szFormatText << endl;
//	getch();
	exit(1);
}

void ZFWarning(const char *fmt, ...)
{
	va_list		ap;							// Pointer To List Of Arguments

	// Make sure we got something to work with.
	if (fmt == NULL)	return;					

	va_start(ap, fmt);						// Parses The String For Variables
		vsprintf(g_szFormatText, fmt, ap);		// And Convert Symbols
	va_end(ap);								// 

	// Now call our print function.
	cout << "Warning: " <<  g_szFormatText << endl;
}

void BASIC_API ZFADisplayError2(char* szError,char* szErrorFile,int iLineNumber)
{
	cout<<"ERROR:"<<szError<<" File:"<<szErrorFile<< " Line:"<<iLineNumber<<endl;
	exit(-1);
}							  

#ifdef WIN32
#include <windows.h>

ERROR_RESULT ZFADisplayError(char* szErrorTitle, char* szErrorText, char* szErrorDesc, char* szErrorFileName,
									  int iLineNumber)
{
	// Create Error Message.
	const int iMaxMessageSize = 1024;
	char acErrMessage[iMaxMessageSize];

	sprintf(acErrMessage, 
		" File: %s\n Line: %d\n Error: %s\n Comment: %s\n\n Abort to exit (or debug), Retry to continue, Ignore to disregard all occurrences of this error.",
		szErrorFileName, iLineNumber, szErrorText, szErrorDesc);

	// Find the top most window.
	HWND hWndParent = GetActiveWindow();
	if( NULL != hWndParent) 
	{
		hWndParent = GetLastActivePopup( hWndParent );
	}

	int iRet = MessageBox(hWndParent, acErrMessage, szErrorTitle, MB_TASKMODAL | MB_SETFOREGROUND | 
		MB_ABORTRETRYIGNORE | MB_ICONERROR);

	if(iRet == IDRETRY)
		return ZFA_CONTINUE;

	if(iRet == IDIGNORE)
		return ZFA_IGNORE;

	iRet = MessageBox(hWndParent, "Debug last Error", "Debug or exit?", MB_TASKMODAL | MB_SETFOREGROUND |
		MB_YESNO | MB_ICONQUESTION);

	if(iRet == IDYES)
		return ZFA_BREAKPOINT;

	exit(-1);
//	return ZFA_ABORT; 040701: 	return ZFA_ABORT;
}

bool GetClipboardText(string& r_strText)
{
	LPTSTR    lptstr; 
	HGLOBAL   hglb;

	if(!IsClipboardFormatAvailable(CF_TEXT) || !OpenClipboard(NULL))
		return false;

	hglb = (char*) GetClipboardData(CF_TEXT);
	if (hglb != NULL)
	{
		lptstr = (LPTSTR)GlobalLock(hglb);
      GlobalUnlock(hglb);
      CloseClipboard();
      r_strText = lptstr;
		return true;
	}

	return false;
}

#endif // Win32



#ifndef WIN32

ERROR_RESULT ZFADisplayError(char* szErrorTitle, char* szErrorText, char* szErrorDesc, char* szErrorFileName,
									  int iLineNumber)
{
	cout << "szErrorTitle: " << szErrorTitle << endl;
	cout << "szErrorText: " << szErrorText << endl;
	cout << "szErrorDesc: " << szErrorDesc << endl;
	cout << "szErrorFileName: " << szErrorFileName << endl;
	cout << "iLineNumber: " << iLineNumber << endl;

	exit(-1);
	return ZFA_ABORT;
}

bool GetClipboardText(string& r_strText)
{
	// zeb: Dvoid kan du skriva en funktion som h�tar clipboard datat i Linux milj�	// och sparar ner det i r_strText? Returnera true om texten gick att f�
	return false;
}

#endif // !WIN32




