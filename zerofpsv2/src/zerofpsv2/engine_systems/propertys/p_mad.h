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
class ENGINE_SYSTEMS_API P_Mad : public Property, public Mad_Modell {
	private:
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );
	
		Render* m_pkRender;
		ZShader* m_pkZShader;
	
		float	m_fLod;
		bool	m_bIsVisible;
		
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
		P_Mad();
		//MadProperty(string strResName);
		void CloneOf(Property* pkProperty) { }
		
		ZeroFps* m_pkZeroFps;

		bool	m_bCanBeInvisible;

		void SetBase(const char* acName);
		void SetAnimation(char* szName, float fStartTime);
		void Update();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);


		void SetVisible(bool bVisible);
		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );

		Vector3 GetJointPosition(char* szJointName);

		bool TestLine(Vector3 kPos,Vector3 kDir);
		Vector3 GetLastColPos() {return m_kColPos;};
		int GetLastColFace() { return m_iColFace;};
};

Property* Create_MadProperty();


/// Property to link a object to a MAD.Joint of the parent.
class ENGINE_SYSTEMS_API P_LinkToJoint : public Property
{
	private:
		vector<PropertyValues> GetPropertyValues();


	public:
		string	m_strToJoint;

		P_LinkToJoint();
		~P_LinkToJoint();
		void Init();

		void CloneOf(Property* pkProperty) { }
		void Update();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);
};

Property* Create_LinkToJoint();


#endif



