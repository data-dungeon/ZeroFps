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
	 Läs in en bild varje frame till minnet OCH skriv i samma textur varje frame.
	 [+] Kostar lite texturminne efetrsom den återanvännder samma textur.
	 [+] Laddas in efter hand, kräver ingen laddningstid.
	 [-] Kostar CPU eftersom den skriver i en textur varje frame.
	 [-] Dekryptering görs varje frame, vilket kan slöa ner för jpeg och 8bit.*/
	READ_EVERY_FRAME_and_WRITE_IN_TEXTURE_EVERY_FRAME = 0,
	/*--------------------------------------------------------------------------*/

	/**	\brief ----------------------------------------------------------------------------
	 Läs in alla bilder till minnet OCH skriv i samma textur varje frame.
	 [+] Kostar lite texturminne efetrsom den återanvännder samma textur.
	 [+] Dekryptering görs endast när animationen läses in och ej varje frame (bra för jpeg och 8bit).
	 [-] Kostar CPU eftersom den skriver i en textur varje frame.
	 [-] Tar längre tid att ladda in. */
	READ_ALL_and_WRITE_IN_TEXTURE_EVERY_FRAME = 1,
	/*--------------------------------------------------------------------------*/

	/**	\brief ----------------------------------------------------------------------------
	 Läs in en bild varje frame till minnet OCH spara ner varje bild som en ny
	 textur allt eftersom den renderar.
	 Efter första loopvaret behövs ingen läsning göras.
	 [+] Snabb rendering då varje bild får sin egen textur.
	 [+] Laddas in efter hand, kräver ingen laddningstid. 
	 [-] Kostar mycket texturminne .
	 [-] Dekryptering görs varje frame, vilket kan slöa ner för jpeg och 8bit.*/
	READ_EVERY_FRAME_TO_VIDEOMEMORY = 2,
	/*----------------------------------------------------------------------------

	/**	\brief ----------------------------------------------------------------------------
	 Läs in alla bilder till minnet OCH spara ner varje bild som en ny 
	 textur allt eftersom den renderar.
	 [+] Snabb rendering då varje bild får sin egen textur.
	 [+] Dekryptering görs endast när animationen läses in och ej varje frame (bra för jpeg och 8bit).
	 [-] Kostar mycket texturminne .
	 [-] Tar längre tid att ladda in. */
	READ_ALL_TO_VIDEOMEMORY = 3
	/*--------------------------------------------------------------------------*/
};

enum ZIFFormat
{
	ZIF_RGB24	= 0,	///< Jätte snabb, jätte snygg, jätte stor.
	ZIF_RGB8	= 1,	///< Mellan stor, ganska ful, ganska snabb
	ZIF_JPEG	= 2,	///< Minst storlek, mest CPU krävande
};

enum ZIFOptions 
{
	ZIF_PLAY_ONES	= 1,	///< Spela animationen bara 1 gång.
};

/**	\brief	Class to display a GUI animation.
		\ingroup Basic

  A animation object for a ZGuiskin to loop between several textures 
  instead of just one.
*/
class BASIC_API ZIFAnimation
{
public:
	
	/**	\brief	Animationens filnamn som oxå används för att generera ett 
				unikt texturnamn.
	*/	
	char* m_szFileName; 
	
	/**	\brief	Pekare till ZGuiSkin::m_rcBkTile som används för att kunna 
				rendera ett område på texturen ifall texturen är större än 
				bilden.
	*/	
	Rect* m_pkTextureRect;

	int m_iWidth, m_iHeight; ///< Bredd, höjd på varje textur i animationen.
	bool m_bPlay; ///< Spela/Stopa


							
	ZIFAnimation(char* szFileName=0, int iMode=0, int iOptions=0, 
		Rect* pkTextureRect=0);
	~ZIFAnimation();

	bool Update(bool& r_bRebuildTexture);	///< Returnerar TRUE om animationen har bytt frame.
	
	/**	\brief	Returnerar en pekare till RGB data till aktuell frame i 
				animationen.
	*/	
	char* GetFramePixels(); 
	char* GetTexIDName(); ///< Hämta resursnamn på aktuell textur som används.

private:

	/**	\brief	Använd en och samma textur. OBS! _BÖR_ vara satt till true 
				annars ryker allt texturminne!!! Endast små animationer 
				(på typ max: 40 frame) kan dra nytta av att sätta denna till 
				false för att slippa bygga om texturen varje frame.
	*/	
	bool m_bRebuildTexture; 

	float m_fLastTick; ///< Senast tid då framen uppdaterades.
	int m_iCurrentFrame; ///< Aktuell frame som visas.
	unsigned int m_iFileOffset; ///< Skriv offset in i filen som streamas
	
	/**	\brief	TRUE = Läs en frame i taget från disk, FALS = Läs in alla 
				frames i början.
	*/
	bool m_bStream;	

	int m_iNumFrames; ///< Antal frames i animationen.
	
	/**	\brief	Hur många gånger per sekund varje frame skall spelas.
	*/
	int m_iMsecsPerFrame;

	int m_iPixelDataSize; ///< Hur stor varje frame är i Byte (på texturen).

	char* m_pPixelData; ///< RGB data som texturmanagern tittar på.
	
	/**	\brief	Används för att decoda JPG filer. I klassen sparas 
				arbetsbufferten.
	*/
	JpgDecoder* m_pkJPGDec;
	
	/**	\brief	Pekare till en array med storlek på varje jpgbild i 
				animationskedjan.
	*/
	int* m_pFramesSizesJPG; 
	
	char m_szTexIDName[16]; ///< Unikt TexturID namn för varje zif animation
	
	/**	\brief	Bestämmer hur animationen skall läsas in och renderas. 
	*/
	ZIFAnimationMode m_eMode;
	
	bool m_bLoop; ///< Om animation skall loopa eller inte.
	
	/**	\brief	Vilket komprimeringsformat som har använts i filen.
	*/
	ZIFFormat m_eFormat; 
	
	/**	\brief	Bredd och höjd på varje bild i animationen.
	*/
	int m_iImageFrameWidth, m_iImageFrameHeight;

	bool Read(); ///< Läs bildata från fil.

	/**	\brief	Läs information om animationsfilen från fil.
	*/
	int ReadHeader(FILE* pkFile);
	void DeallocateWorkMemory(); ///< Frigör arbetsminne.
};

/**	\brief	Singleton to manage to share workmemory between diffrent 
			ZIFAnimations.
		\ingroup Basic

  Resourcemanager för ZIF animations. All data used by ZIF animations are 
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
		char* m_pPixelData; ///< RGB data som texturmanagern tittar på.

		/**	\brief	startID, om varje animationsframe har sin 
					egen textur.
		*/
		int m_iIDTexArrayStart;

		set<ZIFAnimation*> m_kUses;
	};

	map<string, ZIF_DATA*> m_kZIFData; ///< filename, data
	~ZIFMemMan();

public:

	/**	\brief	Hämta en pekare till Arbetsdatat. Skapa eventuellt nytt 
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






