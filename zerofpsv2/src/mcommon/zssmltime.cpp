#include "zssmltime.h"

ZSSMLTime::ZSSMLTime() : ZFSubSystem("ZSSMLTime") 
{
	m_iYear			=	0;
	m_iMonth			=	0;
	m_iWeek			=	0;
	m_iDay			=	0;
	
	m_iHour			=	0;
	m_iMinute		=	0;
	m_iSecond		=	0;
	
	m_dTotalTime	=	0;
	m_fScale 		=	6.0;	

	
	//RegisterVariable("mltimescale",&m_fScale,CSYS_FLOAT);			
}

bool ZSSMLTime::StartUp()
{
	
	return true;
}


void ZSSMLTime::SetTime(double dSecondsST)
{
	//scale time
	dSecondsST *= m_fScale;
	
	//save total time
	m_dTotalTime = dSecondsST;

	//year
	m_iYear = int(dSecondsST /  44236800);
	dSecondsST -= double(m_iYear) * 44236800;

	//month
	m_iMonth = int(dSecondsST / 5529600);
	dSecondsST -= double(m_iMonth) *5529600;
	
	
	//week
	m_iWeek = int(dSecondsST / 691200);
	dSecondsST -= double(m_iWeek) * 691200;

	//day
	m_iDay = int(dSecondsST / 86400);
	dSecondsST -= double(m_iDay) * 86400;
	
	//hour
	m_iHour = int(dSecondsST / 3600);
	dSecondsST -= double(m_iHour) * 3600;
	
	//minute
	m_iMinute = int(dSecondsST / 60);
	dSecondsST -= double(m_iMinute) * 60;
	
	//m_iSecond
	m_iSecond = int(dSecondsST);
};

string ZSSMLTime::GetDateString()
{
	

/*	return	IntToString(m_fTotalTime) + string("=") + IntToString(m_iYear) + string("-") + GetMonthName(m_iMonth) +
				string("-") + IntToString(m_iWeek) + string("-") + GetDayName(m_iDay) + string("-") + 
				IntToString(m_iHour) + string(":") + IntToString(m_iMinute) + string(":") + IntToString(m_iSecond);*/
				
	return	GetDayName(m_iDay)+ string(" ") + GetMonthName(m_iMonth) + string(" ") + IntToString(8*m_iWeek + m_iDay + 1) + 
				string(" ") + IntToString(m_iHour)+string(":")+IntToString(m_iMinute)+string(":")+IntToString(m_iSecond) +
				string(" ") + IntToString(m_iYear);
}

string ZSSMLTime::GetDayName(int iDay)
{
	switch(iDay)
	{
		case 0:
			return string("Monday");
		case 1:
			return string("Tuesday");
		case 2:
			return string("Wednesday");
		case 3:
			return string("Thursday");
		case 4:
			return string("Friday");
		case 5:
			return string("Saturday");
		case 6:
			return string("Sunday");
		case 7:
			return string("Mistday");
			
		default:
			return string("ERRORDAY");
	}
}

string ZSSMLTime::GetMonthName(int iMonth)
{
	switch(iMonth)
	{
		case 0:
			return string("January");
		case 1:
			return string("February");
		case 2:
			return string("March");
		case 3:
			return string("April");
		case 4:
			return string("May");
		case 5:
			return string("June");
		case 6:
			return string("July");
		case 7:
			return string("August");

		default:
			return string("ERRORMONTH");				
	}
}










