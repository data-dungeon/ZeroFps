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
}


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
				
					((Vector4*)kItor->pkValue)->set(fTemp1,fTemp2,fTemp3,fTemp4); 
					return true;
					
				
				};
				
			}	
		kItor++;
		};

	};
	return false;
}

Mat::PropertyValues::PropertyValues()
{
	fLowerBound=FLT_MIN;
	fUpperBound=FLT_MAX;
	iNumberOfValues = 1;	
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
	return -1;
}



