#include <iostream>
#include "pmd.h"
#include "3ds.h"

using namespace std;

void rgb_c::print(void)
{
	cout << "<" << r << "," <<
		g << "," << 
		b << ">";
}

material_c::material_c()
{
	strcpy(Name, "");
	strcpy(Texture.File_name, "");
}

material_c::material_c(const material_c& mat)
{
	Ambient_color  = mat.Ambient_color;
	Diffuse_color  = mat.Diffuse_color;
	Specular_color = mat.Specular_color;
	
	transparency = mat.transparency;
	strcpy(Name,mat.Name);
	Texture = mat.Texture;
}

void material_c::print(void)
{
	cout << " Name: " << Name << " : ";
	Ambient_color.print();
	Diffuse_color.print();
	Specular_color.print();
	cout << endl,
	cout << "   " << Texture.File_name << endl;

}

facemat_c::facemat_c()
{
	strcpy(Name,"");
	num_of_faces = 0;
	faces = NULL;
}

facemat_c::~facemat_c()
{
	if(faces)
		delete [] faces;
}

facemat_c::facemat_c(const facemat_c& kOther)
{
	strcpy(Name,kOther.Name);
	num_of_faces = kOther.num_of_faces;
	faces = new int [kOther.num_of_faces];
	for(int i=0; i<kOther.num_of_faces; i++)
	{
		faces[i] = kOther.faces[i];
	}
}

facemat_c& facemat_c::operator = (const facemat_c& kOther )	// Copy
{
	if(faces)
		delete [] faces;

	strcpy(Name,kOther.Name);
	num_of_faces = kOther.num_of_faces;
	faces = new int [kOther.num_of_faces];
	for(int i=0; i<kOther.num_of_faces; i++)
	{
		faces[i] = kOther.faces[i];
	}

	return *this;
}

void facemat_c::Clear(void)
{
	if(faces)
		delete [] faces;
}





/*
material_c& material_c::operator = ( material_c& s )
{
	return *this;

}*/

char*	ChunkFindName(int id );


void PrintUnknownChunkInfo(int id, char* parentname)
{
	char* chname = ChunkFindName(id);
	printf("Unknown Chunk (%x / %s) - Parent (%s)\n", id, chname, parentname);
}

void PrintChunkInfo(int id, int lengd)
{
	char* chname = ChunkFindName(id);
	printf("Read Chunk (%x / %s) - size %d)\n", id, chname, lengd);
}


// BASIC READ FUNCTIONS ///////////////////////////////////////////////////
int ReadByte(FILE *fp, unsigned char &value)
{
	return (fread(&value, sizeof(value),1,fp) == 1);
}

int ReadShort(FILE *fp, unsigned short &value)
{
	return (fread(&value, sizeof(value),1,fp) == 1);
}

int ReadLong(FILE *fp, long &value)
{
	return (fread(&value, sizeof(value),1,fp) == 1);
}

int ReadULong(FILE *fp, unsigned long &value)
{
	return (fread(&value, sizeof(value),1,fp) == 1);
}

short ReadFloat(FILE* fp, float& value)
{
	return (fread(&value, sizeof(value), 1, fp) == 1);
}


void Read3DSString(FILE *fp, char *name, int len)
{
	int c;

	for(int i=0; (c =fgetc(fp)) != EOF && c != 0; i++) {
		if(i < len) name[i]=c;
		}

	if(i <len) name[i]=0;
		else name[len-1]=0;
}

// Read chunk header
int Modell3DS::Read3DSChunk(FILE *fp, Chunk3DS &chunk)
{
	if(!ReadShort(fp, chunk.id))	return false;
	if(!ReadLong(fp, chunk.len))	return false;

	int chunkid = chunk.id;
	int lengd = chunk.len;
//	PrintChunkInfo(chunkid, lengd);
	return true;
}

// Reads a chunk of vertex. Returns num_vertxex.
int Modell3DS::Chunk_CHUNK3DS_POINT_ARRAY(FILE *fp, part_c *part)
{
	unsigned short num_vertex;
	ReadShort(fp, num_vertex);

	part->vertex = new Vector3 [num_vertex];

	for(int i = 0; i<num_vertex; i++) {
		ReadFloat(fp, part->vertex[i].x);
		ReadFloat(fp, part->vertex[i].y);
		ReadFloat(fp, part->vertex[i].z);
		}

	return num_vertex;
}

int Modell3DS::Chunk_CHUNK3DS_TEX_VERTS(FILE *fp, part_c *part)
{
	unsigned short num_vertex;
	ReadShort(fp, num_vertex);

	part->mapcoo = new Vector3 [num_vertex];
	
	for(int i = 0; i<num_vertex; i++) {
		ReadFloat(fp, part->mapcoo[i].x);
		ReadFloat(fp, part->mapcoo[i].y);
		}

	return num_vertex;
}

int Modell3DS::Chunk_CHUNK3DS_FACE_ARRAY(FILE *fp, long FileStart, long FileLen, long FileSize, part_c *part)
{
	unsigned short num_faces, evis;
	ReadShort(fp, num_faces);

	part->surface = new surface_s [num_faces];
	part->texture = new int [num_faces];


	for(int i = 0; i<num_faces; i++) {
		ReadShort(fp, part->surface[i].index[0]);
		ReadShort(fp, part->surface[i].index[1]);
		ReadShort(fp, part->surface[i].index[2]);
		ReadShort(fp, evis);
		}

	long		ChunkStart = ftell(fp);
	Chunk3DS	chunk;
	int			chunkid;
	facemat_c	FaceMat;
	unsigned short value;

	while( ChunkStart < (FileStart+FileLen) &&  Read3DSChunk(fp, chunk)) {
		switch(chunk.id) {
			// Chunk exist but we dont handle them.
			case CHUNK3DS_MSH_MAT_GROUP:
				FaceMat.Clear();
				Read3DSString(fp, FaceMat.Name, 255);
				ReadShort(fp,value);
				FaceMat.num_of_faces = value;
				FaceMat.faces = new int [FaceMat.num_of_faces];
				for(i=0; i<FaceMat.num_of_faces; i++)
				{
					ReadShort(fp,value);
					FaceMat.faces[i] = value;
				}


				cout << "Found MAT group: " << FaceMat.Name << ", - " << FaceMat.num_of_faces << endl;
				part3DS.MaterialGroups.push_back(FaceMat);

				break;

			default:
				// Skip unknown chunk's
				chunkid = chunk.id;
				PrintUnknownChunkInfo(chunkid,"Chunk_CHUNK3DS_FACE_ARRAY");
			}

		fseek(fp, ChunkStart +chunk.len, SEEK_SET);
		ChunkStart = ftell(fp);
		}


	return num_faces;
}




void Modell3DS::ReadTRIObject(FILE *fp, long FileStart, long FileLen, long FileSize)
{
	long		ChunkStart = ftell(fp);
	Chunk3DS	chunk;
	int			chunkid;

	int num_vertex, num_faces,num_vertcoo;
	part3DS.clear();

	while( ChunkStart < (FileStart+FileLen) &&  Read3DSChunk(fp, chunk)) {
		switch(chunk.id) {
		
			case CHUNK3DS_POINT_ARRAY:	num_vertex  = Chunk_CHUNK3DS_POINT_ARRAY(fp, &part3DS);	break;
			case CHUNK3DS_FACE_ARRAY:	
				num_faces   = Chunk_CHUNK3DS_FACE_ARRAY(fp, ChunkStart, chunk.len, FileLen, &part3DS);	
				break;
			case CHUNK3DS_TEX_VERTS:	num_vertcoo = Chunk_CHUNK3DS_TEX_VERTS(fp, &part3DS);	break;

			// Chunk exist but we dont handle them.
			case CHUNK3DS_MESH_MATRIX:
				break;

			default:
				// Skip unknown chunk's
				chunkid = chunk.id;
				PrintUnknownChunkInfo(chunkid,"CHUNK3DS_N_TRI_OBJECT");
				//printf("Chunk: %x\n", chunkid);
			}

		fseek(fp, ChunkStart +chunk.len, SEEK_SET);
		ChunkStart = ftell(fp);
		}

	if(num_vertex == 0 || num_faces == 0)	return;

	part3DS.num_vertex = num_vertex;
	part3DS.num_surfaces = num_faces;
}

void Modell3DS::ReadNamedObject(FILE *fp, long FileStart, long FileLen, long FileSize)
{
	long	ChunkStart;
	Chunk3DS chunk;
	char GName[255];
	int chunkid;

	Read3DSString(fp, GName, 255);
	ChunkStart = ftell(fp);

	while( ChunkStart < (FileStart+FileLen) &&  Read3DSChunk(fp, chunk)) {
		switch(chunk.id) {
			case CHUNK3DS_N_TRI_OBJECT:
				ReadTRIObject(fp, ChunkStart, chunk.len,FileLen);
				break;

			default:
				// Skip unknown chunk's
				chunkid = chunk.id;
				PrintUnknownChunkInfo(chunkid,"CHUNK3DS_NAMED_OBJECT");
				//printf("Chunk: %x\n", chunkid);
			}

		fseek(fp, ChunkStart +chunk.len, SEEK_SET);
		ChunkStart = ftell(fp);
		}
}


rgb_c Modell3DS::Read_Color(FILE *fp, long FileStart, long FileLen, long FileSize)
{
	rgb_c	resultat_color;
	unsigned char	r,g,b;

	long	ChunkStart = ftell(fp);
	Chunk3DS chunk;
	int chunkid;

	while( ChunkStart < (FileStart+FileLen) &&  Read3DSChunk(fp, chunk)) {
		switch(chunk.id) {
			case CHUNK3DS_COLOR_F:
				ReadFloat(fp, resultat_color.r);
				ReadFloat(fp, resultat_color.g);
				ReadFloat(fp, resultat_color.b);
				break;

			case CHUNK3DS_LIN_COLOR_F:
				ReadFloat(fp, resultat_color.r);
				ReadFloat(fp, resultat_color.g);
				ReadFloat(fp, resultat_color.b);
				break;

			case CHUNK3DS_COLOR_24:
				ReadByte(fp, r);
				ReadByte(fp, g);
				ReadByte(fp, b);
				resultat_color.r = r;
				resultat_color.g = g;
				resultat_color.b = b;
				break;

			case CHUNK3DS_LIN_COLOR_24:
				ReadByte(fp, r);
				ReadByte(fp, g);
				ReadByte(fp, b);
				resultat_color.r = r;
				resultat_color.g = g;
				resultat_color.b = b;
				break;

			default:
				// Skip unknown chunk's
				chunkid = chunk.id;
				PrintUnknownChunkInfo(chunkid,"Read_Color");
				//printf("Chunk: %x\n", chunkid);
			}

			fseek(fp, ChunkStart +chunk.len, SEEK_SET);
			ChunkStart = ftell(fp);
		}

	return resultat_color;
}

texture_s Modell3DS::Read_Map(FILE *fp, long FileStart, long FileLen, long FileSize)
{
	long	ChunkStart = ftell(fp);
	Chunk3DS chunk;
	int chunkid;

	texture_s Tex;

	while( ChunkStart < (FileStart+FileLen) &&  Read3DSChunk(fp, chunk)) {
		switch(chunk.id) {
			case CHUNK3DS_MATERIAL_MAPNAME:
				Read3DSString(fp, Tex.File_name, 255);
				cout << "Found Map: " << Tex.File_name << endl;
				break;

			case CHUNK3DS_MATERIAL_MAPVSCALE:	ReadFloat(fp, Tex.V_scale);			break;
			case CHUNK3DS_MATERIAL_MAPUSCALE:	ReadFloat(fp, Tex.U_scale);			break;
			case CHUNK3DS_MATERIAL_MAPVOFFS:	ReadFloat(fp, Tex.V_offset);		break;
			case CHUNK3DS_MATERIAL_MAPUOFFS:	ReadFloat(fp, Tex.U_offset);		break;

			case CHUNK3DS_MATERIAL_MAP_PARAMETERS:
			case CHUNK3DS_MATERIAL_BLUR_PROCENT:
				break;

			default:
				// Skip unknown chunk's
				chunkid = chunk.id;
				PrintUnknownChunkInfo(chunkid,"Read_Map");
				//printf("Chunk: %x\n", chunkid);
			}

			fseek(fp, ChunkStart +chunk.len, SEEK_SET);
			ChunkStart = ftell(fp);
		}

	return Tex;
}


material_c Modell3DS::Read_MATERIAL_EDITOR(FILE *fp, long FileStart, long FileLen, long FileSize)
{
	long	ChunkStart = ftell(fp);
	Chunk3DS chunk;
	int chunkid;

	material_c Mat;

	while( ChunkStart < (FileStart+FileLen) &&  Read3DSChunk(fp, chunk)) {
		switch(chunk.id) {
			case CHUNK3DS_MATERIAL_NAME:
				Read3DSString(fp, Mat.Name, 255);
				cout << "Found material: " << Mat.Name << endl;
				break;

			case CHUNK3DS_MATERIAL_AMBIENT:
				// ChunkStart, chunk.len,FileSize
				Mat.Ambient_color = Read_Color(fp,ChunkStart, chunk.len,FileLen);
				break;

			case CHUNK3DS_MATERIAL_DIFUSSE:
				Mat.Diffuse_color = Read_Color(fp,ChunkStart, chunk.len,FileLen);
				break;

			case CHUNK3DS_MATERIAL_SPECULAR:
				Mat.Specular_color = Read_Color(fp,ChunkStart, chunk.len,FileLen);
				break;

			case CHUNK3DS_MATERIAL_MAP_TEXTURE1:
				Mat.Texture = Read_Map(fp,ChunkStart,chunk.len,FileLen);
				break;

			// Chunk exist but we dont handle them.
			case CHUNK3DS_MATERIAL_SHININESS:
			case CHUNK3DS_MATERIAL_SHININESS_STR:
			case CHUNK3DS_MATERIAL_TRANSPARENCY:
			case CHUNK3DS_MATERIAL_TRANSPARENCY_FALLOFF:
			case CHUNK3DS_MATERIAL_REFLICTION:
			case CHUNK3DS_MATERIAL_RENDERTYPE:
			case CHUNK3DS_MATERIAL_SELFILLUM:
			case CHUNK3DS_MATERIAL_INTRANC:
			case CHUNK3DS_MATERIAL_SOFTEN:
			case CHUNK3DS_MATERIAL_WIRETICKNESS:
				break;

			default:
				// Skip unknown chunk's
				chunkid = chunk.id;
				PrintUnknownChunkInfo(chunkid,"CHUNK3DS_MATERIAL_EDITOR");
			}

			fseek(fp, ChunkStart +chunk.len, SEEK_SET);
			ChunkStart = ftell(fp);
		}

	return Mat;
}


void Modell3DS::ReadMDATA(FILE *fp, long FileStart, long FileLen, long FileSize)
{
	long	ChunkStart = ftell(fp);
	Chunk3DS chunk;
	int chunkid;
	material_c newmat;

	material_c ostus(newmat);
	
	while( ChunkStart < (FileStart+FileLen) &&  Read3DSChunk(fp, chunk)) {
		switch(chunk.id) {
			case CHUNK3DS_NAMED_OBJECT:
				ReadNamedObject(fp, ChunkStart, chunk.len, FileLen);
				break;

			case CHUNK3DS_MATERIAL_EDITOR:
				newmat = Read_MATERIAL_EDITOR(fp, ChunkStart, chunk.len, FileLen);
				part3DS.Material.push_back(newmat);
				break;

			// Chunk exist but we dont handle them.
			case CHUNK3DS_MASTER_SCALE:
			case CHUNK3DS_MESH_VERSION:
				break;

			default:
				// Skip unknown chunk's
				chunkid = chunk.id;
				PrintUnknownChunkInfo(chunkid,"CHUNK3DS_MMAGIC");
				//printf("Chunk: %x\n", chunkid);
			}

			fseek(fp, ChunkStart +chunk.len, SEEK_SET);
			ChunkStart = ftell(fp);
		}
}



void Modell3DS::ReadM3DChunk(FILE *fp,long FileStart, long FileLen, long FileSize)
{
	long ChunkStart = ftell(fp);
	Chunk3DS chunk;
	int chunkid;

	unsigned long version;

	while( ChunkStart < (FileStart+FileLen) &&  Read3DSChunk(fp, chunk)) {
		switch(chunk.id) {
			case CHUNK3DS_MMAGIC:
				ReadMDATA(fp, ChunkStart, chunk.len, FileLen);
				break;

			case CHUNK3DS_VERSION:
				ReadULong(fp,version);
				cout << "3DS Version num " << version << endl;
				break;

			default:
				// Skip unknown chunk's
				chunkid = chunk.id;
				PrintUnknownChunkInfo(chunkid,"CHUNK3DS_MAIN3DS");
				}

			fseek(fp, ChunkStart +chunk.len, SEEK_SET);
			ChunkStart = ftell(fp);
		}
}

/*
int Modell3DS::MatNameToIndex(pmd_c* pmd, char* name)
{
	vector<material_c>::iterator mi;
	int i = 0;
	for (mi = part3DS.Material.begin(); mi != part3DS.Material.end(); mi++, i++)
	{
		if(strcmp(name, mi->Name) == 0)
		{
			return i;
		}
	}

	/*for(int i=0; i<pmd->head.num_of_textures; i++)
	{
		if(strcmp(name, pmd->textures[i].texname) == 0)
		{
			return i;
		}
	}

	cout << "MAteral NOT FOUND \n" << endl;
	return 0;
}*/

bool Modell3DS::Export(MadExporter* mad, const char* filename)
//void transfer_to_part(pmd_c *to)
{
	int i;

	/*
	int		iVersionNum;				// Version num.
	int		iNumOfTextures;				// Num of textures used by mesh.
	int		iNumOfSubMeshes;			// Num of texture coo.
	int		iNumOfAnimation;			// Antal Animationer.
	*/

	mad->kHead.iNumOfVertex		= part3DS.num_vertex;
	mad->kHead.iNumOfFaces		= part3DS.num_surfaces;
	mad->kHead.iNumOfFrames		= 1;
	mad->kHead.iNumOfVertex		= part3DS.num_vertex;
	mad->kHead.iNumOfTextures	= part3DS.Material.size();

	mad->akFrames.resize(1);

	// Copy texture names
	vector<material_c>::iterator mi;
	i = 0;
	for (mi = part3DS.Material.begin(); mi != part3DS.Material.end(); mi++, i++)
	{
		strcpy(mad->akTextures[i].ucTextureName, mi->Texture.File_name);
		mi->print();
		char* extpos = strchr(mad->akTextures[i].ucTextureName,'.');
		if(extpos)
			extpos[0] = 0;
	}

	mad->akFrames[0].akVertex.resize(mad->kHead.iNumOfVertex);
	mad->akTextureCoo.resize(mad->kHead.iNumOfVertex);

	for(i=0; i<mad->kHead.iNumOfVertex; i++) {
		mad->akFrames[0].akVertex[i].x = part3DS.vertex[i].x;
		mad->akFrames[0].akVertex[i].y = part3DS.vertex[i].y;
		mad->akFrames[0].akVertex[i].z = part3DS.vertex[i].z;
		}

	if(part3DS.mapcoo)	{
		for(i=0; i<part3DS.num_vertex; i++) {
			mad->akTextureCoo[i].s = part3DS.mapcoo[i].x;
			mad->akTextureCoo[i].t = part3DS.mapcoo[i].y;
			}
		}
	else
		cout << "No Mapping Coo" << endl;

	mad->akFaces.resize(mad->kHead.iNumOfFaces);
	for(i=0; i<part3DS.num_surfaces; i++) {
		mad->akFaces[i].iIndex[0] = part3DS.surface[i].index[0];
		mad->akFaces[i].iIndex[1] = part3DS.surface[i].index[1];
		mad->akFaces[i].iIndex[2] = part3DS.surface[i].index[2];
		}

/*	vector<facemat_c>::iterator fmi;
	for(fmi = part3DS.MaterialGroups.begin(); fmi != part3DS.MaterialGroups.end(); fmi++)
	{
		int matindex = MatNameToIndex(to, fmi->Name);
		for(i=0; i<fmi->num_of_faces; i++)
		{
			to->triangle[fmi->faces[i]].texture_num = matindex;	
		}
	}*/

	Mad_Animation kNyAnimation;
	Mad_KeyFrame kNyKeyF;
	kNyAnimation.Clear();
	strcpy(kNyAnimation.Name, "freeze"); 
	kNyKeyF.Clear();
	kNyKeyF.iVertexFrame = 0;
	kNyKeyF.fFrameTime = 0.1;
	kNyAnimation.KeyFrame.push_back(kNyKeyF);
	mad->akAnimation.push_back(kNyAnimation);
	
	i=0;
	return 0;
}

void Modell3DS::Read( const char* filename )
{
	FILE *fp;
	long FileSize;
	Chunk3DS chunk;

	// Open 3ds file.
	fp=fopen(filename, "rb");

	if(!fp)	return;

	long ChunkStart = ftell(fp);

	// Get file size
	fseek(fp, 0, SEEK_END);
	FileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// Looap alla chunks
	while(ChunkStart < FileSize && Read3DSChunk(fp, chunk)) {
		switch(chunk.id) {
			case CHUNK3DS_MAIN3DS:
				ReadM3DChunk(fp, ChunkStart, chunk.len,FileSize);
				break;

				}

				fseek(fp, ChunkStart+chunk.len, SEEK_SET);
				ChunkStart = ftell(fp);
			}
 
	fclose(fp);

	return;
}

part_c::part_c()
{
	vertex = NULL;
	surface = NULL;
	mapcoo = NULL;
	num_of_materials = 0;
}

part_c::~part_c()
{
	if(vertex)	delete [] vertex;
	if(surface)	delete [] surface;
	if(mapcoo)	delete [] mapcoo;
}

void part_c::clear(void)
{
	if(vertex)	delete [] vertex;
	vertex = NULL;
	if(surface)	delete [] surface;
	surface = NULL;
	if(mapcoo)	delete [] mapcoo;
	mapcoo = NULL;
	num_of_materials = 0;
}






char*	ChunkFindName(int id )
{
	switch(id) 
	{
		case CHUNK3DS_MAIN3DS:			return "CHUNK3DS_MAIN3DS";			break;
		case CHUNK3DS_MMAGIC:			return "CHUNK3DS_MMAGIC";			break;
		case CHUNK3DS_SMAGIC:			return "CHUNK3DS_SMAGIC";			break;
		case CHUNK3DS_LMAGIC:			return "CHUNK3DS_LMAGIC";			break;
		case CHUNK3DS_MLIBMAGIC:		return "CHUNK3DS_MLIBMAGIC";		break;
		case CHUNK3DS_MATMAGIC:			return "CHUNK3DS_MATMAGIC";			break;
		case CHUNK3DS_MESH_VERSION:		return "CHUNK3DS_MESH_VERSION";		break;
		case CHUNK3DS_VERSION:			return "CHUNK3DS_VERSION";			break;
		case CHUNK3DS_COLOR_F:			return "CHUNK3DS_COLOR_F";			break;
		case CHUNK3DS_COLOR_24:			return "CHUNK3DS_COLOR_24";			break;
		case CHUNK3DS_LIN_COLOR_24:		return "CHUNK3DS_LIN_COLOR_24";		break;
		case CHUNK3DS_LIN_COLOR_F:		return "CHUNK3DS_LIN_COLOR_F";		break;
		case CHUNK3DS_INT_PERCENTAGE:	return "CHUNK3DS_INT_PERCENTAGE";	break;
		case CHUNK3DS_FLOAT_PERCENTAGE:	return "CHUNK3DS_FLOAT_PERCENTAGE";	break;
		case CHUNK3DS_MASTER_SCALE:		return "CHUNK3DS_MASTER_SCALE";		break;
		case CHUNK3DS_BIT_MAP:			return "CHUNK3DS_BIT_MAP";			break;
		case CHUNK3DS_USE_BIT_MAP:		return "CHUNK3DS_USE_BIT_MAP";		break;
		case CHUNK3DS_SOLID_BGND:		return "CHUNK3DS_SOLID_BGND";		break;
		case CHUNK3DS_USE_SOLID_BGND:	return "CHUNK3DS_USE_SOLID_BGND";	break;
		case CHUNK3DS_V_GRADIENT:		return "CHUNK3DS_V_GRADIENT";		break;
		case CHUNK3DS_USE_V_GRADIENT:	return "CHUNK3DS_USE_V_GRADIENT";	break;
		case CHUNK3DS_LO_SHADOW_BIAS:	return "CHUNK3DS_LO_SHADOW_BIAS";	break;
		case CHUNK3DS_HI_SHADOW_BIAS:	return "CHUNK3DS_HI_SHADOW_BIAS";	break;
		case CHUNK3DS_SHADOW_MAP_SIZE:	return "CHUNK3DS_SHADOW_MAP_SIZE";	break;
		case CHUNK3DS_SHADOW_SAMPLES:	return "CHUNK3DS_SHADOW_SAMPLES";	break;
		case CHUNK3DS_SHADOW_RANGE:		return "CHUNK3DS_SHADOW_RANGE";		break;
		case CHUNK3DS_SHADOW_FILTER:	return "CHUNK3DS_SHADOW_FILTER";	break;
		case CHUNK3DS_RAY_BIAS:			return "CHUNK3DS_RAY_BIAS";			break;
		case CHUNK3DS_RAY_SHADOWS:		return "CHUNK3DS_RAY_SHADOWS";		break;
		case CHUNK3DS_O_CONSTS:			return "CHUNK3DS_O_CONSTS";			break;
		case CHUNK3DS_AMBIENT_LIGHT:	return "CHUNK3DS_AMBIENT_LIGHT";	break;
		case CHUNK3DS_FOG:				return "CHUNK3DS_FOG";				break;
		case CHUNK3DS_USE_FOG:			return "CHUNK3DS_USE_FOG";			break;
		case CHUNK3DS_FOG_BGND:			return "CHUNK3DS_FOG_BGND";			break;
		case CHUNK3DS_DISTANCE_CUE:		return "CHUNK3DS_DISTANCE_CUE";		break;
		case CHUNK3DS_USE_DISTANCE_CUE:	return "CHUNK3DS_USE_DISTANCE_CUE";	break;
		case CHUNK3DS_LAYER_FOG:		return "CHUNK3DS_LAYER_FOG";		break;
		case CHUNK3DS_USE_LAYER_FOG:	return "CHUNK3DS_USE_LAYER_FOG";	break;
		case CHUNK3DS_DCUE_BGND:		return "CHUNK3DS_DCUE_BGND";		break;
		case CHUNK3DS_DEFAULT_VIEW:		return "CHUNK3DS_DEFAULT_VIEW";		break;
		case CHUNK3DS_VIEW_TOP:			return "CHUNK3DS_VIEW_TOP";			break;
		case CHUNK3DS_VIEW_BOTTOM:		return "CHUNK3DS_VIEW_BOTTOM";		break;
		case CHUNK3DS_VIEW_LEFT:		return "CHUNK3DS_VIEW_LEFT";		break;
		case CHUNK3DS_VIEW_RIGHT:		return "CHUNK3DS_VIEW_RIGHT";		break;
		case CHUNK3DS_VIEW_FRONT:		return "CHUNK3DS_VIEW_FRONT";		break;
		case CHUNK3DS_VIEW_BACK:		return "CHUNK3DS_VIEW_BACK";		break;
		case CHUNK3DS_VIEW_USER:		return "CHUNK3DS_VIEW_USER";		break;
		case CHUNK3DS_VIEW_CAMERA:		return "CHUNK3DS_VIEW_CAMERA";		break;
		case CHUNK3DS_VIEW_WINDOW:		return "CHUNK3DS_VIEW_WINDOW";		break;
		case CHUNK3DS_NAMED_OBJECT:		return "CHUNK3DS_NAMED_OBJECT";		break;
		case CHUNK3DS_OBJ_HIDDEN:		return "CHUNK3DS_OBJ_HIDDEN";		break;
		case CHUNK3DS_OBJ_VIS_LOFTER:	return "CHUNK3DS_OBJ_VIS_LOFTER";	break;
		case CHUNK3DS_OBJ_DOESNT_CAST:	return "CHUNK3DS_OBJ_DOESNT_CAST";	break;
		case CHUNK3DS_OBJ_MATTE:		return "CHUNK3DS_OBJ_MATTE";		break;
		case CHUNK3DS_OBJ_FAST:			return "CHUNK3DS_OBJ_FAST";			break;
		case CHUNK3DS_OBJ_PROCEDURAL:	return "CHUNK3DS_OBJ_PROCEDURAL";	break;
		case CHUNK3DS_OBJ_FROZEN:		return "CHUNK3DS_OBJ_FROZEN";		break;
		case CHUNK3DS_OBJ_DONT_RCVSHADOW:		return "CHUNK3DS_OBJ_DONT_RCVSHADOW";		break;
		case CHUNK3DS_N_TRI_OBJECT:		return "CHUNK3DS_N_TRI_OBJECT";		break;
		case CHUNK3DS_POINT_ARRAY:		return "CHUNK3DS_POINT_ARRAY";		break;
		case CHUNK3DS_POINT_FLAG_ARRAY:	return "CHUNK3DS_POINT_FLAG_ARRAY";	break;
		case CHUNK3DS_FACE_ARRAY:		return "CHUNK3DS_FACE_ARRAY";		break;
		case CHUNK3DS_MSH_MAT_GROUP:	return "CHUNK3DS_MSH_MAT_GROUP";	break;
		case CHUNK3DS_OLD_MAT_GROUP:	return "CHUNK3DS_OLD_MAT_GROUP";	break;
		case CHUNK3DS_TEX_VERTS:		return "CHUNK3DS_TEX_VERTS";		break;
		case CHUNK3DS_SMOOTH_GROUP:		return "CHUNK3DS_SMOOTH_GROUP";		break;
		case CHUNK3DS_MESH_MATRIX:		return "CHUNK3DS_MESH_MATRIX";		break;
		case CHUNK3DS_MESH_COLOR:		return "CHUNK3DS_MESH_COLOR";		break;
		case CHUNK3DS_MESH_TEXTURE_INFO:		return "CHUNK3DS_MESH_TEXTURE_INFO";		break;
		case CHUNK3DS_PROC_NAME:		return "CHUNK3DS_PROC_NAME";		break;
		case CHUNK3DS_PROC_DATA:		return "CHUNK3DS_PROC_DATA";		break;
		case CHUNK3DS_MSH_BOXMAP:		return "CHUNK3DS_MSH_BOXMAP";		break;
		case CHUNK3DS_N_D_L_OLD:		return "CHUNK3DS_N_D_L_OLD";		break;
		case CHUNK3DS_N_CAM_OLD:		return "CHUNK3DS_N_CAM_OLD";		break;
		case CHUNK3DS_N_DIRECT_LIGHT:	return "CHUNK3DS_N_DIRECT_LIGHT";	break;
		case CHUNK3DS_DL_SPOTLIGHT:		return "CHUNK3DS_DL_SPOTLIGHT";		break;
		case CHUNK3DS_DL_OFF:			return "CHUNK3DS_DL_OFF";			break;
		case CHUNK3DS_DL_ATTENUATE:		return "CHUNK3DS_DL_ATTENUATE";		break;
		case CHUNK3DS_DL_RAYSHAD:		return "CHUNK3DS_DL_RAYSHAD";		break;
		case CHUNK3DS_DL_SHADOWED:		return "CHUNK3DS_DL_SHADOWED";		break;
		case CHUNK3DS_DL_LOCAL_SHADOW:	return "CHUNK3DS_DL_LOCAL_SHADOW";	break;
		case CHUNK3DS_DL_LOCAL_SHADOW2:	return "CHUNK3DS_DL_LOCAL_SHADOW2";	break;
		case CHUNK3DS_DL_SEE_CONE:		return "CHUNK3DS_DL_SEE_CONE";		break;
		case CHUNK3DS_DL_SPOT_RECTANGULAR:		return "CHUNK3DS_DL_SPOT_RECTANGULAR";		break;
		case CHUNK3DS_DL_SPOT_OVERSHOOT:		return "CHUNK3DS_DL_SPOT_OVERSHOOT";		break;
		case CHUNK3DS_DL_SPOT_PROJECTOR:		return "CHUNK3DS_DL_SPOT_PROJECTOR";		break;
		case CHUNK3DS_DL_EXCLUDE:		return "CHUNK3DS_DL_EXCLUDE";		break;
		case CHUNK3DS_DL_RANGE:			return "CHUNK3DS_DL_RANGE";			break;
		case CHUNK3DS_DL_SPOT_ROLL:		return "CHUNK3DS_DL_SPOT_ROLL";		break;
		case CHUNK3DS_DL_SPOT_ASPECT:	return "CHUNK3DS_DL_SPOT_ASPECT";	break;
		case CHUNK3DS_DL_RAY_BIAS:		return "CHUNK3DS_DL_RAY_BIAS";		break;
		case CHUNK3DS_DL_INNER_RANGE:	return "CHUNK3DS_DL_INNER_RANGE";	break;
		case CHUNK3DS_DL_OUTER_RANGE:	return "CHUNK3DS_DL_OUTER_RANGE";	break;
		case CHUNK3DS_DL_MULTIPLIER:	return "CHUNK3DS_DL_MULTIPLIER";	break;
		case CHUNK3DS_N_AMBIENT_LIGHT:	return "CHUNK3DS_N_AMBIENT_LIGHT";	break;
		case CHUNK3DS_N_CAMERA:			return "CHUNK3DS_N_CAMERA";			break;
		case CHUNK3DS_CAM_SEE_CONE:		return "CHUNK3DS_CAM_SEE_CONE";		break;
		case CHUNK3DS_CAM_RANGES:		return "CHUNK3DS_CAM_RANGES";		break;
		case CHUNK3DS_HIERARCHY:		return "CHUNK3DS_HIERARCHY";		break;
		case CHUNK3DS_PARENT_OBJECT:	return "CHUNK3DS_PARENT_OBJECT";	break;
		case CHUNK3DS_PIVOT_OBJECT:		return "CHUNK3DS_PIVOT_OBJECT";		break;
		case CHUNK3DS_PIVOT_LIMITS:		return "CHUNK3DS_PIVOT_LIMITS";		break;
		case CHUNK3DS_PIVOT_ORDER:		return "CHUNK3DS_PIVOT_ORDER";		break;
		case CHUNK3DS_XLATE_RANGE:		return "CHUNK3DS_XLATE_RANGE";		break;
		case CHUNK3DS_POLY_2D:			return "CHUNK3DS_POLY_2D";			break;
		case CHUNK3DS_MATERIAL_EDITOR:	return "CHUNK3DS_MATERIAL_EDITOR";	break;
		case CHUNK3DS_MATERIAL_NAME:	return "CHUNK3DS_MATERIAL_NAME";	break;
		case CHUNK3DS_MATERIAL_AMBIENT:	return "CHUNK3DS_MATERIAL_AMBIENT";	break;
		case CHUNK3DS_MATERIAL_DIFUSSE:	return "CHUNK3DS_MATERIAL_DIFUSSE";	break;
		case CHUNK3DS_MATERIAL_SPECULAR:		return "CHUNK3DS_MATERIAL_SPECULAR";		break;
		case CHUNK3DS_MATERIAL_MAPNAME:	return "CHUNK3DS_MATERIAL_MAPNAME";	break;
		case CHUNK3DS_MATERIAL_MAPVSCALE:		return "CHUNK3DS_MATERIAL_MAPVSCALE";		break;
		case CHUNK3DS_MATERIAL_MAPUSCALE:		return "CHUNK3DS_MATERIAL_MAPUSCALE";		break;
		case CHUNK3DS_MATERIAL_MAPVOFFS:		return "CHUNK3DS_MATERIAL_MAPVOFFS";		break;
		case CHUNK3DS_MATERIAL_MAPUOFFS:		return "CHUNK3DS_MATERIAL_MAPUOFFS";		break;
		case CHUNK3DS_KEYF_AMBIENT:		return "CHUNK3DS_KEYF_AMBIENT";		break;
		case CHUNK3DS_KEYF_MESH:		return "CHUNK3DS_KEYF_MESH";		break;
		case CHUNK3DS_KEYF_CAMERA:		return "CHUNK3DS_KEYF_CAMERA";		break;
		case CHUNK3DS_KEYF_CAMERA_INFO:		return "CHUNK3DS_KEYF_CAMERA_INFO";		break;
		case CHUNK3DS_KEYF_OMNI_LIGT:		return "CHUNK3DS_KEYF_OMNI_LIGT";		break;
		case CHUNK3DS_KEYF_SPOT_LIGHT_TARGET:		return "CHUNK3DS_KEYF_SPOT_LIGHT_TARGET";		break;
		case CHUNK3DS_KEYF_SPOT_LIGHT:		return "CHUNK3DS_KEYF_SPOT_LIGHT";		break;
		case CHUNK3DS_KEYF_FRAMES:		return "CHUNK3DS_KEYF_FRAMES";		break;
		case CHUNK3DS_KEYF_OBJECT_NAME:		return "CHUNK3DS_KEYF_OBJECT_NAME";		break;
		case CHUNK3DS_KEYF_OBJECT_PIVOT:		return "CHUNK3DS_KEYF_OBJECT_PIVOT";		break;
		case CHUNK3DS_KEYF_OBJECT_MORPH:		return "CHUNK3DS_KEYF_OBJECT_MORPH";		break;
		case CHUNK3DS_KEYF_TRACK_POSITION:		return "CHUNK3DS_KEYF_TRACK_POSITION";		break;
		case CHUNK3DS_KEYF_TRACK_ROTATION:		return "CHUNK3DS_KEYF_TRACK_ROTATION";		break;
		case CHUNK3DS_KEYF_TRACK_SCALE:		return "CHUNK3DS_KEYF_TRACK_SCALE";		break;
		case CHUNK3DS_KEYF_TRACK_FOV:		return "CHUNK3DS_KEYF_TRACK_FOV";		break;
		case CHUNK3DS_KEYF_TRACK_ROLL:		return "CHUNK3DS_KEYF_TRACK_ROLL";		break;
		case CHUNK3DS_KEYF_TRACK_COLOR:		return "CHUNK3DS_KEYF_TRACK_COLOR";		break;
		case CHUNK3DS_KEYF_TRACK_MORPH:		return "CHUNK3DS_KEYF_TRACK_MORPH";		break;
		case CHUNK3DS_KEYF_TRACK_HOTSPOTH:		return "CHUNK3DS_KEYF_TRACK_HOTSPOTH";		break;
		case CHUNK3DS_KEYF_TRACK_FALLOFF:		return "CHUNK3DS_KEYF_TRACK_FALLOFF";		break;
		case CHUNK3DS_KEYF_TRACK_HIDE:		return "CHUNK3DS_KEYF_TRACK_HIDE";		break;
		case CHUNK3DS_KEYF_TRACK_HIERARCHY:		return "CHUNK3DS_KEYF_TRACK_HIERARCHY";		break;
		case CHUNK3DS_KEYF:		return "CHUNK3DS_KEYF";		break;
		case CHUNK3DS_MATERIAL_SHININESS:		return "CHUNK3DS_MATERIAL_SHININESS";		break;
		case CHUNK3DS_MATERIAL_SHININESS_STR:		return "CHUNK3DS_MATERIAL_SHININESS_STR";		break;
		case CHUNK3DS_MATERIAL_TRANSPARENCY:		return "CHUNK3DS_MATERIAL_TRANSPARENCY";		break;
		case CHUNK3DS_MATERIAL_TRANSPARENCY_FALLOFF:		return "CHUNK3DS_MATERIAL_TRANSPARENCY_FALLOFF";		break;
		case CHUNK3DS_MATERIAL_REFLICTION:		return "CHUNK3DS_MATERIAL_REFLICTION";		break;
		case CHUNK3DS_MATERIAL_RENDERTYPE:		return "CHUNK3DS_MATERIAL_RENDERTYPE";		break;
		case CHUNK3DS_MATERIAL_TWOSIDED:		return "CHUNK3DS_MATERIAL_TWOSIDED";		break;
		case CHUNK3DS_MATERIAL_ADDTRANS:		return "CHUNK3DS_MATERIAL_ADDTRANS";		break;
		case CHUNK3DS_MATERIAL_SELFILLUM:		return "CHUNK3DS_MATERIAL_SELFILLUM";		break;
		case CHUNK3DS_MATERIAL_WIREFRAMEON:		return "CHUNK3DS_MATERIAL_WIREFRAMEON";		break;
		case CHUNK3DS_MATERIAL_WIRETICKNESS:		return "CHUNK3DS_MATERIAL_WIRETICKNESS";		break;
		case CHUNK3DS_MATERIAL_FACEMAP:		return "CHUNK3DS_MATERIAL_FACEMAP";		break;
		case CHUNK3DS_MATERIAL_INTRANC:		return "CHUNK3DS_MATERIAL_INTRANC";		break;
		case CHUNK3DS_MATERIAL_SOFTEN:		return "CHUNK3DS_MATERIAL_SOFTEN";		break;
		case CHUNK3DS_MATERIAL_WIREINUNITS:		return "CHUNK3DS_MATERIAL_WIREINUNITS";		break;
//		case XXX:		return "XXX";		break;
//		case XXX:		return "XXX";		break;

		case CHUNK3DS_MATERIAL_MAP_TEXTURE1:		return "CHUNK3DS_MATERIAL_MAP_TEXTURE1";		break;
		case CHUNK3DS_MATERIAL_MAP_TEXTURE2:		return "CHUNK3DS_MATERIAL_MAP_TEXTURE2";		break;
		case CHUNK3DS_MATERIAL_MAP_OPACITY:			return "CHUNK3DS_MATERIAL_MAP_OPACITY";		break;
		case CHUNK3DS_MATERIAL_MAP_BUMP:			return "CHUNK3DS_MATERIAL_MAP_BUMP";		break;
		case CHUNK3DS_MATERIAL_MAP_SHININESS:		return "CHUNK3DS_MATERIAL_MAP_SHININESS";		break;
		case CHUNK3DS_MATERIAL_MAP_SPECULAR:		return "CHUNK3DS_MATERIAL_MAP_SPECULAR";		break;
		case CHUNK3DS_MATERIAL_MAP_SELFILLUM:		return "CHUNK3DS_MATERIAL_MAP_SELFILLUM";		break;
		case CHUNK3DS_MATERIAL_MAP_REFLECTION:		return "CHUNK3DS_MATERIAL_MAP_REFLECTION";		break;

		case CHUNK3DS_MATERIAL_MASK_TEXTURE1:		return "CHUNK3DS_MATERIAL_MASK_TEXTURE1";		break;
		case CHUNK3DS_MATERIAL_MASK_TEXTURE2:		return "CHUNK3DS_MATERIAL_MASK_TEXTURE2";		break;
		case CHUNK3DS_MATERIAL_MASK_OPACITY:		return "CHUNK3DS_MATERIAL_MASK_OPACITY";		break;
		case CHUNK3DS_MATERIAL_MASK_BUMP:			return "CHUNK3DS_MATERIAL_MASK_BUMP";		break;
		case CHUNK3DS_MATERIAL_MASK_SHININESS:		return "CHUNK3DS_MATERIAL_MASK_SHININESS";		break;
		case CHUNK3DS_MATERIAL_MASK_SPECULAR:		return "CHUNK3DS_MATERIAL_MASK_SPECULAR";		break;
		case CHUNK3DS_MATERIAL_MASK_SELFILLUM:		return "CHUNK3DS_MATERIAL_MASK_SELFILLUM";		break;
		case CHUNK3DS_MATERIAL_MASK_REFLECTION:		return "CHUNK3DS_MATERIAL_MASK_REFLECTION";		break;

		case CHUNK3DS_MATERIAL_MAP_PARAMETERS:		return "CHUNK3DS_MATERIAL_MAP_PARAMETERS";		break;
		case CHUNK3DS_MATERIAL_BLUR_PROCENT:		return "CHUNK3DS_MATERIAL_BLUR_PROCENT";		break;
		case CHUNK3DS_MATERIAL_ROTATION_ANGLE:		return "CHUNK3DS_MATERIAL_ROTATION_ANGLE";		break;
		case CHUNK3DS_MATERIAL_RGB_LA_TINT1:		return "CHUNK3DS_MATERIAL_RGB_LA_TINT1";		break;
		case CHUNK3DS_MATERIAL_RGB_LA_TINT2:		return "CHUNK3DS_MATERIAL_RGB_LA_TINT2";		break;
		case CHUNK3DS_MATERIAL_RGB_TINT_R:			return "CHUNK3DS_MATERIAL_RGB_TINT_R";		break;
		case CHUNK3DS_MATERIAL_RGB_TINT_G:			return "CHUNK3DS_MATERIAL_RGB_TINT_G";		break;
		case CHUNK3DS_MATERIAL_RGB_TINT_B:			return "CHUNK3DS_MATERIAL_RGB_TINT_B";		break;
			
}

	return "*** --- ***";
}

