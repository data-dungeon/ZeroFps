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
	//bool RemoveValue(string kValueName, unsigned int iIndex);
	bool SetValue(string kValueName ,string kValue);
	bool SetValue(string kValueName, unsigned int iIndex ,string kValue);
	bool CheckIfResize(string kValueName);
	int GetNumberOfValues(string kValueName);
	string GetValue(string kValueName);
	string GetValue(string kValueName, unsigned int iIndex);
	vector<string> GetValueNames();
	float GetUpperBound(string kValueName);
	float GetLowerBound(string kValueName);
	bool Resize(string kValueName, unsigned int uiNewSize);
	bool CheckIfVector(string kValueName);
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
		int iNumberOfValues;
		bool bResize;
		bool bVector;
		inline PropertyValues() : 
			fLowerBound(FLT_MIN),
			fUpperBound(FLT_MAX),
			iNumberOfValues(1),
			bResize(false),
			bVector(true) {} 
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

private:
	string ValueToString(void *pkValue, PropertyValues *pkPropertyValue); 
	bool StringToValue(string kValue, void *pkValue, PropertyValues *pkPropertyValue);
	//bool FindPropertyValue(string kValueName, PropertyValues* pkPropertyValues);
};

#endif // !defined(AFX_MAT_H__44E3E6D4_C30F_45E3_807E_F848F395A3FB__INCLUDED_)
