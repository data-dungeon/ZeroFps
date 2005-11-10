#include "property.h"
#include "../basic/zfsystem.h"

#include "zerofps.h"
 
#include <bitset>
 
 
 

PropertyValues::PropertyValues(const string& strName,ValueTypes eValueType,void* pkValue)
{	
	m_strValueName = 	strName;
	m_eValueType = 	eValueType;
	m_pkValue = 		pkValue;		
	
	m_fLowerBound = 		FLT_MIN;
	m_fUpperBound = 		FLT_MAX;
	m_iNumberOfValues = 	1;
	m_bResize = 			false;
	m_bVector = 			false;
	
}

 
 
 
Property::Property(const char* czName)
{
	m_pkZeroFps				= static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));		
	m_pkEntityManager   	= static_cast<ZSSEntityManager*>(g_ZFObjSys.GetObjectPtr("ZSSEntityManager"));
	
	m_iType=				PROPERTY_TYPE_NORMAL;
	m_iSide=				PROPERTY_SIDE_SERVER;	
	m_bNetwork = 		false;
	m_bSave = 			true;	
	m_pkEntity=			NULL;	
	m_iSortPlace=		0;
	m_bSortDistance=	false;
	m_bReverseSort=	false;
	m_iVersion =		1;
	
	strcpy(m_acName,czName);		 

	SetNrOfConnections(m_pkZeroFps->GetMaxPlayers());	
}

Property::~Property()
{

}

// Game Messages
void Property::OnEvent(GameMessage& Msg)	
{	
	cout << "[Property]: Recv GM: " << m_acName << ", ";
	cout << Msg.m_ToObject << ", ";
	cout << Msg.m_FromObject << ", ";
	cout << Msg.m_Name;
	cout << endl;
	
}



bool Property::operator<(const Property& kOther) const
{
	if(m_iSortPlace < kOther.m_iSortPlace)
		return true;		
	
// 	if(m_iSortPlace == kOther.m_iSortPlace && (m_bSortDistance && kOther.m_bSortDistance) )
	if(m_iSortPlace == kOther.m_iSortPlace)
	{			
		float d1 = m_pkZeroFps->GetCam()->GetPos().DistanceTo(m_pkEntity->GetWorldPosV());
		float d2 = m_pkZeroFps->GetCam()->GetPos().DistanceTo(kOther.m_pkEntity->GetWorldPosV());
		
		if(m_bReverseSort || kOther.m_bReverseSort)
			return d1>d2;		
		else
			return d1<d2;		
 	} 
			
	return false;
}



// Property Edit Interface
vector<string> Property::GetValueNames()
{
	vector<PropertyValues>& kTemp = GetPropertyValues();
	
	vector<string> kReturnVector;
	if(!kTemp.empty())
	{
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			kReturnVector.push_back(kItor->m_strValueName);
			kItor++;
		}
	}
	return kReturnVector;
}


string Property::GetValue(const string& kValueName)
{
	vector<PropertyValues>& kTemp= GetPropertyValues();
	if(!kTemp.empty())
	{
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->m_strValueName)
			{
				return ValueToString(kItor->m_pkValue, &(*kItor));
			}	
			kItor++;
		};
	};
	
	string kStrTemp="invalid ValueName";
	return kStrTemp;
}

string Property::GetValue(const string& kValueName, unsigned int iIndex)
{
	vector<PropertyValues>& kTemp= GetPropertyValues();
	string kStrTemp = "invalid value";
	if(!kTemp.empty())
	{
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->m_strValueName)
			{
				if(kItor->m_iNumberOfValues ==1)
					return ValueToString(kItor->m_pkValue, &(*kItor));
				else
					switch(kItor->m_eValueType)
				{	
					case VALUETYPE_INT:	
						{	
							vector<int> kTempVector = *((vector<int>*)kItor->m_pkValue);
							if(iIndex < kTempVector.size())
								return ValueToString(((void*)&kTempVector[iIndex]), &(*kItor));
							else 
								return kStrTemp;
						}
					case VALUETYPE_STRING:
						{
							vector<string> kTempVector = *((vector<string>*)kItor->m_pkValue);
							if(iIndex < kTempVector.size())
								return ValueToString(((void*)&kTempVector[iIndex]), &(*kItor));
							else 
								return kStrTemp;
						}
					case VALUETYPE_BOOL:
						{
							vector<bool> kTempVector = *((vector<bool>*)kItor->m_pkValue);
							if(iIndex < kTempVector.size())
								return ValueToString(((void*)&kTempVector[iIndex]), &(*kItor));
							else 
								return kStrTemp;
						}
					case VALUETYPE_FLOAT:
						{
							vector<float> kTempVector = *((vector<float>*)kItor->m_pkValue);
							if(iIndex < kTempVector.size())
								return ValueToString(((void*)&kTempVector[iIndex]), &(*kItor));
							else 
								return kStrTemp;
						}
					case VALUETYPE_VECTOR3:	
						{
							vector<Vector3> kTempVector = *((vector<Vector3>*)kItor->m_pkValue);
							if(iIndex < kTempVector.size())
								return ValueToString(((void*)&kTempVector[iIndex]), &(*kItor));
							else 
								return kStrTemp;
						}
					case VALUETYPE_VECTOR4:	
						{
							vector<Vector4> kTempVector = *((vector<Vector4>*)kItor->m_pkValue);
							if(iIndex < kTempVector.size())
								return ValueToString(((void*)&kTempVector[iIndex]), &(*kItor));
							else 
								return kStrTemp;
						}
				};
			}	
			kItor++;
		};
	};
	return kStrTemp;
}

bool Property::SetValue(const string& kValueName, const string& kValue)
{
	//does the property want to handle this value by itself?
	if(HandleSetValue(kValueName, kValue))
	{
		HaveSetValue(kValueName);
		return true;
	}
	
		
	//get possible values
	vector<PropertyValues>& kTemp= GetPropertyValues();
	
	//do we have any values
	if(!kTemp.empty())
	{
		for(vector<PropertyValues>::iterator kItor = kTemp.begin();kItor != kTemp.end();kItor++)
		{
			if( kValueName == (*kItor).m_strValueName)
			{
				if(kItor->m_pkValue == NULL)
				{
					cerr<<"Warning: tried to set value "<<kValueName<<" on property "<<GetName()<<" but value is a NULL value"<<endl;
					return false;
				}

				
				if(StringToValue(kValue, kItor->m_pkValue, &(*kItor)))
				{
					HaveSetValue(kValueName);
					return true;
				}
				
				return false;
			}	
		};
	};
	
	
	return false;
}


bool Property::SetValue(const string& kValueName, unsigned int iIndex, const string& kValue)
{
	if(HandleSetValue(kValueName, kValue))
		return true;
	
	
	
	vector<PropertyValues>& kTemp= GetPropertyValues();
	
	if(!kTemp.empty())
	{
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->m_strValueName)
			{
				if(kItor->m_pkValue == NULL)
				{
					cerr<<"Warning: tried to set value "<<kValueName<<" on property "<<GetName()<<" but value is a NULL value"<<endl;
					return false;
				}			
			
			
				if( kItor->m_iNumberOfValues = 1)
				{
					return StringToValue(kValue, kItor->m_pkValue, &(*kItor));
				}
				else  
				{
					switch(kItor->m_eValueType)
					{	
					case VALUETYPE_INT:	
						{	
							vector<int>* kTempVector = ((vector<int>*)kItor->m_pkValue);;
							if(iIndex < kTempVector->size())
								return StringToValue(kValue,((void*)&(kTempVector[iIndex])), &(*kItor));
							else if(kItor->m_bResize)
							{
								kTempVector->resize(iIndex);
								kItor->m_iNumberOfValues = iIndex+1;
								return StringToValue(kValue,((void*)&(kTempVector[iIndex])), &(*kItor));
							}
							else 
								return false;
						}
					case VALUETYPE_STRING:	
						{	
							vector<string>* kTempVector = ((vector<string>*)kItor->m_pkValue);;
							if(iIndex < kTempVector->size())
								return StringToValue(kValue,((void*)&(kTempVector[iIndex])), &(*kItor));
							else if(kItor->m_bResize)
							{
								kTempVector->resize(iIndex);
								kItor->m_iNumberOfValues = iIndex+1;
								return StringToValue(kValue,((void*)&(kTempVector[iIndex])), &(*kItor));
							}
							else 
								return false;
						}
						
					case VALUETYPE_BOOL:	
						{	
							vector<bool>* kTempVector = ((vector<bool>*)kItor->m_pkValue);;
							if(iIndex < kTempVector->size())
								return StringToValue(kValue,((void*)&(kTempVector[iIndex])), &(*kItor));
							else if(kItor->m_bResize)
							{
								kTempVector->resize(iIndex);
								kItor->m_iNumberOfValues = iIndex+1;
								return StringToValue(kValue,((void*)&(kTempVector[iIndex])), &(*kItor));
							}
							else 
								return false;
						}
						
					case VALUETYPE_FLOAT:	
						{	
							vector<float>* kTempVector = ((vector<float>*)kItor->m_pkValue);;
							if(iIndex < kTempVector->size())
								return StringToValue(kValue,((void*)&(kTempVector[iIndex])), &(*kItor));
							else if(kItor->m_bResize)
							{
								kTempVector->resize(iIndex);
								kItor->m_iNumberOfValues = iIndex+1;
								return StringToValue(kValue,((void*)&(kTempVector[iIndex])), &(*kItor));
							}
							else 
								return false;
						}
						
					case VALUETYPE_VECTOR3:	
						{	
							vector<Vector3>* kTempVector = ((vector<Vector3>*)kItor->m_pkValue);;
							if(iIndex < kTempVector->size())
								return StringToValue(kValue,((void*)&(kTempVector[iIndex])), &(*kItor));
							else if(kItor->m_bResize)
							{
								kTempVector->resize(iIndex);
								kItor->m_iNumberOfValues = iIndex+1;
								return StringToValue(kValue,((void*)&(kTempVector[iIndex])), &(*kItor));
							}
							else 
								return false;
						}
						
					case VALUETYPE_VECTOR4:	
						{	
							vector<Vector4>* kTempVector = ((vector<Vector4>*)kItor->m_pkValue);;
							if(iIndex < kTempVector->size())
								return StringToValue(kValue,((void*)&(kTempVector[iIndex])), &(*kItor));
							else if(kItor->m_bResize)
							{
								kTempVector->resize(iIndex);
								kItor->m_iNumberOfValues = iIndex+1;
								return StringToValue(kValue,((void*)&(kTempVector[iIndex])), &(*kItor));
							}
							else 
								return false;
						}
					}
				};
			};	
			kItor++;		
		};
	};
	return false;
};



int Property::GetNumberOfValues(const string& kValueName)
{
	vector<PropertyValues>& kTemp= GetPropertyValues();
	if(!kTemp.empty())
	{
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->m_strValueName)
			{
				if(kItor->m_bVector)
				{
					switch(kItor->m_eValueType)
					{	
					case VALUETYPE_INT:	
						{	
							vector<int>* kTempVector = ((vector<int>*)kItor->m_pkValue);
							return kTempVector->size();
						}
						
					case VALUETYPE_FLOAT:
						{	
							vector<float>* kTempVector = ((vector<float>*)kItor->m_pkValue);
							return kTempVector->size();
						}
					case VALUETYPE_BOOL:
						{	
							vector<bool>* kTempVector = ((vector<bool>*)kItor->m_pkValue);
							return kTempVector->size();
						}
					case VALUETYPE_STRING:
						{	
							vector<string>* kTempVector = ((vector<string>*)kItor->m_pkValue);
							return kTempVector->size();
						}
					case VALUETYPE_VECTOR3:
						{	
							vector<Vector3>* kTempVector = ((vector<Vector3>*)kItor->m_pkValue);
							return kTempVector->size();
						}
					case VALUETYPE_VECTOR4:
						{	
							vector<Vector4>* kTempVector = ((vector<Vector4>*)kItor->m_pkValue);
							return kTempVector->size();
						}
					}
				}
			}	
			kItor++;
		};
	}
	return -32;
}

string Property::ValueToString(void *pkValue, PropertyValues *pkPropertyValue)
{
	if(pkValue == NULL)
	{
		return "WriteOnly";
	}


	char pk_chBuffer[50];
	
	string kBuffer1,kBuffer2;
	string kBuffer;			
	
	switch(pkPropertyValue->m_eValueType)
	{	
	case VALUETYPE_INT:
		//itoa(*((int*)pkValue),pk_chsBuffer,10); 
		IntToChar(pk_chBuffer,*((int*)pkValue));
		return (kBuffer=pk_chBuffer);
	
	case VALUETYPE_CHARVAL:
	{
		int val = int(*(unsigned char*)pkValue);
		IntToChar(pk_chBuffer,val);
		
		return (kBuffer=pk_chBuffer);
	}
	case VALUETYPE_STRING:
		return(*(reinterpret_cast<string*>(pkValue)));
		
	case VALUETYPE_BOOL:
		if(*((bool*)pkValue)) 
			return (kBuffer="true");
		else return(kBuffer="false");
		
	case VALUETYPE_FLOAT:
		sprintf( pk_chBuffer,"%g", *((float*)pkValue)  );
		
		return kBuffer=pk_chBuffer;
		
	case VALUETYPE_VECTOR3:
		
		sprintf( pk_chBuffer,"%g", ((Vector3*) pkValue)->operator[](0)  );
		kBuffer=pk_chBuffer;
		
		sprintf( pk_chBuffer,"%g", ((Vector3*) pkValue)->operator[](1)  );
		kBuffer+=" ";
		kBuffer+=pk_chBuffer;
		
		sprintf( pk_chBuffer,"%g", ((Vector3*) pkValue)->operator[](2)  );
		kBuffer+=" ";
		kBuffer+=pk_chBuffer;
		return kBuffer; 
		
	case VALUETYPE_VECTOR4:
		
		sprintf( pk_chBuffer,"%g", ((Vector4*) pkValue)->operator[](0)  );
		kBuffer=pk_chBuffer;
		
		sprintf( pk_chBuffer,"%g", ((Vector4*) pkValue)->operator[](1)  );
		kBuffer+=" ";
		kBuffer+=pk_chBuffer;
		
		sprintf( pk_chBuffer,"%g", ((Vector4*) pkValue)->operator[](2)  );
		kBuffer+=" ";
		kBuffer+=pk_chBuffer;
		
		sprintf( pk_chBuffer,"%g", ((Vector4*) pkValue)->operator[](3)  );
		kBuffer+=" ";
		kBuffer+=pk_chBuffer;
		return kBuffer;
	
	case VALUETYPE_CHARS:
		{
			//char* pcTemp;
			//strcpy(kBuffer.c_str(), reinterpret_cast<char*>(pkValue));
			return string(reinterpret_cast<char*>(pkValue));
		}
	};
	string kStrTemp;
	return kStrTemp;				
}


bool Property::StringToValue(const string& kValue, void *pkValue, PropertyValues *pkPropertyValue)
{
	int iTemp1,iTemp2;
	float fTemp1, fTemp2,fTemp3, fTemp4;
	string kTemp1, kTemp2, kTemp3;
	char *cStop;
	
	switch(pkPropertyValue->m_eValueType)
	{
		case VALUETYPE_INT:
			iTemp1=atoi(kValue.c_str());
			if(pkPropertyValue->m_fUpperBound != FLT_MAX)
				if(iTemp1 > pkPropertyValue->m_fUpperBound )
					return false;					
			if(pkPropertyValue->m_fLowerBound !=FLT_MIN)
				if(iTemp1 < pkPropertyValue->m_fLowerBound )
					return false;
			
			*((int*)pkValue)=iTemp1;
			return true;
					
		case VALUETYPE_CHARVAL:
			iTemp1 = atoi(kValue.c_str());
			if((pkPropertyValue->m_fUpperBound)!=FLT_MAX)
				if(iTemp1>(pkPropertyValue->m_fUpperBound))
					return false;
			if((pkPropertyValue->m_fLowerBound)!=FLT_MIN)
				if(iTemp1<(pkPropertyValue->m_fLowerBound))
					return false;
			*((char*)pkValue)=(char)iTemp1;
			return true;		
					
		case VALUETYPE_STRING:
			if((pkPropertyValue->m_fUpperBound)!=FLT_MAX)
				if(kValue.size()>(pkPropertyValue->m_fUpperBound))
					return false;
			if((pkPropertyValue->m_fLowerBound)!=FLT_MIN)
				if(kValue.size()<(pkPropertyValue->m_fLowerBound))
					return false;
			*(reinterpret_cast<string*>(pkValue))=kValue;
			return true;
					
		case VALUETYPE_BOOL:
			if(kValue=="true")
				*((bool*)pkValue) = true; 
			else if(kValue=="false")
				*((bool*)pkValue) = false; 
			else return false;
			return true;
			
		case VALUETYPE_FLOAT:
			fTemp1= (float) strtod( kValue.c_str(), &cStop );
			if((pkPropertyValue->m_fUpperBound)!=FLT_MAX)
				if(fTemp1>(pkPropertyValue->m_fUpperBound))
					return false;
			if((pkPropertyValue->m_fLowerBound)!=FLT_MIN)
				if(fTemp1<(pkPropertyValue->m_fLowerBound))
					return false;
			*((float*)pkValue)=fTemp1; 
			return true;
					
		case VALUETYPE_VECTOR3:
			if((iTemp1=kValue.find(" ")) == -1)//kollar efter ett mellanslag. obs! vet ej om -1 alltid är -1 vid fel...
				return false;		
			kTemp1=kValue.substr(0,iTemp1); //tar ut först talet ur stringen.
			fTemp1= (float) strtod( kTemp1.c_str(), &cStop ); //omvandlar första talet till float
			if((pkPropertyValue->m_fUpperBound)!=FLT_MAX)      //kollar om det finns någon upper bound
				if(fTemp1>(pkPropertyValue->m_fUpperBound))	   // kollar om talet är högre än upperbound
					return false;
			if((pkPropertyValue->m_fLowerBound)!=FLT_MIN)	//kollar lowerbound
				if(fTemp1<(pkPropertyValue->m_fLowerBound))
					return false;
					
			if((iTemp2=kValue.find(" ",iTemp1+1)) == -1) //kollar efter andra mellanslaget
				return false;
			kTemp1=kValue.substr(iTemp1,kValue.length()); //tar ut andra talet ur stringen.
			fTemp2= (float) strtod( kTemp1.c_str(), &cStop ); 
			if((pkPropertyValue->m_fUpperBound)!=FLT_MAX)      
				if(fTemp2>(pkPropertyValue->m_fUpperBound))	   
					return false;
			if((pkPropertyValue->m_fLowerBound)!=FLT_MIN)	
				if(fTemp2<(pkPropertyValue->m_fLowerBound))
					return false;
							
			kTemp1=kValue.substr(iTemp2,kValue.length());
			fTemp3= (float) strtod( kTemp1.c_str(), &cStop ); 
			if((pkPropertyValue->m_fUpperBound)!=FLT_MAX)      
				if(fTemp3>(pkPropertyValue->m_fUpperBound))	   
					return false;
			if((pkPropertyValue->m_fLowerBound)!=FLT_MIN)	
				if(fTemp3<(pkPropertyValue->m_fLowerBound))
					return false;
					
			((Vector3*)pkValue)->Set(fTemp1,fTemp2,fTemp3); 
			return true;
									
		case VALUETYPE_VECTOR4:
			if((iTemp1=kValue.find(" ")) == -1)//kollar efter ett mellanslag. obs! vet ej om -1 alltid är -1 vid fel...
				return false;		
			kTemp1=kValue.substr(0,iTemp1); //tar ut först talet ur stringen.
			fTemp1= (float) strtod( kTemp1.c_str(), &cStop ); //omvandlar första talet till float
			if((pkPropertyValue->m_fUpperBound)!=FLT_MAX)      //kollar om det finns någon upper bound
				if(fTemp1>(pkPropertyValue->m_fUpperBound))	   // kollar om talet är högre än upperbound
					return false;
			
			if((pkPropertyValue->m_fLowerBound)!=FLT_MIN)	//kollar lowerbound
				if(fTemp1<(pkPropertyValue->m_fLowerBound))
					return false;
					
			if((iTemp2=kValue.find(" ",iTemp1+1)) == -1) //kollar efter andra mellanslaget
				return false;
			
			kTemp1=kValue.substr(iTemp1,kValue.length()); //tar ut andra talet ur stringen.
			fTemp2= (float) strtod( kTemp1.c_str(), &cStop ); 
			
			if((pkPropertyValue->m_fUpperBound)!=FLT_MAX)      
				if(fTemp2>(pkPropertyValue->m_fUpperBound))	   
					return false;
			
			if((pkPropertyValue->m_fLowerBound)!=FLT_MIN)	
				if(fTemp2<(pkPropertyValue->m_fLowerBound))
					return false;
							
			if((iTemp1=kValue.find(" ",iTemp2+1)) == -1) //kollar efter andra mellanslaget
				return false;
							
			kTemp1=kValue.substr(iTemp2,kValue.length()); //tar ut andra talet ur stringen.
			fTemp3= (float) strtod( kTemp1.c_str(), &cStop ); 
			
			if((pkPropertyValue->m_fUpperBound)!=FLT_MAX)      
				if(fTemp2>(pkPropertyValue->m_fUpperBound))	   
					return false;
			if((pkPropertyValue->m_fLowerBound)!=FLT_MIN)	
				if(fTemp2<(pkPropertyValue->m_fLowerBound))
					return false;
									
			kTemp1=kValue.substr(iTemp1,kValue.length());
			fTemp4= (float) strtod( kTemp1.c_str(), &cStop ); 
									
			if((pkPropertyValue->m_fUpperBound)!=FLT_MAX)      
				if(fTemp3>(pkPropertyValue->m_fUpperBound))	   
					return false;
			if((pkPropertyValue->m_fLowerBound)!=FLT_MIN)	
				if(fTemp3<(pkPropertyValue->m_fLowerBound))
					return false;
											
			((Vector4*)pkValue)->Set(fTemp1,fTemp2,fTemp3,fTemp4); 
			return true;
			
		case VALUETYPE_CHARS:
			if((pkPropertyValue->m_fUpperBound)!=FLT_MAX)
				if(kValue.size()>(pkPropertyValue->m_fUpperBound))
					return false;
			if((pkPropertyValue->m_fLowerBound)!=FLT_MIN)
				if(kValue.size()<(pkPropertyValue->m_fLowerBound))
					return false;
					
			strcpy(reinterpret_cast<char*>(pkValue),kValue.c_str());
			return true;															
											
	};
	
	return false;	
}



float Property::GetUpperBound(const string& kValueName)
{
	vector<PropertyValues>& kTemp= GetPropertyValues();
	if(!kTemp.empty())
	{
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->m_strValueName)
			{
				
				return kItor->m_fUpperBound;
			}	
			kItor++;
		};
	}
	return 0;
}

float Property::GetLowerBound(const string& kValueName)
{
	vector<PropertyValues>& kTemp= GetPropertyValues();
	if(!kTemp.empty())
	{
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->m_strValueName)
			{
				
				return kItor->m_fLowerBound;
			}	
			kItor++;
		};
	}
	return 0;
}

bool Property::CheckIfResize(const string& kValueName)
{
	vector<PropertyValues>& kTemp= GetPropertyValues();
	if(!kTemp.empty())
	{
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->m_strValueName)
			{
				return kItor->m_bResize;
			}	
			kItor++;
		};
	}
	return false;
}

bool Property::CheckIfVector(const string& kValueName)
{
	vector<PropertyValues>& kTemp= GetPropertyValues();
	if(!kTemp.empty())
	{
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->m_strValueName)
			{
				return kItor->m_bVector;
			}	
			kItor++;
		};
	}
	return false;
}

bool Property::Resize(const string& kValueName, unsigned int uiNewSize)
{
	vector<PropertyValues>& kTemp= GetPropertyValues();
	if(!kTemp.empty())
	{
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->m_strValueName)
			{
				if(kItor->m_bResize)
				{
					switch(kItor->m_eValueType)
					{	
					case VALUETYPE_INT:	
						{	
							vector<int>* kTempVector = ((vector<int>*)kItor->m_pkValue);
							kTempVector->resize(uiNewSize);
							kItor->m_iNumberOfValues=uiNewSize;
							return true;
						}
						
					case VALUETYPE_FLOAT:
						{	
							vector<float>* kTempVector = ((vector<float>*)kItor->m_pkValue);
							kTempVector->resize(uiNewSize);
							kItor->m_iNumberOfValues=uiNewSize;
							return true;
						}
					case VALUETYPE_BOOL:
						{	
							vector<bool>* kTempVector = ((vector<bool>*)kItor->m_pkValue);
							kTempVector->resize(uiNewSize);
							kItor->m_iNumberOfValues=uiNewSize;
							return true;
						}
					case VALUETYPE_STRING:
						{	
							vector<string>* kTempVector = ((vector<string>*)kItor->m_pkValue);
							kTempVector->resize(uiNewSize);
							kItor->m_iNumberOfValues=uiNewSize;
							return true;
						}
					case VALUETYPE_VECTOR3:
						{	
							vector<Vector3>* kTempVector = ((vector<Vector3>*)kItor->m_pkValue);
							kTempVector->resize(uiNewSize);
							kItor->m_iNumberOfValues=uiNewSize;
							return true;
						}
					case VALUETYPE_VECTOR4:
						{	
							vector<Vector4>* kTempVector = ((vector<Vector4>*)kItor->m_pkValue);
							kTempVector->resize(uiNewSize);
							kItor->m_iNumberOfValues=uiNewSize;
							return true;
						}
					}
				}
				else return false;
			}
			kItor++;
		};
	}
	return false;
}




void	Property::SetNrOfConnections(int iConNR)
{
	m_kNetUpdateFlags.resize(iConNR);
	ResetAllNetUpdateFlags();
}

void	Property::ResetAllNetUpdateFlags()
{
	for(unsigned int i = 0;i<m_kNetUpdateFlags.size();i++)
	{
		m_kNetUpdateFlags[i]=true;
	}
}

void Property::SetNetUpdateFlag(bool bValue)
{
	for(unsigned int i = 0;i<m_kNetUpdateFlags.size();i++)
	{
		m_kNetUpdateFlags[i] = bValue;
	}
}

void	Property::SetNetUpdateFlag(int iConID,bool bValue)
{
	m_kNetUpdateFlags[iConID] = bValue;
}

bool	Property::GetNetUpdateFlag(int iConID)
{
	return m_kNetUpdateFlags[iConID];
}




// graveyards  R I P

// void Property::PropertyFound(Property* pkProperty)
// {
// 	
// 	vector<TPointerBase*>::iterator kIt = this->m_kPointerVector.begin();
// 	while(kIt != this->m_kPointerVector.end())
// 	{
// 		if((*kIt)->Set(pkProperty))
// 		{
// 			this->PointerFound((*kIt)->GetType());
// 			kIt = this->m_kPointerVector.end();
// 		}
// 		else
// 			++kIt;
// 	}
// }
// void Property::PropertyLost(Property* pkProperty)
// {
// 	
// 	vector<TPointerBase*>::iterator kIt = this->m_kPointerVector.begin();
// 	while(kIt != this->m_kPointerVector.end())
// 	{
// 		if((*kIt)->Unset(pkProperty))
// 		{
// 			this->PointerLost((*kIt)->GetType());
// 			kIt = this->m_kPointerVector.end();
// 		}
// 		else
// 			++kIt;
// 	}
// 
// }


/*	vector<TPointerBase*>::iterator kIt = this->m_kPointerVector.begin();
	while(kIt != this->m_kPointerVector.end())
	{
		delete (*kIt);
		++kIt;
	}*/
	
// 	if(m_pkEntity)
// 		m_pkEntity->RemoveProperty(this);

// vector<PropertyValues> Property::GetPropertyValues()
// {
// 	vector<PropertyValues> kReturn;
// 	return kReturn;
// };


