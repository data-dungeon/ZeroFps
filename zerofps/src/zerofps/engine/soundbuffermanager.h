#ifndef _SOUNDBUFFERMANAGER_H_
#define _SOUNDBUFFERMANAGER_H_ 

//#include "vector3.h"
#include "../basic/basic.pkg"
#include <string.h>
#include <vector>
//#include "fileio.h"
#include "AL/al.h"
#include "AL/alut.h"

class SoundBuffer {
	public:
		ALuint m_iIndex;
		string m_acFile;
};

class SoundBufferManager{
	private:
		vector<SoundBuffer*> m_kBuffers;
				
		FileIo *io;
	
		int Load(char* acFile);
		int GetIndex(char* acFile);

		
	public:
		SoundBufferManager();

		int LoadSound(char* acFile);
		void ClearAll();

};


#endif



