// ZGuiSkin.h: interface for the ZGuiSkin class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _BASIC_ZIFANIMATIONMODE_H_
#define _BASIC_ZIFANIMATIONMODE_H_

#include "basic_x.h"
#include "rect.h"
#include <stdio.h>
#include <string>
#include <set>
#include <map>
using namespace std;

class JpgDecoder;

enum ZIFAnimationMode
{
	/**	\brief ----------------------------------------------------------------------------
	 L�s in en bild varje frame till minnet OCH skriv i samma textur varje frame.
	 [+] Kostar lite texturminne efetrsom den �teranv�nnder samma textur.
	 [+] Laddas in efter hand, kr�ver ingen laddningstid.
	 [-] Kostar CPU eftersom den skriver i en textur varje frame.
	 [-] Dekryptering g�rs varje frame, vilket kan sl�a ner f�r jpeg och 8bit.*/
	READ_EVERY_FRAME_and_WRITE_IN_TEXTURE_EVERY_FRAME = 0,
	/*--------------------------------------------------------------------------*/

	/**	\brief ----------------------------------------------------------------------------
	 L�s in alla bilder till minnet OCH skriv i samma textur varje frame.
	 [+] Kostar lite texturminne efetrsom den �teranv�nnder samma textur.
	 [+] Dekryptering g�rs endast n�r animationen l�ses in och ej varje frame (bra f�r jpeg och 8bit).
	 [-] Kostar CPU eftersom den skriver i en textur varje frame.
	 [-] Tar l�ngre tid att ladda in. */
	READ_ALL_and_WRITE_IN_TEXTURE_EVERY_FRAME = 1,
	/*--------------------------------------------------------------------------*/

	/**	\brief ----------------------------------------------------------------------------
	 L�s in en bild varje frame till minnet OCH spara ner varje bild som en ny
	 textur allt eftersom den renderar.
	 Efter f�rsta loopvaret beh�vs ingen l�sning g�ras.
	 [+] Snabb rendering d� varje bild f�r sin egen textur.
	 [+] Laddas in efter hand, kr�ver ingen laddningstid. 
	 [-] Kostar mycket texturminne .
	 [-] Dekryptering g�rs varje frame, vilket kan sl�a ner f�r jpeg och 8bit.*/
	READ_EVERY_FRAME_TO_VIDEOMEMORY = 2,
	/*----------------------------------------------------------------------------

	/**	\brief ----------------------------------------------------------------------------
	 L�s in alla bilder till minnet OCH spara ner varje bild som en ny 
	 textur allt eftersom den renderar.
	 [+] Snabb rendering d� varje bild f�r sin egen textur.
	 [+] Dekryptering g�rs endast n�r animationen l�ses in och ej varje frame (bra f�r jpeg och 8bit).
	 [-] Kostar mycket texturminne .
	 [-] Tar l�ngre tid att ladda in. */
	READ_ALL_TO_VIDEOMEMORY = 3
	/*--------------------------------------------------------------------------*/
};

enum ZIFFormat
{
	ZIF_RGB24	= 0,	///< J�tte snabb, j�tte snygg, j�tte stor.
	ZIF_RGB8	= 1,	///< Mellan stor, ganska ful, ganska snabb
	ZIF_JPEG	= 2,	///< Minst storlek, mest CPU kr�vande
};

enum ZIFOptions 
{
	ZIF_PLAY_ONES	= 1,	///< Spela animationen bara 1 g�ng.
};

/**	\brief	Class to display a GUI animation.
		\ingroup Basic

  A animation object for a ZGuiskin to loop between several textures 
  instead of just one.
*/
class BASIC_API ZIFAnimation
{
public:
	
	/**	\brief	Animationens filnamn som ox� anv�nds f�r att generera ett 
				unikt texturnamn.
	*/	
	char* m_szFileName; 
	
	/**	\brief	Pekare till ZGuiSkin::m_rcBkTile som anv�nds f�r att kunna 
				rendera ett omr�de p� texturen ifall texturen �r st�rre �n 
				bilden.
	*/	
	Rect* m_pkTextureRect;

	int m_iWidth, m_iHeight; ///< Bredd, h�jd p� varje textur i animationen.
	bool m_bPlay; ///< Spela/Stopa


							
	ZIFAnimation(char* szFileName=0, int iMode=0, int iOptions=0, 
		Rect* pkTextureRect=0);
	~ZIFAnimation();

	bool Update(bool& r_bRebuildTexture);	///< Returnerar TRUE om animationen har bytt frame.
	
	/**	\brief	Returnerar en pekare till RGB data till aktuell frame i 
				animationen.
	*/	
	char* GetFramePixels(); 
	char* GetTexIDName(); ///< H�mta resursnamn p� aktuell textur som anv�nds.

private:

	/**	\brief	Anv�nd en och samma textur. OBS! _B�R_ vara satt till true 
				annars ryker allt texturminne!!! Endast sm� animationer 
				(p� typ max: 40 frame) kan dra nytta av att s�tta denna till 
				false f�r att slippa bygga om texturen varje frame.
	*/	
	bool m_bRebuildTexture; 

	float m_fLastTick; ///< Senast tid d� framen uppdaterades.
	int m_iCurrentFrame; ///< Aktuell frame som visas.
	unsigned int m_iFileOffset; ///< Skriv offset in i filen som streamas
	
	/**	\brief	TRUE = L�s en frame i taget fr�n disk, FALS = L�s in alla 
				frames i b�rjan.
	*/
	bool m_bStream;	

	int m_iNumFrames; ///< Antal frames i animationen.
	
	/**	\brief	Hur m�nga g�nger per sekund varje frame skall spelas.
	*/
	int m_iMsecsPerFrame;

	int m_iPixelDataSize; ///< Hur stor varje frame �r i Byte (p� texturen).

	char* m_pPixelData; ///< RGB data som texturmanagern tittar p�.
	
	/**	\brief	Anv�nds f�r att decoda JPG filer. I klassen sparas 
				arbetsbufferten.
	*/
	JpgDecoder* m_pkJPGDec;
	
	/**	\brief	Pekare till en array med storlek p� varje jpgbild i 
				animationskedjan.
	*/
	int* m_pFramesSizesJPG; 
	
	char m_szTexIDName[16]; ///< Unikt TexturID namn f�r varje zif animation
	
	/**	\brief	Best�mmer hur animationen skall l�sas in och renderas. 
	*/
	ZIFAnimationMode m_eMode;
	
	bool m_bLoop; ///< Om animation skall loopa eller inte.
	
	/**	\brief	Vilket komprimeringsformat som har anv�nts i filen.
	*/
	ZIFFormat m_eFormat; 
	
	/**	\brief	Bredd och h�jd p� varje bild i animationen.
	*/
	int m_iImageFrameWidth, m_iImageFrameHeight;

	bool Read(); ///< L�s bildata fr�n fil.

	/**	\brief	L�s information om animationsfilen fr�n fil.
	*/
	int ReadHeader(FILE* pkFile);
	void DeallocateWorkMemory(); ///< Frig�r arbetsminne.
};

/**	\brief	Singleton to manage to share workmemory between diffrent 
			ZIFAnimations.
		\ingroup Basic

  Resourcemanager f�r ZIF animations. All data used by ZIF animations are 
  destroyed when the destructor is called.
*/
class ZIFMemMan
{
private:
	ZIFMemMan() { } ///< Prevent creation of Apple instances from client code
	ZIFMemMan(const ZIFMemMan&); ///< Disable copying
	ZIFMemMan& operator=(const ZIFMemMan&); ///< Disable copying

	struct ZIF_DATA {
		JpgDecoder* m_pkJPGDec;
		char* m_pPixelData; ///< RGB data som texturmanagern tittar p�.

		/**	\brief	startID, om varje animationsframe har sin 
					egen textur.
		*/
		int m_iIDTexArrayStart;

		set<ZIFAnimation*> m_kUses;
	};

	map<string, ZIF_DATA*> m_kZIFData; ///< filename, data
	~ZIFMemMan();

public:

	/**	\brief	H�mta en pekare till Arbetsdatat. Skapa eventuellt nytt 
			    DATA objekt.
	*/
	ZIF_DATA* GetData(ZIFAnimation* pkAnimation);

	static ZIFMemMan* GetInstance()
	{
		static ZIFMemMan instance;
		return &instance;
	}
};

#endif






