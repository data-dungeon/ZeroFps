#ifndef _BASIC_ZFASSERT_H_
#define _BASIC_ZFASSERT_H_

#include <cassert>
#include "basic_x.h"

enum ERROR_RESULT
{
	ZFA_IGNORE = 0,
	ZFA_CONTINUE,
	ZFA_BREAKPOINT,
	ZFA_ABORT
};

//#define ZFAssert(test, strdesc)	assert(test && strdesc)
ERROR_RESULT BASIC_API ZFADisplayError(char* szErrorTitle, char* szErrorText, char* szErrorDesc, char* szErrorFileName,
									  int iLineNumber);

void BASIC_API ZFADisplayError2(char* szError,char* szErrorFile,int iLineNumber);								  
									  
									  
#ifdef  NDEBUG
	#define ZFAssert(test, strdesc)	((void)0)
#else
	//windows
	#ifdef WIN32
		#define ZFAssert(test, strdesc) {\
			static bool _ignoreAssert = false;\
			if(!_ignoreAssert && !(test)) \
			{\
				ERROR_RESULT _err_result  = ZFADisplayError("Debug assert", #test, strdesc, __FILE__, __LINE__);\
				if(_err_result == ZFA_IGNORE)\
				{\
					_ignoreAssert = true;\
				}\
				else if ( _err_result == ZFA_BREAKPOINT)\
				{\
				_asm { int 3 };\
				}\
			}\
		}
	//other
	#else
		#define ZFAssert(test, strdesc) {\
			if(!(test)) \
			{\
				ZFADisplayError2(strdesc,__FILE__, __LINE__);\
			}\
		}
	#endif		
#endif	// NDEBUG


#endif


