// zfaudiosystem2.cpp: implementation of the ZFAudioSystem2 class.
//
//////////////////////////////////////////////////////////////////////

#include <AL/al.h>
#include <AL/alut.h>
#include "zfaudiosystem2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZFAudioSystem2::ZFAudioSystem2() : ZFSubSystem("ZFAudioSystem2") 
{
	m_pkMusic = NULL;
}

ZFAudioSystem2::~ZFAudioSystem2()
{
//	if(m_pkMusic)
//		delete m_pkMusic;
}

bool ZFAudioSystem2::StartUp()
{
	m_pkMusic = static_cast<OggMusic*>(g_ZFObjSys.GetObjectPtr("OggMusic"));

/*   alutInit (NULL, 0); 
   alListenerf(AL_GAIN, 1.0f);
   alDopplerFactor(1.0f); 
   alDopplerVelocity(343); */

	return true;
}

bool ZFAudioSystem2::ShutDown()
{
	return true;
}

bool ZFAudioSystem2::IsValid()
{
	return true;
}

void ZFAudioSystem2::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) 
	{
		case FID_MUSICLOAD:
			if(kCommand->m_kSplitCommand.size() > 1)
				m_pkMusic->LoadFile(kCommand->m_kSplitCommand[1]);
			break;
		
		case FID_MUSICPLAY:
			m_pkMusic->Play();
			break;

		case FID_MUSICSTOP:
			m_pkMusic->Stop();
			break;
		
		case FID_MUSICVOLUME:
			if(kCommand->m_kSplitCommand.size() > 1)
				m_pkMusic->SetVolume((float) 
					strtod( (kCommand->m_kSplitCommand[1]).c_str(),NULL));
			break;
			
		case FID_MUSICBUFFERS:
			{
				if(kCommand->m_kSplitCommand.size() > 1)
				{
					int iTemp = atoi((kCommand->m_kSplitCommand[1]).c_str());
					if(iTemp > 0)
					{
						m_pkMusic->Stop();
						delete m_pkMusic;
						m_pkMusic = new OggMusic(iTemp, 4096);
					}
				}
			}
			break;
	}
}

void ZFAudioSystem2::Update()
{
	m_pkMusic->Update();
}
