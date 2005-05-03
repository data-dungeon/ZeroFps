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
		string	m_strSkillIcon;

		float		m_fReloadTimeLeft;
		
		ZGuiButton*	m_pkButton;
	
};

class SkillBar
{
	private:
		vector<SkillInfo>	m_kSkills;
		

	public:
		SkillBar();
		
		void Init();
		void UpdateList(vector<SkillInfo> kSkillInfo);
};


#endif


