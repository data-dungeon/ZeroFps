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
		bool m_bPrevUseGLSLState;		
		bool m_bPrevShadowMapState;
		bool m_bPrevShadowMapRealtimeState;
		int  m_iPrevShadowmapQuality;
		bool m_bPrevVegetationState;
		int m_iCurrentShadowGroup;
		int m_iCurrentPage;
		float m_fPrevVolume;
		//float m_fPrevSoundVolume;
		float m_fPrevMouseSens;
		int m_iPrevNetSpeed;
		int m_iNewNetSpeed;

	} m_kOptionsValues;

	void Open();
	void Close(bool bSave);

private:
	MistClient* m_pkMC;
	ZShaderSystem* m_pkZShaderSystem;
	ZShadow* m_pkZShadow;
	ZSSAudioSystem* m_pkAudioSys;
	ZSSRender* m_pkRender;
	ZSSInput* m_pkInput;
	ZSSZeroFps* m_pkZeroFps;
};

#endif




