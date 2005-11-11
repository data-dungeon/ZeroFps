#ifndef _ZSSMLTIME_H_
#define _ZSSMLTIME_H_

#include <string>
#include "../zerofpsv2/basic/globals.h"
#include "../zerofpsv2/basic/zfsubsystem.h"
#include "../zerofpsv2/basic/concommand.h"
#include "mcommon_x.h"

using namespace std;

class MCOMMON_API ZSSMLTime : public ZFSubSystem 
{
	private:
		// RT = real time (time in reality)
		// ST = sim time (time in engine simulation)
		// MT = mistland time (time in mistlands)
		
		//ML
		int	m_iYear;				// 8 months 	44236800 seconds
		int	m_iMonth;			// 8 weeks,		5529600 seconds
		int	m_iWeek;				// 8 days,		691200 seconds
		int	m_iDay;				// 24 Hour,		86400 seconds
		
		int	m_iHour;				// 60 minutes,	3600 seconds
		int	m_iMinute;			// 60 second 
		int	m_iSecond;			// 1 second 
		
		double	m_dTotalTimeMT;		// total time in seconds GT				
		double	m_dTotalTimeRT;
//		float 	m_fScale;
		
		ConVar	m_kfScale;

	public:
		ZSSMLTime();
		bool StartUp();
		bool ShutDown()		{return true;};
		bool IsValid()			{return true;};				
		
		void SetTime(double dTotalTimeML);		
		void AddTime(double dSecondsRT);
				
		string GetDayName(int iDay);
		string GetMonthName(int iMonth);
		string GetDateString();
		
		int	GetYear()			{	return m_iYear;			};
		int	GetMonth()			{	return m_iMonth;			};
		int	GetWeek()			{	return m_iWeek;			};
		int	GetDay()				{	return m_iDay;				};
		int	GetHour()			{	return m_iHour;			};
		int	GetMinute()			{	return m_iMinute;			};
		int	GetSecond()			{	return m_iSecond;			};
		float GetScale()			{	return m_kfScale.GetFloat();			};		
		double GetTotalMLTime()	{	return m_dTotalTimeMT;	};
};



#endif









