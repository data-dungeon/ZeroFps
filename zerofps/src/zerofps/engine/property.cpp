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
					int  iDecimal, iSign;
					kBuffer= fcvt(*((float*)kItor->pkValue), 5, &iDecimal, &iSign );
					kBuffer.insert(iDecimal, ".");
					if(iSign !=0)
						kBuffer.insert(0, "-");
					return kBuffer; 

				};
				
			}	
		kItor++;
		};

	};
	return kBuffer;
}


bool Property::SetValue(string kValueName, string kValue)
{
	vector<PropertyValues> kTemp= GetPropertyValues();
	if(!kTemp.empty())
	{
	
		vector<PropertyValues>::iterator kItor = kTemp.begin();
		while (kItor != kTemp.end())
		{
			if( kValueName == kItor->kValueName)
			{
				
				
				switch(kItor->iValueType)
				{	
				case VALUETYPE_INT:
					*((int*)kItor->pkValue)=atoi(kValue.c_str());
					return true;
					
				case VALUETYPE_STRING:
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
					char *stop;
					*((float*)kItor->pkValue) = strtod( kValue.c_str(), &stop );
					return true;
					

				};
				
			}	
		kItor++;
		};

	};
	return false;
}





