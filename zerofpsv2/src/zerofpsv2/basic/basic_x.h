#ifndef _BASIC_BASICX_H_
#define _BASIC_BASICX_H_

/**
	\defgroup Basic Basic.Dll
	\ingroup ZeroFPS

	
*/

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TESTDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TESTDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#if defined(WIN32) 

#ifdef BASIC_EXPORTS
#define BASIC_API __declspec(dllexport)
#else
#define BASIC_API __declspec(dllimport)
#endif

#pragma warning( disable : 4251 )	// class 'type' needs to have dll-interface 
#pragma warning( disable : 4305 )	// truncation from 'type1' to 'type2'
#pragma warning( disable : 4786 )	// identifier was truncated to 'number' characters in the debug information

#endif

#if !defined(WIN32) 
#define BASIC_API 
#endif




#endif







