#ifndef _ENGINE_MADPROPERTY_H_
#define _ENGINE_MADPROPERTY_H_

#include "../mad/mad_modell.h"
#include "../../engine/zerofps.h"
#include "../../engine/property.h"
#include <string>
#include "../engine_systems_x.h"
using namespace std;

#define NO_ANIMATION_ID	-1

/// Property to add a Modell (MAD) to a object.
class ENGINE_SYSTEMS_API MadProperty : public Property, public Mad_Modell {
	private:
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );
	
		float	m_fLod;

		//linetest stuff
		Matrix4	m_kModelMatrix;
		Vector3	m_kColPos;
		int		m_iColFace;
		
		bool LineVSSphere(Vector3 &kPos,Vector3 &kDir);
		bool LineVSMesh(Vector3 &kPos,Vector3 &kDir);		
		void GenerateModelMatrix();
		
		bool TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos);		
		bool TestPolygon(Vector3* kVerts,Vector3 kPos,Vector3 kDir);

	public:
		MadProperty();
		//MadProperty(string strResName);
		void CloneOf(Property* pkProperty) { }
		
		ZeroFps* m_pkZeroFps;

		bool	m_bIsVisible;

		void SetBase(const char* acName);
		void Update();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);


		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );


		bool TestLine(Vector3 kPos,Vector3 kDir);
		Vector3 GetLastColPos() {return m_kColPos;};
		int GetLastColFace() { return m_iColFace;};
};

Property* Create_MadProperty();


/// Property to link a object to a MAD.Joint of the parent.
class ENGINE_SYSTEMS_API LinkToJoint : public Property
{
	private:
		vector<PropertyValues> GetPropertyValues();


	public:
		string	m_strToJoint;

		LinkToJoint();
		~LinkToJoint();
		void Init();

		void CloneOf(Property* pkProperty) { }
		void Update();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);
};

Property* Create_LinkToJoint();


#endif



