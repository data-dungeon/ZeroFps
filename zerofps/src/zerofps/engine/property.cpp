#include "property.h"

#include "zerofps.h"

Property::Property()
{
	m_pkZeroFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	m_pkPropertyFactory = static_cast<PropertyFactory*>(g_ZFObjSys.GetObjectPtr("PropertyFactory"));			
	
	bNetwork = false;
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	m_pkObject=NULL;
	strcpy(m_acName,"NoName");		
	
	m_iSortPlace=0;
	m_bSortDistance=false;
}

/*
void Property::PackTo(NetPacket* pkNetPacket) 
{

}

void Property::PackFrom(NetPacket* pkNetPacket) 
{  

}
*/


void Property::Save(ZFMemPackage* pkPackage)
{
	int i=1234;
	pkPackage->Write((void*)&i,4);
}

void Property::Load(ZFMemPackage* pkPackage)
{  
	int i=0;
	pkPackage->Read((void*)&i,4);
	
	if(i != 1234)
		cout<<"ERROR Loading property"<<endl;
}

bool Property::operator<(Property& kOther){
	float d1,d2;	
	
	if(!m_bSortDistance){
		return m_iSortPlace < kOther.m_iSortPlace;
	}else{	
		if(m_iSortPlace < kOther.m_iSortPlace){
			return true;		
		}else {
			if(m_iSortPlace == kOther.m_iSortPlace){			
				d1 = abs( (m_pkZeroFps->GetCam()->GetPos() - m_pkObject->GetPos()).Length() );
				d2 = abs( (m_pkZeroFps->GetCam()->GetPos() - kOther.m_pkObject->GetPos()).Length() );
				return d1<d2;		
			} else {
				return false;
			}
		}
	}
}

vector<Property::PropertyValues> Property::GetPropertyValues()
{
vector<PropertyValues> kReturn;
	return kReturn;
};

vector<string> Property::GetValueNames()
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

string Property::GetValue(string kValueName)
{
//	if(HandleSetValue(kValueName, kValue))
//		return;

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
					kBuffer= fcvt(*((float*)kItor->pkValue), 5, &iDecimal, &iSign );
					kBuffer.insert(iDecimal, ".");
					if(iSign !=0)
						kBuffer.insert(0, "-");
					return kBuffer; 

				case VALUETYPE_VECTOR3:
					kBuffer1=fcvt(( (Vector3*) kItor->pkValue)->operator[](0), 5, &iDecimal, &iSign );
					kBuffer1.insert(iDecimal, ".");
					if(iSign !=0)
					kBuffer1.insert(0, "-");
					kBuffer1+=" ";
					kBuffer=kBuffer1;	
					
					kBuffer1=fcvt(( (Vector3*) kItor->pkValue)->operator[](1), 5, &iDecimal, &iSign );
					kBuffer1.insert(iDecimal, ".");
					if(iSign !=0)
					kBuffer1.insert(0, "-");
					kBuffer1+=" ";
					kBuffer+=kBuffer1;
					
					kBuffer1=fcvt(( (Vector3*) kItor->pkValue)->operator[](2), 5, &iDecimal, &iSign );
					kBuffer1.insert(iDecimal, ".");
					if(iSign !=0)
					kBuffer1.insert(0, "-");
					kBuffer+=kBuffer1;
					return kBuffer; 
				
				case VALUETYPE_VECTOR4:
					kBuffer1=fcvt(( (Vector4*) kItor->pkValue)->operator[](0), 5, &iDecimal, &iSign );
					kBuffer1.insert(iDecimal, ".");
					if(iSign !=0)
					kBuffer1.insert(0, "-");
					kBuffer1+=" ";
					kBuffer=kBuffer1;	
					
					kBuffer1=fcvt(( (Vector4*) kItor->pkValue)->operator[](1), 5, &iDecimal, &iSign );
					kBuffer1.insert(iDecimal, ".");
					if(iSign !=0)
					kBuffer1.insert(0, "-");
					kBuffer1+=" ";
					kBuffer+=kBuffer1;
					
					kBuffer1=fcvt(( (Vector4*) kItor->pkValue)->operator[](2), 5, &iDecimal, &iSign );
					kBuffer1.insert(iDecimal, ".");
					if(iSign !=0)
					kBuffer1.insert(0, "-");
					kBuffer1+=" ";
					kBuffer+=kBuffer1;

					kBuffer1=fcvt(( (Vector4*) kItor->pkValue)->operator[](3), 5, &iDecimal, &iSign );
					kBuffer1.insert(iDecimal, ".");
					if(iSign !=0)
					kBuffer1.insert(0, "-");
					kBuffer+=kBuffer1;
					return kBuffer; 
				}
			
			}	
		kItor++;
		};
	};
	return kBuffer;
}


bool Property::SetValue(string kValueName, string kValue)
{
	if(HandleSetValue(kValueName, kValue))
		return true;

vector<PropertyValues> kTemp= GetPropertyValues();
	if(!kTemp.empty())
	{
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->kValueName)
			{
				int iTemp1,iTemp2;
				float fTemp1, fTemp2,fTemp3, fTemp4;
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
				
					((Vector4*)kItor->pkValue)->Set(fTemp1,fTemp2,fTemp3, fTemp4); 
					return true;
				}
			}	
		kItor++;
		};
	};
	return false;
}

bool Property::HandleSetValue( string kValueName ,string kValue )
{
	return false;
}

bool Property::HandleGetValue( string kValueName )
{
	return false;
}


Property::PropertyValues::PropertyValues()
{
	fLowerBound=FLT_MIN;
	fUpperBound=FLT_MAX;
		
}




