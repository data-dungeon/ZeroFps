#pragma once

#include <vfw.h>						// Header File For Video For Windows
#pragma comment( lib, "vfw32.lib" )		// Search For VFW32.lib While Linking

class AVIFrameGrabber
{
public:
	AVIFrameGrabber(void);
	~AVIFrameGrabber(void);

	bool OpenAVI(char* szFile);
	void CloseAVI();

	char* GetFramePixels(int frame);
	int GetWidth() { return width; }
	int GetHeight() { return height; }
	int GetNumFrames() { return lastframe; }
	int GetFramesPerSecond() { return mpf; }

private:
	void GrabAVIFrame(int frame);

	AVISTREAMINFO		psi;		// Pointer To A Structure Containing Stream Info
	PAVISTREAM			pavi;		// Handle To An Open Stream
	PGETFRAME			pgf;		// Pointer To A GetFrame Object
	int					mpf;		// Will Hold Rough Milliseconds Per Frame
	long				lastframe;	// Last Frame Of The Stream
	char				*pdata;		// Pointer To Texture Data
	int width;
	int height;

	bool m_bOpen;
};
