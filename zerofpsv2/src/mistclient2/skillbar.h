#ifndef _SKILLBAR_H_
#define _SKILLBAR_H_

#include "../zerofpsv2/basic/zguiskin.h"
#include "../zerofpsv2/gui/zguilabel.h"

#include <vector>
#include <string>

class ZGuiButton;

using namespace std;

class SkillInfo
{
	public:
		string	m_strSkillName;
		string	m_strSkillScreenName;

		float		m_fReloadTimeLeft;
		float		m_fReloadTimeTotal;
		
		int		m_iSkillType;
		int		m_iTargetType;

		ZGuiButton*	m_pkButton;	
};

class SkillNetInfo
{
	public:
		char		m_cPos;
		string	m_strSkillName;
		string	m_strSkillScreenName;
		string	m_strSkillIcon;

		float		m_fReloadTimeLeft;
		float		m_fReloadTimeTotal;
		char		m_cSkillType;
		char		m_cTargetType;
};


class SkillBar
{
	private:
		vector<SkillInfo>	m_kSkills;
		ZGuiLabel* 			m_pkCurrentPrimarySkill;
		ZGuiLabel* 			m_pkCurrentSecondarySkill;
	
		string 				m_strPrimarySkill;
		string 				m_strSecondarySkill;
		
	public:
		SkillBar();
		
		void Init();
		void Update(float fTimeDiff);
		void UpdateList(vector<SkillNetInfo> kSkillInfo);
		
		void HandleCommand(int iButton,const string& strCommand);
		
		string GetPrimarySkill()	{	return m_strPrimarySkill;		};
		string GetSecondarySkill()	{	return m_strSecondarySkill;	};
};


#endif


