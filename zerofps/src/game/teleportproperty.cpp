#include "teleportproperty.h"

TeleportProperty::TeleportProperty()
{
	strcpy(m_acName,"TeleportProperty");
	
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	m_pkAlSys=static_cast<OpenAlSystem*>(g_ZFObjSys.GetObjectPtr("OpenAlSystem"));	
	m_pkObjectMan = static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));	
	
	sound=new Sound();
	sound->m_acFile="file:../data/sound/beam.wav";
	sound->m_bLoop=false;
	
	sound2=new Sound();
	sound2->m_acFile="file:../data/sound/beam.wav";
	sound2->m_bLoop=false;

	m_pkStatusProperty = NULL;
	
	m_kToPos.Set(10,10,10);
}

TeleportProperty::~TeleportProperty()
{
	m_pkAlSys->RemoveSound(sound);
	m_pkAlSys->RemoveSound(sound2);	

	delete sound;
	delete sound2;

}


void TeleportProperty::Update()
{
	sound->m_kPos=m_pkObject->GetPos();
	sound2->m_kPos=m_kToPos;
	
	if(m_pkStatusProperty==NULL)
	{
		m_pkStatusProperty=static_cast<StatusProperty*>(m_pkObject->GetProperty("StatusProperty"));
	}
	else
	{
		if(m_pkStatusProperty->m_fHealth <= 0)
			m_pkObjectMan->Delete(m_pkObject);		
	}
}

void TeleportProperty::Touch(Object* pkObject)
{
	cout<<"teleporting :"<<pkObject->GetName()<<endl;

	if(pkObject->GetName()!="HeightMapObject");
	{

		m_pkAlSys->AddSound(sound);
		m_pkAlSys->AddSound(sound2);	
	
		pkObject->GetPos()=m_kToPos;	
	}
}


void TeleportProperty::Save(ZFMemPackage* pkPackage)
{
	pkPackage->Write((void*)&m_kToPos,12);


}

void TeleportProperty::Load(ZFMemPackage* pkPackage)
{
	pkPackage->Read((void*)&m_kToPos,12);

}


Property* Create_TeleportProperty()
{
	return new TeleportProperty;

}











