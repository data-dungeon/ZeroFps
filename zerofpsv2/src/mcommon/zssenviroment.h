#ifndef _ZSSENVIROMENT_H_
#define _ZSSENVIROMENT_H_


#include "mcommon_x.h"
#include "../zerofpsv2/basic/zfsubsystem.h"
#include "../zerofpsv2/basic/zfresource.h"
#include "../zerofpsv2/basic/zfini.h"
#include "../zerofpsv2/basic/math.h"
#include "../zerofpsv2/engine/zerofps.h"

#include "zssmltime.h"

using namespace std;

class P_Enviroment;


/**	\brief	Da EnvSetting
		\ingroup Common
*/

class MCOMMON_API EnvSetting
{
	private:
		ZFIni		m_kIni;
		string	m_strName;
	
		string	m_strMusic;
		string	m_strParticles;
		
		string	m_strSpace;		
		string	m_strSky[4];		//4 skys day,evening,night,morning (only used when sun deffuse color R is -1 (day / night cycles active) )
		string	m_strCloudLow;				
		string	m_strCloudHi;				
		
		Vector4	m_kSunSpecularColor;
		Vector4	m_kSunDiffuseColor;
		Vector4	m_kSunAmbientColor;		
		Vector3	m_kSunPos;
		
		bool		m_bSunFlare;
		bool		m_bThunder;
		
		float		m_fFogStart;
		float		m_fFogStop;		
		Vector4	m_kFogColor[4];
		
		int 		m_iRain;
	
	public:
		EnvSetting();
		~EnvSetting();	
	
		bool LoadEnviroment(const char* czName);
		void Clear();
		//bool Create(string strName);	//	for resource system
		//int  CalculateSize();		

	friend class P_Enviroment;
	friend class ZSSEnviroment;
};

MCOMMON_API ZFResource* Create__EnvSetting();

//---end of EnvSetting

class MCOMMON_API EnvList
{
	private:
		ZFIni		m_kIni;
		
	public:
	
		string					m_strName;	
		vector<EnvSetting*>	m_kEnvSettings;
		int						m_iCurrent;
		float						m_fLastUsed;
		float						m_fLastUpdated;
		bool						m_bChanged;
		
	EnvList()
	{
		m_iCurrent = 		0;
		m_fLastUsed = 		0;
		m_fLastUpdated =	0;
		m_bChanged = 		true;
	};
	
	~EnvList()
	{
		//delete all enviroments
		for(int i = 0 ;i<m_kEnvSettings.size();i++)
		{
			delete m_kEnvSettings[i];
		}
	};
	
	bool Load(const string& strFile);
};


class MCOMMON_API ZSSEnviroment : public ZFSubSystem 
{	
	private:
		enum FuncId_e
		{
			FID_CHANGEENVIROMENT,
		};
	
		ZSSZeroFps*					m_pkZeroFps;
		
		vector<P_Enviroment*>	m_kPEnviroments;
		vector<EnvList*>			m_kEnvLists;
		
		float 						m_fEnviromentChange;
		float							m_fEnviromentTimeout;		
		
		void UpdatePEnv(P_Enviroment* pkPEnv);
		EnvList* GetEnvList(const string& strEnviroment);
		void UpdateEnvLists();
		void UpdateEnvList(EnvList* pkEnvList);
		
	public:
		ZSSEnviroment();
		bool StartUp();
		bool ShutDown()		{return true;};
		bool IsValid()			{return true;};
		void RunCommand(int cmdid, const ConCommandLine* kCommand);		
		
		void Update();
		
		void Register(P_Enviroment* pkPEnviroment);
		void UnRegister(P_Enviroment* pkPEnviroment);
};




#endif







