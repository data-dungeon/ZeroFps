#pragma once

#define BYTE unsigned char
#define WORD unsigned short int

#define DWORD unsigned int
#define SDWORD signed int

#define SBYTE signed char
#define SWORD signed short int

struct HuffmanTable
{
	BYTE m_ucLength[17];  // k =1-16 ; L[k] indicates the number of Huffman codes of length k
	WORD m_uiMinorCode[17];  // indicates the value of the smallest Huffman code of length k
	WORD m_uiMajorCode[17];  // similar, but the highest code
	BYTE m_ucV[65536];  // V[k][j] = Value associated to the j-th Huffman code of length k
	// High nibble = nr of previous 0 coefficients
	// Low nibble = size (in bits) of the coefficient which will be taken from the data stream
};

class JpgDecoder
{
public:

	JpgDecoder();
	~JpgDecoder(void);

	void AllocateBuffers(DWORD uiFileSize, DWORD uiWidth, DWORD uiHeight);
	void LoadHeader(FILE *fp, DWORD uiReadSize, DWORD *X_image, DWORD *Y_image);
	void GetBuffer(WORD X_image,WORD Y_image, BYTE **address_dest_buffer);
	void Decode();
	
private:
	
	void CalculateTabs();
	void CalculateMask();
	void InitJPGDecoding();
	void prepare_range_limit_table();
	void Resync();
	void InitQT();
	void LoadQuantTable(float *quant_table);
	void LoadHuffmanTable(HuffmanTable *HT);
	void PrecalculateCrCbTables();

	void ProcessHuffmanDataUnit(BYTE DC_nr, BYTE AC_nr,SWORD *previous_DC);
	void IDCTTransform(SWORD *incoeff,BYTE *outcoeff,BYTE Q_nr);
	void Convert8x8YCbCrToRGB(BYTE *Y, BYTE *Cb, BYTE *Cr, DWORD im_loc, DWORD X_image_bytes, BYTE *im_buffer);
	void Convert8x8YCbCrToRGBTab(BYTE *Y, BYTE *Cb, BYTE *Cr, BYTE *tab, DWORD im_loc, DWORD X_image_bytes, BYTE *im_buffer);
	
	void DecodeMCU1x1(DWORD im_loc);
	void DecodeMCU2x1(DWORD im_loc);
	void DecodeMCU2x2(DWORD im_loc);
	void DecodeMCU1x2(DWORD im_loc);

	void SkipKbits(BYTE k);
	SWORD GetKbits(BYTE k);

	inline void WordP(unsigned int& i) { 
		m_uiWP=(((WORD)(m_pJPEGbuf[(i)]))<<8) + m_pJPEGbuf[(i)+1]; (i)+=2; 
	}

	inline void ByteP(unsigned int& i) { m_ucBP=m_pJPEGbuf[(i)++]; }

	inline WORD WordHiLo(BYTE byte_high,BYTE byte_low) {
		return byte_low + (byte_high << 8);
	}

	inline WORD LookKbits(BYTE k) {
		BYTE Res = 16 - k;
		return m_uiWordval >> Res;
	}

	inline unsigned short get_svalue(unsigned char k) {
		unsigned int tempval = m_uiWordval;
		tempval = (tempval << k)>>16;
		if (!(tempval & (1<<(k-1))))
			tempval += s_aiNegPow2[k];
		return tempval;
	}

	BYTE m_ucBP; //current byte
	WORD m_uiWP; //current word

	DWORD m_uiBytePos; // current byte position
	DWORD m_uiXimageBytes; // size in bytes of 1 line of the image = m_uiXRound * 4
	DWORD m_uiYincValue ; // 32*m_uiXRound; // used by DecodeMCU1x2,2x1,2x2

	BYTE *m_pJPEGbuf; // the buffer we use for storing the entire JPG file
	BYTE *m_pImBuffer; // RGBA image buffer

	WORD m_uiXRound, m_uiYRound; // The dimensions rounded to multiple of Hmax*8 (X_round) and Ymax*8 (Y_round)
	WORD m_uiHmax, m_uiVmax;
	BYTE m_ucYH, m_ucYV, m_ucCbH, m_ucCbV, m_ucCrH, m_ucCrV; // sampling factors (horizontal and vertical) for Y,Cb,Cr

	BYTE m_ucYQNr, m_ucCbQNr, m_ucCrQNr; // quantization table number for Y, Cb, Cr
	BYTE m_ucYDCNr, m_ucCbDCNr, m_ucCrDCNr; // DC Huffman table number for Y,Cb, Cr
	BYTE m_ucYACNr, m_ucCbACNr, m_ucCrACNr; // AC Huffman table number for Y,Cb, Cr

	BYTE m_ucRestartMarkers; // if 1 => Restart markers on , 0 => no restart markers
	WORD m_uiMCURestart; //Restart markers appears every MCU_restart MCU blocks

	SWORD m_iDCY, m_iDCCb, m_iDCCr; // Coeficientii DC pentru Y,Cb,Cr
	SWORD m_aiDCTCoeff[64]; // Current DCT_coefficients
	BYTE m_aucY[64], m_aucCb[64], m_aucCr[64]; // Y, Cb, Cr of the current 8x8 block for the 1x1 case
	BYTE m_aucY1[64], m_aucY2[64], m_aucY3[64], m_aucY4[64];
	BYTE m_aucTab1[64], m_aucTab2[64], m_aucTab3[64], m_aucTab4[64]; 

	BYTE m_ucDK;  // Bit displacement in memory, relative to the offset of w1. it's always <16
	WORD m_uiW1, m_uiW2; // w1 = First word in memory; w2 = Second word

	DWORD m_uiWordval ; // the actual used value in Huffman decoding.

	float* m_pfQT[4]; // quantization tables, no more than 4 quantization tables (QT[0..3])

	static SWORD s_aiNegPow2[17];
	static BYTE* s_pucRlimitTable;

	static HuffmanTable s_kHTDC[4]; //DC huffman tables , no more than 4 (0..3)
	static HuffmanTable s_kHTAC[4]; //AC huffman tables                  (0..3)

	bool m_bBuffersAllocated;
};












