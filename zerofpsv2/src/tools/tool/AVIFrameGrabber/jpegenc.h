#pragma once

#define BYTE unsigned char
#define SBYTE signed char
#define SWORD signed short int
#define WORD unsigned short int
#define DWORD unsigned long int
#define SDWORD signed long int

#define  Y(R,G,B) ((BYTE)( (YRtab[(R)]+YGtab[(G)]+YBtab[(B)])>>16 ) - 128)
#define Cb(R,G,B) ((BYTE)( (CbRtab[(R)]+CbGtab[(G)]+CbBtab[(B)])>>16 ) )
#define Cr(R,G,B) ((BYTE)( (CrRtab[(R)]+CrGtab[(G)]+CrBtab[(B)])>>16 ) )

#define writebyte(b) fputc((b),fp_jpeg_stream)
#define writeword(w) writebyte((w)/256);writebyte((w)%256);

class JpgEncoder
{
public:

	struct colorRGB
	{ 
		BYTE B,G,R; 
	};

	struct bitstring
	{ 
		BYTE length; 
		WORD value; 
	};

	JpgEncoder();
	~JpgEncoder(void);
	void init_all();
	bool SaveData(FILE* pkFile, colorRGB* pkRGBdata, int w, int h);

private:

	static struct DQTinfotype {
		WORD marker;  // = 0xFFDB
		WORD length;  // = 132
		BYTE QTYinfo;// = 0:  bit 0..3: number of QT = 0 (table for Y)
				//       bit 4..7: precision of QT, 0 = 8 bit
		BYTE Ytable[64];
		BYTE QTCbinfo; // = 1 (quantization table for Cb,Cr}
		BYTE Cbtable[64];
		};

	DQTinfotype DQTinfo;

	// Ytable from DQTinfo should be equal to a scaled and zizag reordered version
	// of the table which can be found in "tables.h": std_luminance_qt
	// Cbtable , similar = std_chrominance_qt
	// We'll init them in the program using set_DQTinfo function

	static struct DHTinfotype {
			WORD marker;  // = 0xFFC4
			WORD length;  //0x01A2
			BYTE HTYDCinfo; // bit 0..3: number of HT (0..3), for Y =0
				//bit 4  :type of HT, 0 = DC table,1 = AC table
					//bit 5..7: not used, must be 0
			BYTE YDC_nrcodes[16]; //at index i = nr of codes with length i
			BYTE YDC_values[12];
			BYTE HTYACinfo; // = 0x10
			BYTE YAC_nrcodes[16];
			BYTE YAC_values[162];//we'll use the standard Huffman tables
			BYTE HTCbDCinfo; // = 1
			BYTE CbDC_nrcodes[16];
			BYTE CbDC_values[12];
			BYTE HTCbACinfo; //  = 0x11
			BYTE CbAC_nrcodes[16];
			BYTE CbAC_values[162];
	};

	DHTinfotype DHTinfo;

	static BYTE bytenew; // The byte that will be written in the JPG file
	static SBYTE bytepos; // bit position in the byte we write (bytenew) should be<=7 and >=0
	static WORD mask[16];

	// The Huffman tables we'll use:
	static bitstring YDC_HT[12];
	static bitstring CbDC_HT[12];
	static bitstring YAC_HT[256];
	static bitstring CbAC_HT[256];

	static BYTE *category_alloc;
	static BYTE *category; //Here we'll keep the category of the numbers in range: -32767..32767
	static bitstring *bitcode_alloc;
	static bitstring *bitcode; // their bitcoded representation

	//Precalculated tables for a faster YCbCr->RGB transformation
	// We use a SDWORD table because we'll scale values by 2^16 and work with integers
	static SDWORD YRtab[256],YGtab[256],YBtab[256];
	static SDWORD CbRtab[256],CbGtab[256],CbBtab[256];
	static SDWORD CrRtab[256],CrGtab[256],CrBtab[256];
	static float fdtbl_Y[64];
	static float fdtbl_Cb[64]; //the same with the fdtbl_Cr[64]

	static SBYTE YDU[64]; // This is the Data Unit of Y after YCbCr->RGB transformation
	static SBYTE CbDU[64];
	static SBYTE CrDU[64];
	static SWORD DU_DCT[64]; // Current DU (after DCT and quantization) which we'll zigzag
	static SWORD DU[64]; //zigzag reordered DU which will be Huffman coded

	colorRGB *RGB_buffer; //image to be encoded
	WORD Ximage,Yimage;// image dimensions divisible by 8

	FILE *fp_jpeg_stream;	

	void main_encoder();
	void load_data_units_from_RGB_buffer(WORD xpos,WORD ypos);
	void process_DU(SBYTE *ComponentDU,float *fdtbl,SWORD *DC,bitstring *HTDC,bitstring *HTAC);
	void fdct_and_quantization(SBYTE *data,float *fdtbl,SWORD *outdata);
	void prepare_quant_tables();
	void precalculate_YCbCr_tables();
	void set_numbers_category_and_bitcode();
	void init_Huffman_tables();
	void compute_Huffman_table(BYTE *nrcodes,BYTE *std_table,bitstring *HT);
	void writebits(bitstring bs);
	void write_comment(BYTE *comment);
	void write_SOSinfo();
	void set_DHTinfo();
	void write_DHTinfo();
	void set_DQTinfo();
	void set_quant_table(BYTE *basic_table,BYTE scale_factor,BYTE *newtable);
	void write_DQTinfo();
	void write_SOF0info();
	void write_APP0info();

	void exitmessage(char* msg);

};
