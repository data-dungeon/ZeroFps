#ifndef _ENGINE_MADPROPERTY_H_
#define _ENGINE_MADPROPERTY_H_

#include "../mad/mad_modell.h"
#include "../../engine/zerofps.h"
#include "../../engine/property.h"
#include <string>
#include "../engine_systems_x.h"
using namespace std;

//#define NO_ANIMATION_ID	-1	// Index used for non existing anim.

/// Property to add a Modell (MAD) to a object.
class ENGINE_SYSTEMS_API P_Mad : public Property, public Mad_Modell {
	private:
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );
	
		Render*		m_pkRender;		// Ptr to render sys.
//		ZShader*	m_pkZShader;	// Ptr to shader sys.
		ZeroFps*	m_pkZeroFps;	// Ptr to zerofps sys.
	
		float		m_fLod;
		bool		m_bIsVisible;
		
		//linetest stuff
		Matrix4		m_kModelMatrix;
		Vector3		m_kColPos;
		int			m_iColFace;
		
		bool LineVSSphere(Vector3 &kPos,Vector3 &kDir);
		bool LineVSMesh(Vector3 &kPos,Vector3 &kDir);		
		void GenerateModelMatrix();
		
		bool TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos);		
		bool TestPolygon(Vector3* kVerts,Vector3 kPos,Vector3 kDir);

	public:
		P_Mad();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);
		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );
		void CloneOf(Property* pkProperty) { }
		
		bool	m_bCanBeInvisible;	// True if this Mad fades away if it gets between the cam and the player.

		void SetBase(const char* acName);
		void SetAnimation(char* szName, float fStartTime);
		void SetNextAnimation(char* szName);		
		void Update();

		void SetVisible(bool bVisible);

		Vector3 GetJointPosition(char* szJointName);

		bool TestLine(Vector3 kPos,Vector3 kDir);
		Vector3 GetLastColPos() {return m_kColPos;};
		int GetLastColFace() { return m_iColFace;};
		bool AddMesh(int iSId);
};

Property* Create_MadProperty();


#endif



