// zfaudiosystem2.h: interface for the ZFAudioSystem2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZFAUDIOSYSTEM2_H__C102D4BD_588E_4554_AE5F_D84BC72D8D63__INCLUDED_)
#define AFX_ZFAUDIOSYSTEM2_H__C102D4BD_588E_4554_AE5F_D84BC72D8D63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../engine_systems_x.h"
#include "../../basic/zfresource.h"
#include "../../basic/zfobject.h"
#include "oggmusic.h"

class ENGINE_SYSTEMS_API ZFAudioSystem2  : public ZFSubSystem 
{
public:

	ZFAudioSystem2();
	virtual ~ZFAudioSystem2();

	//
	// Pure virtual functions derived from ZFSubSystem
	//
	bool StartUp();
	bool ShutDown();
	bool IsValid();

	//
	// Ogg music.
	//
	enum FuncId_e
	{
		FID_MUSICLOAD,
		FID_MUSICPLAY,
		FID_MUSICVOLUME,
		FID_MUSICBUFFERS,
		FID_MUSICSTOP
	};

	void RunCommand(int cmdid, const CmdArgument* kCommand);

	//
	// Common
	//

	void Update();

private:
	OggMusic* m_pkMusic;

};

#endif // !defined(AFX_ZFAUDIOSYSTEM2_H__C102D4BD_588E_4554_AE5F_D84BC72D8D63__INCLUDED_)
