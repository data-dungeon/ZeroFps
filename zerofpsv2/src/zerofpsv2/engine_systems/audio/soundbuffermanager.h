#ifndef _ENGINE_SOUNDBUFFERMANAGER_H_
#define _ENGINE_SOUNDBUFFERMANAGER_H_ 

#include <AL/alut.h>
#include <AL/al.h>
#include <string.h>
#include <vector>
#include "../engine_systems_x.h"
#include <string>
#include "../../basic/zfobjectmanger.h"

using namespace std;

class ENGINE_SYSTEMS_API SoundBuffer {
	public:
		ALuint m_iIndex;
		string m_acFile;
};

class ENGINE_SYSTEMS_API SoundBufferManager : public ZFSubSystem {
	private:
		vector<SoundBuffer*> m_kBuffers;
				
		int Load(char* acFile);
		int GetIndex(char* acFile);

		
	public:
		SoundBufferManager();

		int LoadSound(char* acFile);
		void ClearAll();

};


#endif



