#ifndef _SOUNDBUFFERMANAGER_H_
#define _SOUNDBUFFERMANAGER_H_ 

#include "../basic/basic.pkg"
#include <string.h>
#include <vector>
#include "AL/al.h"
#include "AL/alut.h"
#include "engine_x.h"

class ENGINE_API SoundBuffer {
	public:
		ALuint m_iIndex;
		string m_acFile;
};

class ENGINE_API SoundBufferManager : public ZFObject {
	private:
		vector<SoundBuffer*> m_kBuffers;
				
		FileIo *io;
	
		int Load(char* acFile);
		int GetIndex(char* acFile);

		
	public:
		SoundBufferManager(FileIo * pkIo);

		int LoadSound(char* acFile);
		void ClearAll();

};


#endif



