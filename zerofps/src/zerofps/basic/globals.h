#ifndef _BASIC_GLOBALS_H_
#define _BASIC_GLOBALS_H_

#include <cstring>
#include <iostream>
#include "basic_x.h"
#include "os.h"
#include <cmath>

#include "vector3.h"

using namespace std;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

float const degtorad = float(57.3248);

float BASIC_API DegToRad(float fAngle);				///< Convert a angle in degrees to radians.
float BASIC_API RadToDeg(float fAngle);				///< Convert a angle in radians to degrees.

void BASIC_API Gemens(char* aText);					///< Turn a string into lowcase.
void BASIC_API IntToChar(char* aText,int iValue);

int BASIC_API PowerOf2(int iVal);


Vector3 BASIC_API GetYawVector2(float fAngleDeg);

float BASIC_API GetYawAngle(Vector3 kDir);

char* BoolStr(bool bFlag);
void BASIC_API g_Logf(const char *fmt, ...);

bool BASIC_API IsSameIgnoreCase(const char* szString1, const char* szString2);

extern char g_szFormatText[4096];	

enum BASIC_API GlobalButton
{
	gKEY_ESCAPE=0,
	gKEY_LEFT,
	gKEY_RIGHT,
	gKEY_UP,
	gKEY_DOWN,
	gKEY_SPACE,
	gKEY_BACKSPACE,
	gKEY_RETURN,
	gKEY_RSHIFT,
	gKEY_RCTRL,
	gKEY_LSHIFT,
	gKEY_LCTRL,
	gKEY_F12,
	gKEY_F11,
	gKEY_F10,
	gKEY_F9,	
	gKEY_F8,	
	gKEY_F7,	
	gKEY_F6,
	gKEY_F5,	
	gKEY_F4,	
	gKEY_F3,	
	gKEY_F2,
	gKEY_F1,
	gKEY_PAGEUP,
	gKEY_PAGEDOWN,
	gKEY_HOME,
	gKEY_END,
	gKEY_TAB,
	gKEY_DELETE,
	gKEY_INSERT,	
	gKEY_1,
	gKEY_2,
	gKEY_3,
	gKEY_4,
	gKEY_5,
	gKEY_6,
	gKEY_7,
	gKEY_8,
	gKEY_9,
	gKEY_0,
	gKEY_Q,
	gKEY_W,	
	gKEY_E,	
	gKEY_R,	
	gKEY_T,	
	gKEY_Y,	
	gKEY_U,	
	gKEY_I,	
	gKEY_O,	
	gKEY_P,	
	gKEY_A,	
	gKEY_S,	
	gKEY_D,	
	gKEY_F,	
	gKEY_G,	
	gKEY_H,	
	gKEY_J,	
	gKEY_K,	
	gKEY_L,	
	gKEY_Z,	
	gKEY_X,	
	gKEY_C,	
	gKEY_V,	
	gKEY_B,	
	gKEY_N,	
	gKEY_M,
	gKEY_EXCLAIM,
	gKEY_QUOTEDBL,
	gKEY_HASH,
	gKEY_DOLLAR,
	gKEY_AMPERSAND,
	gKEY_QUOTE,
	gKEY_LEFTPAREN,
	gKEY_RIGHTPAREN,
	gKEY_ASTERISK,
	gKEY_PLUS,
	gKEY_COMMA,
	gKEY_MINUS,
	gKEY_PERIOD,
	gKEY_SLASH,
	gKEY_COLON,
	gKEY_SEMICOLON,
	gKEY_LESS,
	gKEY_EQUALS,
	gKEY_GREATER,
	gKEY_QUESTION,
	gKEY_AT,
	gKEY_LEFTBRACKET,
	gKEY_BACKSLASH,
	gKEY_RIGHTBRACKET,
	gKEY_CARET,
	gKEY_UNDERSCORE,
	gKEY_BACKQUOTE,
	gKEY_KP_PERIOD,
	gKEY_KP_DIVIDE,
	gKEY_KP_MULTIPLY,
	gKEY_KP_MINUS,
	gKEY_KP_PLUS,
	gKEY_KP_ENTER,
	gKEY_KP_EQUALS,
	gKEY_NUMLOCK,
	gKEY_CAPSLOCK,
	gKEY_SCROLLOCK,
	gMOUSELEFT,
	gMOUSEMIDDLE,
	gMOUSERIGHT
};

#endif
