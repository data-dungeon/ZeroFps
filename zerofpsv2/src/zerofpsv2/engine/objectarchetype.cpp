#include "object.h"

// **************************** PropertyValue ****************************
PropertyValue::PropertyValue()
{
	m_strVariable = "";
	m_strValue = "";
}
 
PropertyValue::~PropertyValue()
{

}

PropertyValue::PropertyValue(const PropertyValue &Other)
{
	m_strVariable = Other.m_strVariable;
	m_strValue = Other.m_strValue;
}

PropertyValue& PropertyValue::operator=(const PropertyValue &Other)
{
	m_strVariable = Other.m_strVariable;
	m_strValue = Other.m_strValue;
	return *this;
}


// **************************** PropertyArcheType ****************************

PropertyArcheType::PropertyArcheType()
{
	m_strName = "";
}

PropertyArcheType::~PropertyArcheType()
{
	
}

PropertyArcheType::PropertyArcheType(const PropertyArcheType &Other)
{
	m_strName = Other.m_strName;
	m_kVariables = Other.m_kVariables;
}

PropertyArcheType& PropertyArcheType::operator=(const PropertyArcheType &Other)
{
	m_strName = Other.m_strName;
	m_kVariables = Other.m_kVariables;
	return *this;
}


// **************************** ObjectArcheType ****************************

ObjectArcheType::ObjectArcheType()
{
	m_strName			= "";
	m_strParentName	= "" ;
}

ObjectArcheType::~ObjectArcheType()
{

}

void ObjectArcheType::AddArchProperty(string strArchPropertys)
{
	PropertyArcheType kAProperty;
	kAProperty.m_strName = strArchPropertys;
	m_kArchPropertys.push_back(kAProperty);
}

PropertyArcheType* ObjectArcheType::GetArchProperty(string strArchPropertys)
{
	for(unsigned int i=0; i<m_kArchPropertys.size(); i++) 
	{
		if(strArchPropertys == m_kArchPropertys[i].m_strName)
			return &m_kArchPropertys[i];
	}

	return NULL;
}

PropertyArcheType* ObjectArcheType::GetAddArchProperty(string strArchPropertys)
{
	PropertyArcheType* pkAt = GetArchProperty(strArchPropertys);
	if(!pkAt) {
		AddArchProperty( strArchPropertys );
		pkAt = GetArchProperty(strArchPropertys);
		}

	return pkAt;
}

void ObjectArcheType::SetValue(string strProperty, string strVariable, string Value)
{
	PropertyArcheType* pkAt = GetAddArchProperty(strProperty);
	PropertyValue kPValue;
	kPValue.m_strVariable = strVariable;
	kPValue.m_strValue = Value;

	pkAt->m_kVariables.push_back(kPValue);
		
}

ObjectArcheType::ObjectArcheType(const ObjectArcheType &Other)
{
	m_strName = Other.m_strName;
	m_strParentName = Other.m_strParentName;
	m_kArchPropertys = Other.m_kArchPropertys;
}

ObjectArcheType& ObjectArcheType::operator=(const ObjectArcheType &Other)
{
	m_strName = Other.m_strName;
	m_strParentName = Other.m_strParentName;
	m_kArchPropertys = Other.m_kArchPropertys;
	return *this;
}

