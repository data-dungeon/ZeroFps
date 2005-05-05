#ifndef _SKILLBAR_H_
#define _SKILLBAR_H_

#include "../zerofpsv2/basic/zguiskin.h"

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
};


class SkillBar
{
	private:
		vector<SkillInfo>	m_kSkills;
		

	public:
		SkillBar();
		
		void Init();
		void Update(float fTimeDiff);
		void UpdateList(vector<SkillNetInfo> kSkillInfo);
		
		void HandleCommand(const string& strCommand);
};


#endif


