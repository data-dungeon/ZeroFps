#include <windows.h>
#include ".\aviframegrabber.h"

AVIFrameGrabber::AVIFrameGrabber(void)
{
	pdata = NULL;
	width = 0;
	height = 0;
	m_bOpen = false;
}

AVIFrameGrabber::~AVIFrameGrabber(void)
{
	if(m_bOpen)
		CloseAVI();
}

bool AVIFrameGrabber::OpenAVI(char* szFile)
{
	if(m_bOpen == true)
		return true;

	AVIFileInit();

	// Opens The AVI Stream
	if (AVIStreamOpenFromFile(&pavi, szFile, streamtypeVIDEO, 0, OF_READ, NULL) !=0)
	{
		m_bOpen = false;
		return false;
	}

	// Reads Information About The Stream Into psi
	AVIStreamInfo(pavi, &psi, sizeof(psi));						
	width=psi.rcFrame.right-psi.rcFrame.left;					
	height=psi.rcFrame.bottom-psi.rcFrame.top;					

	// The Last Frame Of The Stream
	lastframe=AVIStreamLength(pavi);							

	// Calculate Rough Milliseconds Per Frame
	mpf=AVIStreamSampleToTime(pavi,lastframe)/lastframe;		

	// Create The PGETFRAME	Using Our Request Mode
	pgf=AVIStreamGetFrameOpen(pavi, NULL);						
	if (pgf==NULL)
	{
		// An Error Occurred Opening The Frame
		m_bOpen = false;
	}

	m_bOpen = true;

	return m_bOpen;
}

void AVIFrameGrabber::CloseAVI(void)
{
	if(m_bOpen)
	{
		AVIStreamGetFrameClose(pgf);	// Deallocates The GetFrame Resources
		AVIStreamRelease(pavi);			// Release The Stream
		AVIFileExit();					// Release The File
		m_bOpen = false;
	}
}

void AVIFrameGrabber::GrabAVIFrame(int frame)			
{
	LPBITMAPINFOHEADER lpbi;									
	lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf, frame);	
	pdata=(char *)lpbi+lpbi->biSize+lpbi->biClrUsed * sizeof(RGBQUAD);	
}

char* AVIFrameGrabber::GetFramePixels(int frame) 
{ 
	GrabAVIFrame(frame);
	return pdata;
}