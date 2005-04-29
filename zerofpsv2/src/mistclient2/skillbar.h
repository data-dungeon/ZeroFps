#ifndef _SKILLBAR_H_
#define _SKILLBAR_H_

#include "../zerofpsv2/basic/zguiskin.h"

#include <vector>
#include <string>

using namespace std;

class SkillInfo
{
	public:
		bool		m_bInUse;
		string	m_strSkillName;
		string	m_strSkillIcon;

		float		m_fReloadTimeLeft;
};

class SkillBar
{
	private:
		vector<SkillInfo>	m_kSkills;
		

	public:
		SkillBar();

		void Update();
};


#endif


