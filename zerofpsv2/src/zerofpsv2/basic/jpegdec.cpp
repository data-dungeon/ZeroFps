#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jpegdec.h"
#include "zfassert.h"

// Used markers:
#define SOI 0xD8
#define EOI 0xD9
#define APP0 0xE0
#define SOF 0xC0
#define DQT 0xDB
#define DHT 0xC4
#define SOS 0xDA
#define DRI 0xDD
#define COM 0xFE

int g_iShiftTemp;
#define RIGHT_SHIFT(x,shft)  \
	((g_iShiftTemp = (x)) < 0 ? \
	(g_iShiftTemp >> (shft)) | ((~(0L)) << (32-(shft))) : \
	(g_iShiftTemp >> (shft)))
#define DESCALE(x,n)  RIGHT_SHIFT((x) + (1L << ((n)-1)), n)
#define RANGE_MASK 1023L

char g_szErrorString[512];
void exit_func(char* function, char* desc)
{
	sprintf(g_szErrorString, "JpgDecoder::%s - %s", function, desc);
	ZFAssert(0, g_szErrorString);
}

HuffmanTable JpgDecoder::s_kHTDC[4]; //DC huffman tables , no more than 4 (0..3)
HuffmanTable JpgDecoder::s_kHTAC[4]; //AC huffman tables                  (0..3)
BYTE* JpgDecoder::s_pucRlimitTable;
SWORD JpgDecoder::s_aiNegPow2[17]= { 
	0,-1,-3,-7,-15,-31,-63,-127,-255,-511,-1023,-2047,-4095,-8191,-16383,-32767 
};

SWORD g_aiCrTab[256], g_aiCbTab[256]; // Precalculated Cr, Cb tables
SWORD g_aiCrCbGreenTab[65536];
DWORD g_uiMask[17];

BYTE zigzag[64]= {	
	0, 1, 5, 6,14,15,27,28,
	2, 4, 7,13,16,26,29,42,
	3, 8,12,17,25,30,41,43,
	9,11,18,24,31,40,44,53,
	10,19,23,32,39,45,52,54,
	20,22,33,38,46,51,55,60,
	21,34,37,47,50,56,59,61,
	35,36,48,49,57,58,62,63 
};

JpgDecoder::JpgDecoder()
{
	m_ucDK=0;
	m_bBuffersAllocated = false;
}

JpgDecoder::~JpgDecoder()
{
	free(m_pJPEGbuf);
	free(m_pImBuffer);
}

void JpgDecoder::AllocateBuffers(DWORD uiFileSize, DWORD uiWidth, DWORD uiHeight)
{
	m_pJPEGbuf=(BYTE*)malloc(uiFileSize+4);
	m_pImBuffer=(BYTE*)malloc(uiWidth*uiHeight*4);
	InitJPGDecoding();
	m_bBuffersAllocated = true;
}

void JpgDecoder::LoadHeader(FILE* pkFile, DWORD uiReadSize, DWORD* puiXimage, DWORD* puiYimage)
{
	if(m_bBuffersAllocated == false || uiReadSize==0)
	{
		DWORD pos_cur = ftell(pkFile);
		fseek(pkFile,0,SEEK_END);
		DWORD length_of_file = ftell(pkFile);
		fseek(pkFile,pos_cur,SEEK_SET);
		m_pJPEGbuf = (BYTE *)malloc(length_of_file+4);
		uiReadSize = length_of_file;
	}

	BYTE vers,units;
	WORD Xdensity,Ydensity,Xthumbnail,Ythumbnail;
	WORD length;
	float* qtable;
	DWORD old_byte_pos;
	HuffmanTable* htable;
	DWORD j;
	BYTE precision,comp_id,nr_components;
	BYTE QT_info,HT_info;
	BYTE SOS_found,SOF_found;

	fread(m_pJPEGbuf,uiReadSize,1,pkFile);

	if ((m_pJPEGbuf[0]!=0xFF)||(m_pJPEGbuf[1]!=SOI)) exit_func("LoadHeader","Not a JPG file ?\n");
	if ((m_pJPEGbuf[2]!=0xFF)||(m_pJPEGbuf[3]!=APP0)) exit_func("LoadHeader","Invalid JPG file.");
	if ( (m_pJPEGbuf[6]!='J')||(m_pJPEGbuf[7]!='F')||(m_pJPEGbuf[8]!='I')||(m_pJPEGbuf[9]!='F')||
		(m_pJPEGbuf[10]!=0) ) exit_func("LoadHeader","Invalid JPG file.");

	if(m_bBuffersAllocated == false)
		InitJPGDecoding();

	m_uiBytePos=11;

	ByteP(m_uiBytePos);vers=m_ucBP;
	if (vers!=1) exit_func("LoadHeader","JFIF version not supported");
	ByteP(m_uiBytePos); // vers_lo=m_ucBP;
	ByteP(m_uiBytePos);  units=m_ucBP;
	WordP(m_uiBytePos); Xdensity=m_uiWP; WordP(m_uiBytePos); Ydensity=m_uiWP;
	ByteP(m_uiBytePos);Xthumbnail=m_ucBP;ByteP(m_uiBytePos);Ythumbnail=m_ucBP;
	if ((Xthumbnail!=0)||(Ythumbnail!=0))
		exit_func("LoadHeader","Cannot process JFIF thumbnailed files\n");
	// Start decoding process
	SOS_found=0; SOF_found=0; m_ucRestartMarkers=0;
	while ((m_uiBytePos<uiReadSize)&&!SOS_found)
	{
		ByteP(m_uiBytePos);
		if (m_ucBP!=0xFF) continue;
		// A marker was found
		ByteP(m_uiBytePos);
		switch(m_ucBP)
		{
		case DQT: WordP(m_uiBytePos); length=m_uiWP; // length of the DQT marker
			for (j=0;j<m_uiWP-2;)
			{
				old_byte_pos=m_uiBytePos;
				ByteP(m_uiBytePos); QT_info=m_ucBP;
				if ((QT_info>>4)!=0)
					exit_func("LoadHeader","16 bit quantization table not supported");
				qtable=m_pfQT[QT_info&0xF];
				LoadQuantTable(qtable);
				j+=m_uiBytePos-old_byte_pos;
			}
			break;
		case DHT: WordP(m_uiBytePos); length=m_uiWP;
			for (j=0;j<m_uiWP-2;)
			{
				old_byte_pos=m_uiBytePos;
				ByteP(m_uiBytePos); HT_info=m_ucBP;
				if ((HT_info&0x10)!=0) 
					htable = &s_kHTAC[HT_info&0xF];
				else htable=&s_kHTDC[HT_info&0xF];
				LoadHuffmanTable(htable);
				j+=m_uiBytePos-old_byte_pos;
			}
			break;
		case COM: WordP(m_uiBytePos); length=m_uiWP;
			m_uiBytePos+=m_uiWP-2;
			break;
		case DRI: m_ucRestartMarkers=1;
			WordP(m_uiBytePos); length=m_uiWP; //should be = 4
			WordP(m_uiBytePos);  m_uiMCURestart=m_uiWP;
			if (m_uiMCURestart==0) m_ucRestartMarkers=0;
			break;
		case SOF: WordP(m_uiBytePos); length=m_uiWP; //should be = 8+3*3=17
			ByteP(m_uiBytePos); precision=m_ucBP;
			if (precision!=8) exit_func("LoadHeader","Only 8 bit precision supported");
			WordP(m_uiBytePos); *puiYimage=m_uiWP; WordP(m_uiBytePos); *puiXimage=m_uiWP;

			if(m_bBuffersAllocated == false) {
				m_pImBuffer=(BYTE*)malloc((*puiXimage)*(*puiYimage)*4);
				m_bBuffersAllocated = true;
			}

			ByteP(m_uiBytePos); nr_components=m_ucBP;
			if (nr_components!=3) exit_func("LoadHeader","Only truecolor JPGS supported");
			for (j=1;j<=3;j++)
			{
				ByteP(m_uiBytePos); comp_id=m_ucBP;
				if ((comp_id==0)||(comp_id>3)) exit_func("LoadHeader","Only YCbCr format supported");
				switch (comp_id)
				{
				case 1: // Y
					ByteP(m_uiBytePos); m_ucYH=m_ucBP>>4;m_ucYV=m_ucBP&0xF;
					ByteP(m_uiBytePos); m_ucYQNr=m_ucBP;
					break;
				case 2: // Cb
					ByteP(m_uiBytePos); m_ucCbH=m_ucBP>>4;m_ucCbV=m_ucBP&0xF;
					ByteP(m_uiBytePos); m_ucCbQNr=m_ucBP;
					break;
				case 3: // Cr
					ByteP(m_uiBytePos); m_ucCrH=m_ucBP>>4;m_ucCrV=m_ucBP&0xF;
					ByteP(m_uiBytePos); m_ucCrQNr=m_ucBP;
					break;
				}
			}
			SOF_found=1;
			break;
		case SOS: WordP(m_uiBytePos); length=m_uiWP; //should be = 6+3*2=12
			ByteP(m_uiBytePos); nr_components=m_ucBP;
			if (nr_components!=3) exit_func("LoadHeader","Invalid SOS marker");
			for (j=1;j<=3;j++)
			{
				ByteP(m_uiBytePos); comp_id=m_ucBP;
				if ((comp_id==0)||(comp_id>3)) exit_func("LoadHeader","Only YCbCr format supported");
				switch (comp_id)
				{
				case 1: // Y
					ByteP(m_uiBytePos); m_ucYDCNr=m_ucBP>>4;m_ucYACNr=m_ucBP&0xF;
					break;
				case 2: // Cb
					ByteP(m_uiBytePos); m_ucCbDCNr=m_ucBP>>4;m_ucCbACNr=m_ucBP&0xF;
					break;
				case 3: // Cr
					ByteP(m_uiBytePos); m_ucCrDCNr=m_ucBP>>4;m_ucCrACNr=m_ucBP&0xF;
					break;
				}
			}
			ByteP(m_uiBytePos); ByteP(m_uiBytePos); ByteP(m_uiBytePos); // Skip 3 bytes
			SOS_found=1;
			break;
		case 0xFF:
			break; // do nothing for 0xFFFF, sequence of consecutive 0xFF are for
			// filling purposes and should be ignored
		default:  WordP(m_uiBytePos); length=m_uiWP;
			m_uiBytePos+=m_uiWP-2; //skip unknown marker
			break;
		}
	}
	if (!SOS_found) exit_func("LoadHeader","Invalid JPG file. No SOS marker found.");
	if (!SOF_found) exit_func("LoadHeader","Progressive JPEGs not supported");

	if ((m_ucCbH>m_ucYH)||(m_ucCrH>m_ucYH)) 
		exit_func("LoadHeader","Vertical sampling factor for Y should be >= sampling factor for Cb,Cr");
	if ((m_ucCbV>m_ucYV)||(m_ucCrV>m_ucYV)) 
		exit_func("LoadHeader","Horizontal sampling factor for Y should be >= sampling factor for Cb,Cr");

	if ((m_ucCbH>=2)||(m_ucCbV>=2)) exit_func("LoadHeader","Cb sampling factors should be = 1");
	if ((m_ucCrV>=2)||(m_ucCrV>=2)) exit_func("LoadHeader","Cr sampling factors should be = 1");

	// Restricting sampling factors for Y,Cb,Cr should give us 4 
	// possible cases for sampling factors
	// YHxYV,CbHxCbV,CrHxCrV: 2x2,1x1,1x1;  1x2,1x1,1x1; 2x1,1x1,1x1;
	// and 1x1,1x1,1x1 = no upsampling needed

	m_uiHmax=m_ucYH,m_uiVmax=m_ucYV;
	if (*puiXimage%(m_uiHmax*8)==0) m_uiXRound=*puiXimage; // m_uiXRound = Multiple of m_uiHmax*8
	else m_uiXRound=(*puiXimage/(m_uiHmax*8)+1)*(m_uiHmax*8);
	if (*puiYimage%(m_uiVmax*8)==0) m_uiYRound=*puiYimage; // m_uiYRound = Multiple of m_uiVmax*8
	else m_uiYRound=(*puiYimage/(m_uiVmax*8)+1)*(m_uiVmax*8);
}

void JpgDecoder::GetBuffer(WORD uiXimage, WORD uiYimage, BYTE** ppAddressDestBuffer)
{
	WORD y;
	DWORD dest_loc=0;
	BYTE* src_buffer=m_pImBuffer;
	BYTE* dest_buffer_start, *dest_buffer;

	DWORD src_bytes_per_line=m_uiXRound*4;
	DWORD dest_bytes_per_line=uiXimage*4;

	if ((m_uiXRound==uiXimage)&&(m_uiYRound==uiYimage))
		*ppAddressDestBuffer=m_pImBuffer;
	else
	{
		dest_buffer_start = (BYTE*)malloc(uiXimage*uiYimage*4);
		if (dest_buffer_start==NULL) 
			exit_func("GetBuffer","Not enough memory for storing the JPEG image");
		dest_buffer = dest_buffer_start;
		for (y=0;y<uiYimage;y++) {
			memcpy(dest_buffer,src_buffer,dest_bytes_per_line);
			src_buffer+=src_bytes_per_line;
			dest_buffer+=dest_bytes_per_line;
		}
		*ppAddressDestBuffer=dest_buffer_start;
		//free(m_pImBuffer);
	}
	// release the buffer which contains the JPG file
	//free(m_pJPEGbuf);
}

void JpgDecoder::Decode()
{
	WORD x_mcu_cnt,y_mcu_cnt;
	DWORD nr_mcu;
	WORD X_MCU_nr,Y_MCU_nr; 
	DWORD MCU_dim_x; 
	DWORD im_loc_inc; 
	DWORD im_loc; 

	m_uiBytePos-=2;
	Resync();

	m_uiYincValue = 32*m_uiXRound;
	CalculateTabs(); 

	MCU_dim_x=m_uiHmax*8*4;

	Y_MCU_nr=m_uiYRound/(m_uiVmax*8); // nr of MCUs on Y axis
	X_MCU_nr=m_uiXRound/(m_uiHmax*8); // nr of MCUs on X axis

	m_uiXimageBytes=m_uiXRound*4; im_loc_inc = (m_uiVmax*8-1) * m_uiXimageBytes;
	nr_mcu=0; im_loc=0; // memory location of the current MCU
	for (y_mcu_cnt=0;y_mcu_cnt<Y_MCU_nr;y_mcu_cnt++)
	{
		for (x_mcu_cnt=0;x_mcu_cnt<X_MCU_nr;x_mcu_cnt++)
		{
			if ((m_ucYH==1)&&(m_ucYV==1)) 
				DecodeMCU1x1(im_loc);
			else 
			{
				if (m_ucYH==2)
				{
					if (m_ucYV==2) 
						DecodeMCU2x2(im_loc);
					else 
						DecodeMCU2x1(im_loc);
				}
				else 
					DecodeMCU1x2(im_loc);
			}

			if ((m_ucRestartMarkers)&&((nr_mcu+1)%m_uiMCURestart==0)) Resync();
			nr_mcu++;
			im_loc+=MCU_dim_x;
		}
		im_loc+=im_loc_inc;
	}
}

void JpgDecoder::SkipKbits(BYTE ucK)
{
	BYTE b_high,b_low;
	m_ucDK+=ucK;
	if (m_ucDK>=16) { m_ucDK-=16;
	m_uiW1=m_uiW2;
	// Get the next word in m_uiW2
	ByteP(m_uiBytePos);
	if (m_ucBP!=0xFF) b_high=m_ucBP;
	else 
	{
		if (m_pJPEGbuf[m_uiBytePos]==0) m_uiBytePos++; //skip 00
		else m_uiBytePos--; // stop m_uiBytePos pe restart marker
		b_high=0xFF;
	}
	ByteP(m_uiBytePos);
	if (m_ucBP!=0xFF) b_low=m_ucBP;
	else 
	{
		if (m_pJPEGbuf[m_uiBytePos]==0) m_uiBytePos++; //skip 00
		else m_uiBytePos--; // stop m_uiBytePos pe restart marker
		b_low=0xFF;
	}
	m_uiW2=WordHiLo(b_high,b_low);
	}

	m_uiWordval = ((DWORD)(m_uiW1)<<16) + m_uiW2;
	m_uiWordval <<= m_ucDK;
	m_uiWordval >>= 16;
}

SWORD JpgDecoder::GetKbits(BYTE ucK)
{
	SWORD signed_wordvalue;
	signed_wordvalue=get_svalue(ucK);
	SkipKbits(ucK);
	return signed_wordvalue;
}

void JpgDecoder::CalculateMask()
{
	BYTE k;
	DWORD tmpdv;
	for (k=0;k<=16;k++) 
	{ 
		tmpdv=0x10000;
		g_uiMask[k]=(tmpdv>>k)-1; //precalculated bit mask
	} 
}

void JpgDecoder::InitQT()
{
	BYTE i;
	for (i=0;i<=3;i++) 
		m_pfQT[i]=(float*)malloc(sizeof(float)*64);
}

void JpgDecoder::LoadQuantTable(float* pfQuantTable)
{
	float scalefactor[8]={1.0f, 1.387039845f, 1.306562965f, 1.175875602f,
		1.0f, 0.785694958f, 0.541196100f, 0.275899379f};
	BYTE j,row,col;
	// Load quantization coefficients from JPG file, scale them for DCT and reorder
	// from zig-zag order
	for (j=0;j<=63;j++) 
		pfQuantTable[j]=m_pJPEGbuf[m_uiBytePos+zigzag[j]];
	j=0;
	for (row=0;row<=7;row++)
		for (col=0;col<=7;col++) 
		{
			pfQuantTable[j]*=scalefactor[row]*scalefactor[col];
			j++;
		}
		m_uiBytePos+=64;
}

void JpgDecoder::LoadHuffmanTable(HuffmanTable* pkHT)
{
	BYTE k,j;
	DWORD code;

	for (j=1;j<=16;j++) {
		ByteP(m_uiBytePos);
		pkHT->m_ucLength[j]=m_ucBP;
	}
	for (k=1;k<=16;k++)
		for (j=0;j<pkHT->m_ucLength[k];j++) {
			ByteP(m_uiBytePos);
			pkHT->m_ucV[WordHiLo(k,j)]=m_ucBP;
		}

		code=0;
		for (k=1;k<=16;k++) {
			pkHT->m_uiMinorCode[k] = (WORD)code;
			for (j=1;j<=pkHT->m_ucLength[k];j++) code++;
			pkHT->m_uiMajorCode[k]=(WORD)(code-1);
			code*=2;
			if (pkHT->m_ucLength[k]==0) {
				pkHT->m_uiMinorCode[k]=0xFFFF;
				pkHT->m_uiMajorCode[k]=0;
			}
		}
}

void JpgDecoder::ProcessHuffmanDataUnit(BYTE ucDCNr, BYTE ucACNr, SWORD* pPreviousDC)
{
	// Process one data unit. A data unit = 64 DCT coefficients
	// Data is decompressed by Huffman decoding, then the array is dezigzag-ed
	// The result is a 64 DCT coefficients array: m_aiDCTCoeff
	BYTE nr,k,j,EOB_found;
	register WORD tmp_Hcode;
	BYTE size_val,count_0;
	WORD* min_code,*maj_code;	// min_code[k]=minimum code of length k, 
								// maj_code[k]=similar but maximum
	WORD* max_val, *min_val;
	BYTE* huff_values;
	SWORD DCT_tcoeff[64];
	BYTE byte_temp;

	// Start Huffman decoding
	// First the DC coefficient decoding
	min_code = s_kHTDC[ucDCNr].m_uiMinorCode;
	maj_code = s_kHTDC[ucDCNr].m_uiMajorCode;
	huff_values = s_kHTDC[ucDCNr].m_ucV;

	for (nr = 0; nr < 64 ; nr++) 
		DCT_tcoeff[nr] = 0; //Initialize DCT_tcoeff

	nr=0;// DC coefficient

	min_val = &min_code[1]; max_val = &maj_code[1];
	for (k=1;k<=16;k++) 
	{
		tmp_Hcode = LookKbits(k);
		//	   max_val = &maj_code[k]; min_val = &min_code[k];
		if ( (tmp_Hcode<=*max_val)&&(tmp_Hcode>=*min_val) ) 
		{
			//Found a valid Huffman code
			SkipKbits(k);
			size_val=huff_values[WordHiLo(k,(BYTE)(tmp_Hcode-*min_val))];
			if (size_val==0) 
				DCT_tcoeff[0]=*pPreviousDC;
			else 
			{
				DCT_tcoeff[0]=*pPreviousDC+GetKbits(size_val);
				*pPreviousDC=DCT_tcoeff[0];
			}
			break;
		}
		min_val++; max_val++;
	}

	// Second, AC coefficient decoding
	min_code = s_kHTAC[ucACNr].m_uiMinorCode;
	maj_code = s_kHTAC[ucACNr].m_uiMajorCode;
	huff_values = s_kHTAC[ucACNr].m_ucV;

	nr=1; // AC coefficient
	EOB_found=0;
	while ( (nr<=63)&&(!EOB_found) )
	{
		max_val = &maj_code[1]; min_val =&min_code[1];
		for (k=1;k<=16;k++)
		{
			tmp_Hcode=LookKbits(k);
			//	   max_val = &maj_code[k]; &min_val = min_code[k];
			if ( (tmp_Hcode<=*max_val)&&(tmp_Hcode>=*min_val) )
			{
				SkipKbits(k);
				byte_temp=huff_values[WordHiLo(k,(BYTE)(tmp_Hcode-*min_val))];
				size_val=byte_temp&0xF;
				count_0=byte_temp>>4;
				if (size_val==0) 
				{
					if (count_0==0) 
						EOB_found=1;
					else 
						if (count_0==0xF) 
							nr+=16; //skip 16 zeroes
				}
				else
				{
					nr+=count_0; //skip count_0 zeroes
					DCT_tcoeff[nr++]=GetKbits(size_val);
				}
				break;
			}
			min_val++; max_val++;
		}
		if (k>16) nr++;  // This should not occur
	}

	for (j=0;j<=63;j++) 
		m_aiDCTCoeff[j]=DCT_tcoeff[zigzag[j]]; // voila ... m_aiDCTCoeff
}

// Fast float IDCT transform
void JpgDecoder::IDCTTransform(SWORD* piIncoeff, BYTE* pucOutcoeff, BYTE ucQNr)
{
	BYTE x;
	SBYTE y;
	SWORD* inptr;
	BYTE* outptr;
	float workspace[64];
	float* wsptr;//Workspace pointer
	float* quantptr; // Quantization table pointer
	float dcval;
	float tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7;
	float tmp10,tmp11,tmp12,tmp13;
	float z5,z10,z11,z12,z13;
	BYTE* range_limit=s_pucRlimitTable+128;
	// Pass 1: process COLUMNS from input and store into work array.
	wsptr=workspace;
	inptr=piIncoeff;
	quantptr=m_pfQT[ucQNr];
	for (y=0;y<=7;y++)
	{
		if( (inptr[8]|inptr[16]|inptr[24]|inptr[32]|inptr[40]|inptr[48]|inptr[56])==0)
		{
			// AC terms all zero (in a column)
			dcval=inptr[0]*quantptr[0];
			wsptr[0]  = dcval;
			wsptr[8]  = dcval;
			wsptr[16] = dcval;
			wsptr[24] = dcval;
			wsptr[32] = dcval;
			wsptr[40] = dcval;
			wsptr[48] = dcval;
			wsptr[56] = dcval;
			inptr++;quantptr++;wsptr++;//advance pointers to next column
			continue ;
		}
		//Even part
		tmp0 = inptr[0] *quantptr[0];
		tmp1 = inptr[16]*quantptr[16];
		tmp2 = inptr[32]*quantptr[32];
		tmp3 = inptr[48]*quantptr[48];

		tmp10 = tmp0 + tmp2;// phase 3
		tmp11 = tmp0 - tmp2;

		tmp13 = tmp1 + tmp3;// phases 5-3
		tmp12 = (tmp1 - tmp3) * 1.414213562f - tmp13; // 2*c4

		tmp0 = tmp10 + tmp13;// phase 2
		tmp3 = tmp10 - tmp13;
		tmp1 = tmp11 + tmp12;
		tmp2 = tmp11 - tmp12;

		// Odd part
		tmp4 = inptr[8] *quantptr[8];
		tmp5 = inptr[24]*quantptr[24];
		tmp6 = inptr[40]*quantptr[40];
		tmp7 = inptr[56]*quantptr[56];

		z13 = tmp6 + tmp5;// phase 6
		z10 = tmp6 - tmp5;
		z11 = tmp4 + tmp7;
		z12 = tmp4 - tmp7;

		tmp7 = z11 + z13;// phase 5
		tmp11= (z11 - z13) * 1.414213562f; // 2*c4

		z5 = (z10 + z12) * 1.847759065f; // 2*c2
		tmp10 = 1.082392200f * z12 - z5; // 2*(c2-c6)
		tmp12 = -2.613125930f * z10 + z5;// -2*(c2+c6)

		tmp6 = tmp12 - tmp7;// phase 2
		tmp5 = tmp11 - tmp6;
		tmp4 = tmp10 + tmp5;

		wsptr[0]  = tmp0 + tmp7;
		wsptr[56] = tmp0 - tmp7;
		wsptr[8]  = tmp1 + tmp6;
		wsptr[48] = tmp1 - tmp6;
		wsptr[16] = tmp2 + tmp5;
		wsptr[40] = tmp2 - tmp5;
		wsptr[32] = tmp3 + tmp4;
		wsptr[24] = tmp3 - tmp4;
		inptr++;
		quantptr++;
		wsptr++;//advance pointers to the next column
	}

	// Pass 2: process ROWS from work array, store into output array.
	// Note that we must descale the results by a factor of 8 = 2^3
	outptr=pucOutcoeff;
	wsptr=workspace;
	for (x=0;x<=7;x++)
	{
		// Even part
		tmp10 = wsptr[0] + wsptr[4];
		tmp11 = wsptr[0] - wsptr[4];

		tmp13 = wsptr[2] + wsptr[6];
		tmp12 =(wsptr[2] - wsptr[6]) * 1.414213562f - tmp13;

		tmp0 = tmp10 + tmp13;
		tmp3 = tmp10 - tmp13;
		tmp1 = tmp11 + tmp12;
		tmp2 = tmp11 - tmp12;

		// Odd part
		z13 = wsptr[5] + wsptr[3];
		z10 = wsptr[5] - wsptr[3];
		z11 = wsptr[1] + wsptr[7];
		z12 = wsptr[1] - wsptr[7];

		tmp7 = z11 + z13;
		tmp11= (z11 - z13) * 1.414213562f;

		z5 = (z10 + z12) * 1.847759065f; // 2*c2
		tmp10 = 1.082392200f * z12 - z5;  // 2*(c2-c6)
		tmp12 = -2.613125930f * z10 + z5; // -2*(c2+c6)

		tmp6 = tmp12 - tmp7;
		tmp5 = tmp11 - tmp6;
		tmp4 = tmp10 + tmp5;

		// Final output stage: scale down by a factor of 8
		outptr[0] = range_limit[(DESCALE((int) (tmp0 + tmp7), 3)) & 1023L];
		outptr[7] = range_limit[(DESCALE((int) (tmp0 - tmp7), 3)) & 1023L];
		outptr[1] = range_limit[(DESCALE((int) (tmp1 + tmp6), 3)) & 1023L];
		outptr[6] = range_limit[(DESCALE((int) (tmp1 - tmp6), 3)) & 1023L];
		outptr[2] = range_limit[(DESCALE((int) (tmp2 + tmp5), 3)) & 1023L];
		outptr[5] = range_limit[(DESCALE((int) (tmp2 - tmp5), 3)) & 1023L];
		outptr[4] = range_limit[(DESCALE((int) (tmp3 + tmp4), 3)) & 1023L];
		outptr[3] = range_limit[(DESCALE((int) (tmp3 - tmp4), 3)) & 1023L];

		wsptr+=8;//advance pointer to the next row
		outptr+=8;
	}
}

void JpgDecoder::PrecalculateCrCbTables()
{
	WORD k;
	WORD Cr_v,Cb_v;
	for (k=0;k<=255;k++) g_aiCrTab[k]=(SWORD)((k-128.0)*1.402);
	for (k=0;k<=255;k++) g_aiCbTab[k]=(SWORD)((k-128.0)*1.772);

	for (Cr_v=0;Cr_v<=255;Cr_v++)
		for (Cb_v=0;Cb_v<=255;Cb_v++)
			g_aiCrCbGreenTab[((WORD)(Cr_v)<<8)+Cb_v]=
				(int)(-0.34414*(Cb_v-128.0)-0.71414*(Cr_v-128.0));

}

void JpgDecoder::Convert8x8YCbCrToRGB(BYTE* pucY, BYTE* pCb, BYTE* pCr, DWORD im_loc, 
									  DWORD uiXimageBytes, BYTE* pImBuffer)
{
	DWORD x,y;
	BYTE im_nr;
	BYTE* Y_val = pucY, *Cb_val = pCb, *Cr_val = pCr;
	BYTE* ibuffer = pImBuffer + im_loc;

	for (y=0;y<8;y++)
	{
		im_nr=0;
		for (x=0;x<8;x++)
		{
			ibuffer[im_nr++] = s_pucRlimitTable[*Y_val + g_aiCbTab[*Cb_val]]; //B
			ibuffer[im_nr++] = s_pucRlimitTable[*Y_val + g_aiCrCbGreenTab[WordHiLo(*Cr_val,*Cb_val)]]; //G
			ibuffer[im_nr++] = s_pucRlimitTable[*Y_val + g_aiCrTab[*Cr_val]]; // R
			/*
			// Monochrome display
			pImBuffer[im_nr++] = *Y_val;
			pImBuffer[im_nr++] = *Y_val;
			pImBuffer[im_nr++] = *Y_val;
			*/
			Y_val++; Cb_val++; Cr_val++; im_nr++;
		}
		ibuffer+=uiXimageBytes;
	}
}

void JpgDecoder::Convert8x8YCbCrToRGBTab(BYTE* pY, BYTE* pCb, BYTE* pCr, 
										 BYTE* tab, DWORD im_loc, 
										 DWORD uiXimageBytes, BYTE* pImBuffer)
{
	DWORD x,y;
	BYTE nr, im_nr;
	BYTE Y_val,Cb_val,Cr_val;
	BYTE* ibuffer = pImBuffer + im_loc;

	nr=0;
	for (y=0;y<8;y++)
	{
		im_nr=0;
		for (x=0;x<8;x++)
		{
			Y_val=pY[nr];
			Cb_val=pCb[tab[nr]]; Cr_val=pCr[tab[nr]]; // reindexare folosind tabelul
			ibuffer[im_nr++] = s_pucRlimitTable[Y_val + g_aiCbTab[Cb_val]]; //B
			ibuffer[im_nr++] = s_pucRlimitTable[Y_val + 
				g_aiCrCbGreenTab[WordHiLo(Cr_val,Cb_val)]]; //G
			ibuffer[im_nr++] = s_pucRlimitTable[Y_val + g_aiCrTab[Cr_val]]; // R
			nr++; im_nr++;
		}
		ibuffer+=uiXimageBytes;
	}
}

void JpgDecoder::CalculateTabs()
{
	BYTE tab_temp[256];
	BYTE x,y;

	for (y=0;y<16;y++)
		for (x=0;x<16;x++)
			tab_temp[y*16+x] = (y/m_ucYV)* 8 + x/m_ucYH;

	for (y=0;y<8;y++)
	{
		for (x=0;x<8;x++)
			m_aucTab1[y*8+x]=tab_temp[y*16+x];
		for (x=8;x<16;x++)
			m_aucTab2[y*8+(x-8)]=tab_temp[y*16+x];
	}
	for (y=8;y<16;y++)
	{
		for (x=0;x<8;x++)
			m_aucTab3[(y-8)*8+x]=tab_temp[y*16+x];
		for (x=8;x<16;x++)
			m_aucTab4[(y-8)*8+(x-8)]=tab_temp[y*16+x];
	}
}

// Allocate and fill in the sample_range_limit table
void JpgDecoder::prepare_range_limit_table()
{
	int j;
	s_pucRlimitTable = (BYTE*)malloc(5 * 256L + 128) ;
	// First segment of "simple" table: limit[x] = 0 for x < 0
	memset((void*)s_pucRlimitTable,0,256);
	s_pucRlimitTable += 256;	// allow negative subscripts of simple table
	// Main part of "simple" table: limit[x] = x
	for (j = 0; j < 256; j++) s_pucRlimitTable[j] = j;
	// End of simple table, rest of first half of post-IDCT table
	for (j = 256; j < 640; j++) s_pucRlimitTable[j] = 255;
	// Second half of post-IDCT table
	memset((void*)(s_pucRlimitTable + 640),0,384);
	for (j = 0; j < 128 ; j++) s_pucRlimitTable[j+1024] = j;
}

void JpgDecoder::InitJPGDecoding()
{
	m_uiBytePos=0;
	InitQT();
	CalculateMask();
	prepare_range_limit_table();
	PrecalculateCrCbTables();
}

// m_uiBytePos  = pozitionat pe restart marker
void JpgDecoder::Resync()
{
	m_uiBytePos+=2;
	ByteP(m_uiBytePos);
	if (m_ucBP==0xFF) m_uiBytePos++; // skip 00
	m_uiW1=WordHiLo(m_ucBP, 0);
	ByteP(m_uiBytePos);
	if (m_ucBP==0xFF) m_uiBytePos++; // skip 00
	m_uiW1+=m_ucBP;
	ByteP(m_uiBytePos);
	if (m_ucBP==0xFF) m_uiBytePos++; // skip 00
	m_uiW2=WordHiLo(m_ucBP, 0);
	ByteP(m_uiBytePos);
	if (m_ucBP==0xFF) m_uiBytePos++; // skip 00
	m_uiW2+=m_ucBP;
	m_uiWordval=m_uiW1; m_ucDK=0; // Reinit bitstream decoding
	m_iDCY=0; m_iDCCb=0; m_iDCCr=0; // Init DC coefficients
}

void JpgDecoder::DecodeMCU1x1(DWORD uiImLoc)
{
	// Y
	ProcessHuffmanDataUnit(m_ucYDCNr,m_ucYACNr,&m_iDCY);
	IDCTTransform(m_aiDCTCoeff,m_aucY,m_ucYQNr);
	// Cb
	ProcessHuffmanDataUnit(m_ucCbDCNr,m_ucCbACNr,&m_iDCCb);
	IDCTTransform(m_aiDCTCoeff,m_aucCb,m_ucCbQNr);
	// Cr
	ProcessHuffmanDataUnit(m_ucCrDCNr,m_ucCrACNr,&m_iDCCr);
	IDCTTransform(m_aiDCTCoeff,m_aucCr,m_ucCrQNr);

	Convert8x8YCbCrToRGB(m_aucY,m_aucCb,m_aucCr,uiImLoc,m_uiXimageBytes,m_pImBuffer);
}

void JpgDecoder::DecodeMCU2x1(DWORD uiImLoc)
{
	// Y
	ProcessHuffmanDataUnit(m_ucYDCNr,m_ucYACNr,&m_iDCY);
	IDCTTransform(m_aiDCTCoeff,m_aucY1,m_ucYQNr);
	ProcessHuffmanDataUnit(m_ucYDCNr,m_ucYACNr,&m_iDCY);
	IDCTTransform(m_aiDCTCoeff,m_aucY2,m_ucYQNr);
	// Cb
	ProcessHuffmanDataUnit(m_ucCbDCNr,m_ucCbACNr,&m_iDCCb);
	IDCTTransform(m_aiDCTCoeff,m_aucCb,m_ucCbQNr);
	// Cr
	ProcessHuffmanDataUnit(m_ucCrDCNr,m_ucCrACNr,&m_iDCCr);
	IDCTTransform(m_aiDCTCoeff,m_aucCr,m_ucCrQNr);

	Convert8x8YCbCrToRGBTab(m_aucY1,m_aucCb,m_aucCr,m_aucTab1,uiImLoc,m_uiXimageBytes,m_pImBuffer);
	Convert8x8YCbCrToRGBTab(m_aucY2,m_aucCb,m_aucCr,m_aucTab2,uiImLoc+32,m_uiXimageBytes,m_pImBuffer);
}

void JpgDecoder::DecodeMCU2x2(DWORD uiImLoc)
{
	// Y
	ProcessHuffmanDataUnit(m_ucYDCNr,m_ucYACNr,&m_iDCY);
	IDCTTransform(m_aiDCTCoeff,m_aucY1,m_ucYQNr);
	ProcessHuffmanDataUnit(m_ucYDCNr,m_ucYACNr,&m_iDCY);
	IDCTTransform(m_aiDCTCoeff,m_aucY2,m_ucYQNr);
	ProcessHuffmanDataUnit(m_ucYDCNr,m_ucYACNr,&m_iDCY);
	IDCTTransform(m_aiDCTCoeff,m_aucY3,m_ucYQNr);
	ProcessHuffmanDataUnit(m_ucYDCNr,m_ucYACNr,&m_iDCY);
	IDCTTransform(m_aiDCTCoeff,m_aucY4,m_ucYQNr);
	// Cb
	ProcessHuffmanDataUnit(m_ucCbDCNr,m_ucCbACNr,&m_iDCCb);
	IDCTTransform(m_aiDCTCoeff,m_aucCb,m_ucCbQNr);
	// Cr
	ProcessHuffmanDataUnit(m_ucCrDCNr,m_ucCrACNr,&m_iDCCr);
	IDCTTransform(m_aiDCTCoeff,m_aucCr,m_ucCrQNr);

	Convert8x8YCbCrToRGBTab(m_aucY1,m_aucCb,m_aucCr,m_aucTab1,uiImLoc,m_uiXimageBytes,m_pImBuffer);
	Convert8x8YCbCrToRGBTab(m_aucY2,m_aucCb,m_aucCr,m_aucTab2,uiImLoc+32,m_uiXimageBytes,m_pImBuffer);
	Convert8x8YCbCrToRGBTab(m_aucY3,m_aucCb,m_aucCr,m_aucTab3,uiImLoc+m_uiYincValue,m_uiXimageBytes,m_pImBuffer);
	Convert8x8YCbCrToRGBTab(m_aucY4,m_aucCb,m_aucCr,m_aucTab4,uiImLoc+m_uiYincValue+32,m_uiXimageBytes,m_pImBuffer);
}

void JpgDecoder::DecodeMCU1x2(DWORD uiImLoc)
{
	// Y
	ProcessHuffmanDataUnit(m_ucYDCNr,m_ucYACNr,&m_iDCY);
	IDCTTransform(m_aiDCTCoeff,m_aucY1,m_ucYQNr);
	ProcessHuffmanDataUnit(m_ucYDCNr,m_ucYACNr,&m_iDCY);
	IDCTTransform(m_aiDCTCoeff,m_aucY2,m_ucYQNr);
	// Cb
	ProcessHuffmanDataUnit(m_ucCbDCNr,m_ucCbACNr,&m_iDCCb);
	IDCTTransform(m_aiDCTCoeff,m_aucCb,m_ucCbQNr);
	// Cr
	ProcessHuffmanDataUnit(m_ucCrDCNr,m_ucCrACNr,&m_iDCCr);
	IDCTTransform(m_aiDCTCoeff,m_aucCr,m_ucCrQNr);

	Convert8x8YCbCrToRGBTab(m_aucY1,m_aucCb,m_aucCr,m_aucTab1,uiImLoc,m_uiXimageBytes,m_pImBuffer);
	Convert8x8YCbCrToRGBTab(m_aucY2,m_aucCb,m_aucCr,m_aucTab3,uiImLoc+m_uiYincValue,m_uiXimageBytes,m_pImBuffer);
}