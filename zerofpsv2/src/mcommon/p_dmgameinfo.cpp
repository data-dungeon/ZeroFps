#include "p_dmgameinfo.h" 

P_DMGameInfo::P_DMGameInfo()
{
	strcpy(m_acName,"P_DMGameInfo");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;
	
	bNetwork = true;
	
	
	
	m_strClanName = "unnamed";
	m_strClanColor = "blue";
	m_strClanLogo = "";
	
	m_iMoney = 0;
	m_kCameraPos.Set(0,0,0);
	
}

P_DMGameInfo::~P_DMGameInfo()
{


}

void P_DMGameInfo::Init()
{
	cout<<"game info created"<<endl;

}


vector<PropertyValues> P_DMGameInfo::GetPropertyValues()
{
	vector<PropertyValues> kReturn(0);
		

	return kReturn;
}



void P_DMGameInfo::Save(ZFIoInterface* pkPackage)
{	
	char temp[128];
	
	strcpy(temp,m_strClanName.c_str());
	pkPackage->Write(temp,128,1);
	
	strcpy(temp,m_strClanColor.c_str());
	pkPackage->Write(temp,128,1);
	
	strcpy(temp,m_strClanLogo.c_str());
	pkPackage->Write(temp,128,1);
	
	pkPackage->Write(&m_iMoney,sizeof(m_iMoney),1);	
	
	pkPackage->Write(&m_kCameraPos,sizeof(m_kCameraPos),1);	
	
}

void P_DMGameInfo::Load(ZFIoInterface* pkPackage)
{
	char temp[128];

	pkPackage->Read(temp,128,1);
	m_strClanName = temp;

	pkPackage->Read(temp,128,1);
	m_strClanColor = temp;
	
	pkPackage->Read(temp,128,1);
	m_strClanLogo = temp;

	pkPackage->Read(&m_iMoney,sizeof(m_iMoney),1);	

	pkPackage->Read(&m_kCameraPos,sizeof(m_kCameraPos),1);	

}


Property* Create_P_DMGameInfo()
{
	return new P_DMGameInfo;
}