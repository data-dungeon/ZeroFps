#ifndef _MADEXP_BONE_
#define _MADEXP_BONE_

#include <maya/MDagPath.h>

class Vector3 {
public:
	double x;
	double y;
	double z;

	void Print(FILE *file) 
	{ 
		fprintf(file, " %f %f %f ",x,y,z); 
	}

	void PrintLf(FILE *file) 
	{ 
		fprintf(file, " %f %f %f \n",x,y,z); 
	}
};

class MadExpBone  
{
public:
	MadExpBone() {};
	virtual ~MadExpBone() {};

	Vector3		m_kTrans;
	Vector3		m_kOrient;

	int			m_iId;
	int			m_iParentId;
	char*		m_ucpName;
	MDagPath	m_kPath;

	void Print(FILE *file)
	{
		fprintf(file, " %d %s %d\n", m_iId, m_ucpName, m_iParentId);
	}

	void PrintPose(FILE *file)
	{
		fprintf(file, " %d", m_iId);
		m_kTrans.Print(file);
		m_kOrient.PrintLf(file);
	}
	
};

#endif 