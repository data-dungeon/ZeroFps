#ifndef _BASICX_H_
#define _BASICX_H_

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

#endif

#if !defined(WIN32) 
#define BASIC_API 
#endif




#endif







