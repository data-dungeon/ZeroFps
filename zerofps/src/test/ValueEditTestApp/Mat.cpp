// Mat.cpp: implementation of the Mat class.
//
//////////////////////////////////////////////////////////////////////

#include "Mat.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Mat::Mat()
{
	
}

Mat::~Mat()
{
	
}

vector<Mat::PropertyValues> Mat::GetPropertyValues()
{
	vector<PropertyValues> kReturn;
	return kReturn;
};


vector<string> Mat::GetValueNames()
{
	vector<PropertyValues> kTemp= GetPropertyValues();
	
	vector<string> kReturnVector;
	if(!kTemp.empty())
	{
		
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			kReturnVector.push_back(kItor->kValueName);
			kItor++;
		}
	}
	return kReturnVector;
}

/*
string Mat::GetValue(string kValueName)
{
vector<PropertyValues> kTemp= GetPropertyValues();
string kBuffer;
if(!kTemp.empty())
{

		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
		if( kValueName == kItor->kValueName)
		{
		
		  char pk_chBuffer[50];
		  int  iDecimal, iSign;
		  string kBuffer1,kBuffer2;
		  
			switch(kItor->iValueType)
			{	
			case VALUETYPE_INT:
			itoa(*((int*)kItor->pkValue),pk_chBuffer,10); 
			return (kBuffer=pk_chBuffer);
			
			  case VALUETYPE_STRING:
			  return(*(reinterpret_cast<string*>(kItor->pkValue)));
			  
				case VALUETYPE_BOOL:
				if(*((bool*)kItor->pkValue)) 
				return (kBuffer="true");
				else return(kBuffer="false");
				
				  case VALUETYPE_FLOAT:
				  sprintf( pk_chBuffer,"%g", *((float*)kItor->pkValue)  );
				  
					return kBuffer=pk_chBuffer;
					
					  case VALUETYPE_VECTOR3:
					  
						sprintf( pk_chBuffer,"%g", ((Vector3*) kItor->pkValue)->operator[](0)  );
						kBuffer=pk_chBuffer;
						
						  sprintf( pk_chBuffer,"%g", ((Vector3*) kItor->pkValue)->operator[](1)  );
						  kBuffer+=" ";
						  kBuffer+=pk_chBuffer;
						  
							sprintf( pk_chBuffer,"%g", ((Vector3*) kItor->pkValue)->operator[](2)  );
							kBuffer+=" ";
							kBuffer+=pk_chBuffer;
							return kBuffer; 
							
							  case VALUETYPE_VECTOR4:
							  
								sprintf( pk_chBuffer,"%g", ((Vector4*) kItor->pkValue)->operator[](0)  );
								kBuffer=pk_chBuffer;
								
								  sprintf( pk_chBuffer,"%g", ((Vector4*) kItor->pkValue)->operator[](1)  );
								  kBuffer+=" ";
								  kBuffer+=pk_chBuffer;
								  
									sprintf( pk_chBuffer,"%g", ((Vector4*) kItor->pkValue)->operator[](2)  );
									kBuffer+=" ";
									kBuffer+=pk_chBuffer;
									
									  sprintf( pk_chBuffer,"%g", ((Vector4*) kItor->pkValue)->operator[](3)  );
									  kBuffer+=" ";
									  kBuffer+=pk_chBuffer;
									  return kBuffer;
									  };
									  
										}	
										kItor++;
										};
										
										  };
										  return kBuffer;
}*/
string Mat::GetValue(string kValueName)
{
	vector<PropertyValues> kTemp= GetPropertyValues();
	
	if(!kTemp.empty())
	{
		
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->kValueName)
			{
				return ValueToString(kItor->pkValue, kItor);
				
				
			}	
			kItor++;
		};
		
	};
	string kStrTemp="invalid ValueName";
	return kStrTemp;
}

string Mat::GetValue(string kValueName, unsigned int iIndex)
{
	vector<PropertyValues> kTemp= GetPropertyValues();
	string kStrTemp = "invalid value";
	if(!kTemp.empty())
	{
		
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->kValueName)
			{
				if(kItor->iNumberOfValues ==-1)
					return ValueToString(kItor->pkValue, kItor);
				else
					
					switch(kItor->iValueType)
					{	
					case VALUETYPE_INT:	
						{	
						vector<int> kTempVecktor = *((vector<int>*)kItor->pkValue);
						if(iIndex < kTempVecktor.size())
							return ValueToString(((void*)&kTempVecktor[iIndex]), kItor);
						else 
							return kStrTemp;
						}
					
					case VALUETYPE_STRING:
						{
						vector<string> kTempVecktor = *((vector<string>*)kItor->pkValue);
						if(iIndex < kTempVecktor.size())
							return ValueToString(((void*)&kTempVecktor[iIndex]), kItor);
						else 
							return kStrTemp;
						}
					
					case VALUETYPE_BOOL:
							{
						vector<bool> kTempVecktor = *((vector<bool>*)kItor->pkValue);
						if(iIndex < kTempVecktor.size())
							return ValueToString(((void*)&kTempVecktor[iIndex]), kItor);
						else 
							return kStrTemp;
						}
					case VALUETYPE_FLOAT:
						{
						vector<float> kTempVecktor = *((vector<float>*)kItor->pkValue);
						if(iIndex < kTempVecktor.size())
							return ValueToString(((void*)&kTempVecktor[iIndex]), kItor);
						else 
							return kStrTemp;
						}
					case VALUETYPE_VECTOR3:	
						{
						vector<Vector3> kTempVecktor = *((vector<Vector3>*)kItor->pkValue);
						if(iIndex < kTempVecktor.size())
							return ValueToString(((void*)&kTempVecktor[iIndex]), kItor);
						else 
							return kStrTemp;
						}
					case VALUETYPE_VECTOR4:	
						{
						vector<Vector4> kTempVecktor = *((vector<Vector4>*)kItor->pkValue);
						if(iIndex < kTempVecktor.size())
							return ValueToString(((void*)&kTempVecktor[iIndex]), kItor);
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
/*bool Mat::SetValue(string kValueName, string kValue)
{
	vector<PropertyValues> kTemp= GetPropertyValues();
	if(!kTemp.empty())
	{
		
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->kValueName)
			{
				int iTemp1,iTemp2;
				float fTemp1, fTemp2,fTemp3,  fTemp4;
				string kTemp1, kTemp2, kTemp3;
				char *cStop;
				
				switch(kItor->iValueType)
				{	
				case VALUETYPE_INT:
					iTemp1=atoi(kValue.c_str());
					if((kItor->fUpperBound)!=FLT_MAX)
						if(iTemp1>(kItor->fUpperBound))
							return false;
						if((kItor->fLowerBound)!=FLT_MIN)
							if(iTemp1<(kItor->fLowerBound))
								return false;
							*((int*)kItor->pkValue)=iTemp1;
							return true;
							
				case VALUETYPE_STRING:
					if((kItor->fUpperBound)!=FLT_MAX)
						if(kValue.size()>(kItor->fUpperBound))
							return false;
						if((kItor->fLowerBound)!=FLT_MIN)
							if(kValue.size()<(kItor->fLowerBound))
								return false;
							*(reinterpret_cast<string*>(kItor->pkValue))=kValue;
							return true;
							
				case VALUETYPE_BOOL:
					if(kValue=="true")
						*((bool*)kItor->pkValue) = true; 
					else if(kValue=="false")
						*((bool*)kItor->pkValue) = false; 
					else return false;
					return true;
					
				case VALUETYPE_FLOAT:
					fTemp1= strtod( kValue.c_str(), &cStop );
					if((kItor->fUpperBound)!=FLT_MAX)
						if(fTemp1>(kItor->fUpperBound))
							return false;
						if((kItor->fLowerBound)!=FLT_MIN)
							if(fTemp1<(kItor->fLowerBound))
								return false;
							*((float*)kItor->pkValue)=fTemp1; 
							return true;
							
				case VALUETYPE_VECTOR3:
					if((iTemp1=kValue.find(" ")) == -1)//kollar efter ett mellanslag. obs! vet ej om -1 alltid är -1 vid fel...
						return false;		
					kTemp1=kValue.substr(0,iTemp1); //tar ut först talet ur stringen.
					fTemp1= strtod( kTemp1.c_str(), &cStop ); //omvandlar första talet till float
					if((kItor->fUpperBound)!=FLT_MAX)      //kollar om det finns någon upper bound
						if(fTemp1>(kItor->fUpperBound))	   // kollar om talet är högre än upperbound
							return false;
						if((kItor->fLowerBound)!=FLT_MIN)	//kollar lowerbound
							if(fTemp1<(kItor->fLowerBound))
								return false;
							
							if((iTemp2=kValue.find(" ",iTemp1+1)) == -1) //kollar efter andra mellanslaget
								return false;
							kTemp1=kValue.substr(iTemp1,kValue.length()); //tar ut andra talet ur stringen.
							fTemp2= strtod( kTemp1.c_str(), &cStop ); 
							if((kItor->fUpperBound)!=FLT_MAX)      
								if(fTemp2>(kItor->fUpperBound))	   
									return false;
								if((kItor->fLowerBound)!=FLT_MIN)	
									if(fTemp2<(kItor->fLowerBound))
										return false;
									
									kTemp1=kValue.substr(iTemp2,kValue.length());
									fTemp3= strtod( kTemp1.c_str(), &cStop ); 
									if((kItor->fUpperBound)!=FLT_MAX)      
										if(fTemp3>(kItor->fUpperBound))	   
											return false;
										if((kItor->fLowerBound)!=FLT_MIN)	
											if(fTemp3<(kItor->fLowerBound))
												return false;
											
											((Vector3*)kItor->pkValue)->Set(fTemp1,fTemp2,fTemp3); 
											return true;
											
				case VALUETYPE_VECTOR4:
					if((iTemp1=kValue.find(" ")) == -1)//kollar efter ett mellanslag. obs! vet ej om -1 alltid är -1 vid fel...
						return false;		
					kTemp1=kValue.substr(0,iTemp1); //tar ut först talet ur stringen.
					fTemp1= strtod( kTemp1.c_str(), &cStop ); //omvandlar första talet till float
					if((kItor->fUpperBound)!=FLT_MAX)      //kollar om det finns någon upper bound
						if(fTemp1>(kItor->fUpperBound))	   // kollar om talet är högre än upperbound
							return false;
						if((kItor->fLowerBound)!=FLT_MIN)	//kollar lowerbound
							if(fTemp1<(kItor->fLowerBound))
								return false;
							
							if((iTemp2=kValue.find(" ",iTemp1+1)) == -1) //kollar efter andra mellanslaget
								return false;
							kTemp1=kValue.substr(iTemp1,kValue.length()); //tar ut andra talet ur stringen.
							fTemp2= strtod( kTemp1.c_str(), &cStop ); 
							if((kItor->fUpperBound)!=FLT_MAX)      
								if(fTemp2>(kItor->fUpperBound))	   
									return false;
								if((kItor->fLowerBound)!=FLT_MIN)	
									if(fTemp2<(kItor->fLowerBound))
										return false;
									
									if((iTemp1=kValue.find(" ",iTemp2+1)) == -1) //kollar efter andra mellanslaget
										return false;
									kTemp1=kValue.substr(iTemp2,kValue.length()); //tar ut andra talet ur stringen.
									fTemp3= strtod( kTemp1.c_str(), &cStop ); 
									if((kItor->fUpperBound)!=FLT_MAX)      
										if(fTemp2>(kItor->fUpperBound))	   
											return false;
										if((kItor->fLowerBound)!=FLT_MIN)	
											if(fTemp2<(kItor->fLowerBound))
												return false;
											
											kTemp1=kValue.substr(iTemp1,kValue.length());
											fTemp4= strtod( kTemp1.c_str(), &cStop ); 
											if((kItor->fUpperBound)!=FLT_MAX)      
												if(fTemp3>(kItor->fUpperBound))	   
													return false;
												if((kItor->fLowerBound)!=FLT_MIN)	
													if(fTemp3<(kItor->fLowerBound))
														return false;
													
													((Vector4*)kItor->pkValue)->set(fTemp1,fTemp2,fTemp3,fTemp4); 
													return true;
													
													
				};
				
			}	
			kItor++;
		};
		
	};
	return false;
}*/
bool Mat::SetValue(string kValueName, string kValue)
{
	vector<PropertyValues> kTemp= GetPropertyValues();
	if(!kTemp.empty())
	{
		
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->kValueName)
			{
												
				return StringToValue(kValue, kItor->pkValue, kItor);
				
			}	
			kItor++;
		};
		
	};
	return false;
}


bool Mat::SetValue(string kValueName, unsigned int iIndex, string kValue)
{
	cout <<"check 1"  <<endl;
	vector<PropertyValues> kTemp= GetPropertyValues();
	if(!kTemp.empty())
	{
		
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			cout <<"check 2"  <<endl;
			if( kValueName == kItor->kValueName)
			{
				
				
				if( kItor->iNumberOfValues < 0)
				{
					cout <<"check 3a"  <<endl;
					return StringToValue(kValue, kItor->pkValue, kItor);
				}
				
				else  
				{
					cout <<"check 3b"  <<endl;
					switch(kItor->iValueType)
					{	
					case VALUETYPE_INT:	
						{	
							vector<int>* kTempVecktor = ((vector<int>*)kItor->pkValue);;
							if(iIndex < kTempVecktor->size())
								return StringToValue(kValue,((void*)&(kTempVecktor->at(iIndex))), kItor);
							else if(kItor->bResize)
							{
								kTempVecktor->resize(iIndex);
								kItor->iNumberOfValues = iIndex+1;
								return StringToValue(kValue,((void*)&(kTempVecktor->at(iIndex))), kItor);
							}
							else 
								return false;
						}
					case VALUETYPE_STRING:	
						{	
							vector<string>* kTempVecktor = ((vector<string>*)kItor->pkValue);;
							if(iIndex < kTempVecktor->size())
								return StringToValue(kValue,((void*)&(kTempVecktor->at(iIndex))), kItor);
							else if(kItor->bResize)
							{
								kTempVecktor->resize(iIndex);
								kItor->iNumberOfValues = iIndex+1;
								return StringToValue(kValue,((void*)&(kTempVecktor->at(iIndex))), kItor);
							}
							else 
								return false;
						}
					
					case VALUETYPE_BOOL:	
						{	
							vector<bool>* kTempVecktor = ((vector<bool>*)kItor->pkValue);;
							if(iIndex < kTempVecktor->size())
								return StringToValue(kValue,((void*)&(kTempVecktor->at(iIndex))), kItor);
							else if(kItor->bResize)
							{
								kTempVecktor->resize(iIndex);
								kItor->iNumberOfValues = iIndex+1;
								return StringToValue(kValue,((void*)&(kTempVecktor->at(iIndex))), kItor);
							}
							else 
								return false;
						}
					
					case VALUETYPE_FLOAT:	
						{	
							vector<float>* kTempVecktor = ((vector<float>*)kItor->pkValue);;
							if(iIndex < kTempVecktor->size())
								return StringToValue(kValue,((void*)&(kTempVecktor->at(iIndex))), kItor);
							else if(kItor->bResize)
							{
								kTempVecktor->resize(iIndex);
								kItor->iNumberOfValues = iIndex+1;
								return StringToValue(kValue,((void*)&(kTempVecktor->at(iIndex))), kItor);
							}
							else 
								return false;
						}
					
					case VALUETYPE_VECTOR3:	
						{	
							vector<Vector3>* kTempVecktor = ((vector<Vector3>*)kItor->pkValue);;
							if(iIndex < kTempVecktor->size())
								return StringToValue(kValue,((void*)&(kTempVecktor->at(iIndex))), kItor);
							else if(kItor->bResize)
							{
								kTempVecktor->resize(iIndex);
								kItor->iNumberOfValues = iIndex+1;
								return StringToValue(kValue,((void*)&(kTempVecktor->at(iIndex))), kItor);
							}
							else 
								return false;
						}
					
					case VALUETYPE_VECTOR4:	
						{	
							vector<Vector4>* kTempVecktor = ((vector<Vector4>*)kItor->pkValue);;
							if(iIndex < kTempVecktor->size())
								return StringToValue(kValue,((void*)&(kTempVecktor->at(iIndex))), kItor);
							else if(kItor->bResize)
							{
								kTempVecktor->resize(iIndex);
								kItor->iNumberOfValues = iIndex+1;
								return StringToValue(kValue,((void*)&(kTempVecktor->at(iIndex))), kItor);
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


Mat::PropertyValues::PropertyValues()
{
	fLowerBound=FLT_MIN;
	fUpperBound=FLT_MAX;
	iNumberOfValues = -1;
	bResize = false;
}

int Mat::GetNumberOfValues(string kValueName)
{
	vector<PropertyValues> kTemp= GetPropertyValues();
	if(!kTemp.empty())
	{
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->kValueName)
			{
				return kItor->iNumberOfValues;
			}	
			kItor++;
		};
	}
	return -32;
}

string Mat::ValueToString(void *pkValue, PropertyValues *pkPropertyValue)
{
	char pk_chBuffer[50];
	
	string kBuffer1,kBuffer2;
	string kBuffer;			
	
	switch(pkPropertyValue->iValueType)
	{	
	case VALUETYPE_INT:
		itoa(*((int*)pkValue),pk_chBuffer,10); 
		return (kBuffer=pk_chBuffer);
		
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
	};
string kStrTemp;
	return kStrTemp;				
}


bool Mat::StringToValue(string kValue, void *pkValue, PropertyValues *pkPropertyValue)
{
	int iTemp1,iTemp2;
	float fTemp1, fTemp2,fTemp3, fTemp4;
	string kTemp1, kTemp2, kTemp3;
	char *cStop;
				
	switch(pkPropertyValue->iValueType)
	{	
	case VALUETYPE_INT:
		iTemp1=atoi(kValue.c_str());
		if((pkPropertyValue->fUpperBound)!=FLT_MAX)
			if(iTemp1>(pkPropertyValue->fUpperBound))
				return false;
			if((pkPropertyValue->fLowerBound)!=FLT_MIN)
				if(iTemp1<(pkPropertyValue->fLowerBound))
					return false;
				*((int*)pkValue)=iTemp1;
				return true;
				
	case VALUETYPE_STRING:
		if((pkPropertyValue->fUpperBound)!=FLT_MAX)
			if(kValue.size()>(pkPropertyValue->fUpperBound))
				return false;
			if((pkPropertyValue->fLowerBound)!=FLT_MIN)
				if(kValue.size()<(pkPropertyValue->fLowerBound))
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
		fTemp1= strtod( kValue.c_str(), &cStop );
		if((pkPropertyValue->fUpperBound)!=FLT_MAX)
			if(fTemp1>(pkPropertyValue->fUpperBound))
				return false;
			if((pkPropertyValue->fLowerBound)!=FLT_MIN)
				if(fTemp1<(pkPropertyValue->fLowerBound))
					return false;
				*((float*)pkValue)=fTemp1; 
				return true;
				
	case VALUETYPE_VECTOR3:
		if((iTemp1=kValue.find(" ")) == -1)//kollar efter ett mellanslag. obs! vet ej om -1 alltid är -1 vid fel...
			return false;		
		kTemp1=kValue.substr(0,iTemp1); //tar ut först talet ur stringen.
		fTemp1= strtod( kTemp1.c_str(), &cStop ); //omvandlar första talet till float
		if((pkPropertyValue->fUpperBound)!=FLT_MAX)      //kollar om det finns någon upper bound
			if(fTemp1>(pkPropertyValue->fUpperBound))	   // kollar om talet är högre än upperbound
				return false;
			if((pkPropertyValue->fLowerBound)!=FLT_MIN)	//kollar lowerbound
				if(fTemp1<(pkPropertyValue->fLowerBound))
					return false;
				
				if((iTemp2=kValue.find(" ",iTemp1+1)) == -1) //kollar efter andra mellanslaget
					return false;
				kTemp1=kValue.substr(iTemp1,kValue.length()); //tar ut andra talet ur stringen.
				fTemp2= strtod( kTemp1.c_str(), &cStop ); 
				if((pkPropertyValue->fUpperBound)!=FLT_MAX)      
					if(fTemp2>(pkPropertyValue->fUpperBound))	   
						return false;
					if((pkPropertyValue->fLowerBound)!=FLT_MIN)	
						if(fTemp2<(pkPropertyValue->fLowerBound))
							return false;
						
						kTemp1=kValue.substr(iTemp2,kValue.length());
						fTemp3= strtod( kTemp1.c_str(), &cStop ); 
						if((pkPropertyValue->fUpperBound)!=FLT_MAX)      
							if(fTemp3>(pkPropertyValue->fUpperBound))	   
								return false;
							if((pkPropertyValue->fLowerBound)!=FLT_MIN)	
								if(fTemp3<(pkPropertyValue->fLowerBound))
									return false;
								
								((Vector3*)pkValue)->Set(fTemp1,fTemp2,fTemp3); 
								return true;
								
	case VALUETYPE_VECTOR4:
		if((iTemp1=kValue.find(" ")) == -1)//kollar efter ett mellanslag. obs! vet ej om -1 alltid är -1 vid fel...
			return false;		
		kTemp1=kValue.substr(0,iTemp1); //tar ut först talet ur stringen.
		fTemp1= strtod( kTemp1.c_str(), &cStop ); //omvandlar första talet till float
		if((pkPropertyValue->fUpperBound)!=FLT_MAX)      //kollar om det finns någon upper bound
			if(fTemp1>(pkPropertyValue->fUpperBound))	   // kollar om talet är högre än upperbound
				return false;
			if((pkPropertyValue->fLowerBound)!=FLT_MIN)	//kollar lowerbound
				if(fTemp1<(pkPropertyValue->fLowerBound))
					return false;
				
				if((iTemp2=kValue.find(" ",iTemp1+1)) == -1) //kollar efter andra mellanslaget
					return false;
				kTemp1=kValue.substr(iTemp1,kValue.length()); //tar ut andra talet ur stringen.
				fTemp2= strtod( kTemp1.c_str(), &cStop ); 
				if((pkPropertyValue->fUpperBound)!=FLT_MAX)      
					if(fTemp2>(pkPropertyValue->fUpperBound))	   
						return false;
					if((pkPropertyValue->fLowerBound)!=FLT_MIN)	
						if(fTemp2<(pkPropertyValue->fLowerBound))
							return false;
						
						if((iTemp1=kValue.find(" ",iTemp2+1)) == -1) //kollar efter andra mellanslaget
							return false;
						kTemp1=kValue.substr(iTemp2,kValue.length()); //tar ut andra talet ur stringen.
						fTemp3= strtod( kTemp1.c_str(), &cStop ); 
						if((pkPropertyValue->fUpperBound)!=FLT_MAX)      
							if(fTemp2>(pkPropertyValue->fUpperBound))	   
								return false;
							if((pkPropertyValue->fLowerBound)!=FLT_MIN)	
								if(fTemp2<(pkPropertyValue->fLowerBound))
									return false;
								
								kTemp1=kValue.substr(iTemp1,kValue.length());
								fTemp4= strtod( kTemp1.c_str(), &cStop ); 
								if((pkPropertyValue->fUpperBound)!=FLT_MAX)      
									if(fTemp3>(pkPropertyValue->fUpperBound))	   
										return false;
									if((pkPropertyValue->fLowerBound)!=FLT_MIN)	
										if(fTemp3<(pkPropertyValue->fLowerBound))
											return false;
										
										((Vector4*)pkValue)->set(fTemp1,fTemp2,fTemp3,fTemp4); 
										return true;
										
										
				};
return false;	
}



float Mat::GetUpperBound(string kValueName)
{

	PropertyValues* pkTemp;
			
	if( FindPropertyValue(kValueName,pkTemp))
			{
				
				return pkTemp->fUpperBound;
			}	
			
	else
		return 0;
}

float Mat::GetLowerBound(string kValueName)
{
vector<PropertyValues> kTemp= GetPropertyValues();
	if(!kTemp.empty())
	{
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->kValueName)
			{
				
				return kItor->fLowerBound;
			}	
			kItor++;
		};
	}
	return 0;
}

bool Mat::CheckIfResize(string kValueName)
{
vector<PropertyValues> kTemp= GetPropertyValues();
	if(!kTemp.empty())
	{
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->kValueName)
			{
				return kItor->bResize;
			}	
			kItor++;
		};
	}
	return false;
}

/*PropertyValues Mat::FindPropertyValue(string kValueName, PropertyValues* pkPropertyValues)
{
	vector<PropertyValues> kTemp= GetPropertyValues();
	if(!kTemp.empty())
	{
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->kValueName)
			{
				cout<<kItor->kValueName <<endl;
				cout<<kItor->fUpperBound <<endl;
				pkPropertyValues = kItor;
				return true;
			}	
			kItor++;
		};
	}

	return false;
}*/
	