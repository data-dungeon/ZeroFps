#include "globals.h"

#define NRDEG_IN_ONE_RAD	57.29577951
#define NRRAD_IN_ONE_DEG	0.0174532952

void Gemens(char* aText) 
{
	int i=0;
	
	while(aText[i]!='\0'){
		//gems  96>x<123     vers  64>x<91
		int code=int(aText[i]);
		
		if(code>64 && code<91)
			aText[i]=char(code+32);

		i++;
	}
};

void IntToChar(char* aText,int iValue)
{
	strcpy(aText,"");
	bool found=false;
	
	for(int i=10000000;i>0;i/=10) {
		if(iValue/i>0){
			int nr=int(iValue/i);
//			cout<<nr;
			iValue-=i*nr;
			found=true;
			nr+=48;
			strncat(aText,(char*)&(nr),1);
		} else {
			if(found==true)
				strcat(aText,"0");			
//			cout<<0;		
		}
	}
}
	
float BASIC_API DegToRad(float fAngle)
{
	return (fAngle * NRRAD_IN_ONE_DEG);
}

float BASIC_API RadToDeg(float fAngle)
{
	return (fAngle * NRDEG_IN_ONE_RAD);
}
