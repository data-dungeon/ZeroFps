#ifndef FILE__3DS_H
#define FILE__3DS_H

#define MAX_MATERIALNAME	256
#define MAX_MAPNAME			256

#include "3dsdef.h"

#include <vector>
using namespace std;

class rgb_c
{
public:
	rgb_c() { }
	rgb_c(const rgb_c& c) { r = c.r; g = c.g; b = c.b; }
	rgb_c& operator = ( const rgb_c& c )  { r = c.r; g = c.g; b = c.b; return *this; }

	void print(void);

	float r,g,b;
};

struct texture_s
{
	char	File_name[MAX_MAPNAME];
	float	V_scale, U_scale;
	float	V_offset, U_offset;
	float	Rotation_angle;
};

class material_c
{
public:
	material_c();
	material_c(const material_c& mat);
//	material_c& operator = ( material_c& s );	// Copy

	void print(void);

	char Name[MAX_MATERIALNAME];

	// Colors
	rgb_c	Ambient_color;
	rgb_c	Diffuse_color;
	rgb_c	Specular_color;

	float	transparency;

	texture_s	Texture;
};

struct surface_s 
{
	unsigned short		index[3];
};

class facemat_c
{
public:
	facemat_c();
	~facemat_c();
	facemat_c(const facemat_c& mat);
	facemat_c& operator = ( const facemat_c& s );	// Copy

	void	Clear(void);

	char	Name[MAX_MATERIALNAME];
	int		num_of_faces;
	int*	faces;
};

class part_c {
public:
	int			num_vertex;		
	int			num_surfaces;

	Vector3*	vertex;			// Array with all vertices in object.
	surface_s*	surface;		// Array with all triangles in object.
	Vector3*	mapcoo;			// Texture coo for all vertices.
	int*		texture;			// Texture Index för faces.

	part_c();
	~part_c();
	void clear(void);

	vector<facemat_c>	MaterialGroups;

	vector<material_c>	Material;
	int			num_of_materials;
};


#endif