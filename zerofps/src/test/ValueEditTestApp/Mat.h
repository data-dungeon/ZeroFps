// Mat.h: interface for the Mat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAT_H__44E3E6D4_C30F_45E3_807E_F848F395A3FB__INCLUDED_)
#define AFX_MAT_H__44E3E6D4_C30F_45E3_807E_F848F395A3FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include <iostream>
#include <cfloat>
#include "vector3.h"
#include "vector4.h"

#pragma warning (disable : 4786)

using namespace std;

class Mat  
{
public:
	bool SetValue(string kValueName ,string kValue);
	bool SetValue(string kValueName, int iIndex ,string kValue);
	int GetNumberOfValues(string kValueName);
	string GetValue(string kValueName);
	string GetValue(string kValueName, int iIndex);
	vector<string> GetValueNames();
	Mat();
	virtual ~Mat();

protected:
	struct PropertyValues
	{
		string kValueName;
		void* pkValue;
		int iValueType;
		float fUpperBound;
		float fLowerBound;
		PropertyValues();
		int iNumberOfValues;
	};	
	
	enum ValueTypes
	{
		VALUETYPE_INT,
		VALUETYPE_FLOAT,
		VALUETYPE_BOOL,
		VALUETYPE_STRING,
		VALUETYPE_VECTOR3,
		VALUETYPE_VECTOR4,
	};
	virtual vector<PropertyValues> GetPropertyValues();
};

#endif // !defined(AFX_MAT_H__44E3E6D4_C30F_45E3_807E_F848F395A3FB__INCLUDED_)
