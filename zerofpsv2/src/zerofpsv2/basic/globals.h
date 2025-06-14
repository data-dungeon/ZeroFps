#ifndef _BASIC_GLOBALS_H_
#define _BASIC_GLOBALS_H_

#include <iostream>
#include <cmath>
#include "basic_x.h"
#include "vector3.h"

using namespace std;

#ifdef WIN32

	#define Max(a,b)            (((a) > (b)) ? (a) : (b))
	#define Min(a,b)            (((a) < (b)) ? (a) : (b))

#endif 

#ifndef WIN32

	template <class Any> 
	inline float BASIC_API Max(Any a,Any b)
	{
		if(a > b)
			return a;
		else
			return b;
	}

	template <class Any> 
	inline float BASIC_API Min(Any a,Any b)
	{
		if(a < b)
			return a;
		else
			return b;
	}

#endif 


//EVIL hash_map fix
// namespace std 
// { 
// 	template<> struct hash< std::string > 
// 	{ 
// 		size_t operator()( const std::string& x ) const 
// 		{ 
// 			return hash< const char* >()( x.c_str() ); 
// 		} 
// 	}; 
// }

//some constants
//#define PId2	1.5707963279489
//#define PI		3.1415926535897932384626433832795f
//#define zf_pi	6.28318531f
//#define epsilon 1.0e-4f

//float const degtorad = float(57.2957795130823208767981548141052f);

//character handling
void BASIC_API ConvertToLower(string& strText);
void BASIC_API Gemens(char* aText);									///< Turn a string into lowcase.
void BASIC_API IntToChar(char* aText,int iValue);				//
bool BASIC_API IsSameIgnoreCase(const char* szString1, const char* szString2);
void BASIC_API StringReverse(char* czString);
string BASIC_API IntToString(int iValue);

//error handling
extern char g_szFormatText[4096];	
BASIC_API void ZFError(const char *fmt, ...);
BASIC_API void ZFWarning(const char *fmt, ...);
void BASIC_API g_Logf(const char *fmt, ...);

//strange funktions =D
Vector3 BASIC_API RndColor();												//randomize a colorvector...i think

//profiling
#define NOPROFILE
#ifdef NOPROFILE
	#define StartProfileTimer(czName)	((void)0)
	#define StopProfileTimer(czName)	((void)0)
#else
	#define StartProfileTimer(czName) {\
		g_ZFObjSys.DontUseStartProfileTimer(czName);\
		}
		
	#define StopProfileTimer(czName) {\
		g_ZFObjSys.DontUseStopProfileTimer(czName);\
		}	
#endif


//logging
//#define NOLOG
// #ifdef NOLOG
// 	#define LOGF(czName,...) ((void)0)
// #else
// 	#define LOGF(czName,...) {\
// 		g_ZFObjSys.Logf(czName,__VA_ARGS__);\
// 		}
// #endif


bool BASIC_API GetClipboardText(string& r_strText);


#endif
