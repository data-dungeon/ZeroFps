#ifndef _GUI_OPTIONSDLG_H_
#define _GUI_OPTIONSDLG_H_

class MistClient;

class OptionsDlg
{
public:
	OptionsDlg(MistClient* pkMistClient);
	~OptionsDlg();

	struct OPTIONS_VALUES
	{
		float m_fGammaColors[3];
		bool m_bLockGammaColors;
		bool m_abEnabledShadowGroups[6]; // 0:an använs inte...
		bool m_bPrevShadowMapState;
		int m_iCurrentShadowGroup;
		int m_iCurrentPage;
		float m_fPrevMusicVolume;
		float m_fPrevSoundVolume;
		float m_fPrevMouseSens;

	} m_kOptionsValues;

	void Open();
	void Close(bool bSave);

private:
	MistClient* m_pkMC;
	ZShaderSystem* m_pkZShaderSystem;
	ZShadow* m_pkZShadow;
	ZFAudioSystem* m_pkAudioSys;
	Render* m_pkRender;
	Input* m_pkInput;
	ZeroFps* m_pkZeroFps;
};

#endif