#include "p_dmshop.h" 

P_DMShop::P_DMShop()
{
	strcpy(m_acName,"P_DMShop");		
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	bNetwork = true;
	m_fTradeRate = 1.0f;
	m_strShopName = "Unnamed shop";

	m_pkItems = NULL;
}

P_DMShop::~P_DMShop()
{
	if(m_pkItems)
		delete m_pkItems;

}

//-----------------------------------------------------------------------------
//
// Init
//
//-----------------------------------------------------------------------------

void P_DMShop::Init()
{
	m_pkObjectMan = 
		static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));

	m_pkItems = new DMContainer(m_pkObjMan,m_pkObject->GetEntityID(),8,10);
}

//-----------------------------------------------------------------------------
//
// GetPropertyValues
//
//-----------------------------------------------------------------------------

vector<PropertyValues> P_DMShop::GetPropertyValues()
{
	vector<PropertyValues> kReturn(2);
		
	kReturn[0].kValueName = "shopname";
	kReturn[0].iValueType = VALUETYPE_STRING;
	kReturn[0].pkValue    = &m_strShopName;		

	kReturn[1].kValueName = "traderate"; // hur snäll affärsägaren är ( 1 < billigt)
	kReturn[1].iValueType = VALUETYPE_FLOAT;
	kReturn[1].pkValue    = &m_fTradeRate;	

	return kReturn;
}

//-----------------------------------------------------------------------------
//
// Save
//
//-----------------------------------------------------------------------------

void P_DMShop::Save(ZFIoInterface* pkPackage)
{	
	char temp[128];

	//saving inventory
	m_pkItems->Save(pkPackage);	

	// saving shopname
	strcpy( temp, m_strShopName.c_str() );
	pkPackage->Write(temp,128,1);

	// saving traderate
	pkPackage->Write ( &m_fTradeRate, sizeof(m_fTradeRate), 1 );

	// saving pricemap size
	int iMapSize = m_kPricemap.size();
	pkPackage->Write ( &iMapSize, sizeof(iMapSize), 1 );

	// saving pricemap data
	map<string, int>::iterator it;
   for ( it = m_kPricemap.begin(); it != m_kPricemap.end(); it++ )
   {
		strcpy( temp, it->first.c_str() ); 
	   pkPackage->Write((void*)&temp,128,1);  // name
	   pkPackage->Write(&it->second, sizeof(it->second), 1 );  // value
	}
	
}

//-----------------------------------------------------------------------------
//
// Load
//
//-----------------------------------------------------------------------------

void P_DMShop::Load(ZFIoInterface* pkPackage)
{
	char temp[128];
	int value;

	//loading inventory
	m_pkItems->Load(pkPackage);		

	// loading shopname
	pkPackage->Read(temp,128,1);
	m_strShopName = temp;

	// loading traderate
	pkPackage->Read ( &m_fTradeRate, sizeof (m_fTradeRate), 1 );

	// loading pricemap size
	int iMapSize;
	pkPackage->Read (  &iMapSize, sizeof (iMapSize), 1 );

	m_kPricemap.clear(); // <- just to be sure, I don't know if it's neceary :)

	// loading pricemap data
   for (int i=0; i<iMapSize; i++)
   {
		pkPackage->Read(temp,128,1);
		pkPackage->Read(&value, sizeof (value), 1 );
		m_kPricemap.insert( map<string, int>::value_type(string(temp), value));
	}
}

//-----------------------------------------------------------------------------
//
// Bestäm vad ett föremål i affären skall kosta.
//
//-----------------------------------------------------------------------------

bool P_DMShop::SetPrice(string strItemName, int iPrice) 
{
	m_kPricemap.insert( map<string, int>::value_type(strItemName, iPrice));
	return true;
}

//-----------------------------------------------------------------------------
//
// Fråga vad ett föremål i affären kostar.
//
//-----------------------------------------------------------------------------

int P_DMShop::GetSellPrice(int iObjectToBuy)
{
	if(iObjectToBuy)
	{
		Entity* pkObject = m_pkObjectMan->GetObjectByNetWorkID(iObjectToBuy);

		P_DMItem* pkItemProperty = (P_DMItem*)pkObject->GetProperty("P_DMItem");

		map<string, int>::iterator itItem;
		itItem = m_kPricemap.find(pkItemProperty->GetName());
		
		if(itItem != m_kPricemap.end())
			return itItem->second;
	}

	return -1;
}

//-----------------------------------------------------------------------------
//
// Fråga affären hur mycket den köper ett speciellt föremål för.
// Affären tittar bland alla sina föremål och kollar om det finns
// ett liknande föremål, ser på värdet på föremålet som skickas in
// och kommer fram till ett pris.
// Returnerar -1 om affären inte vill köpa föremålet
//
//-----------------------------------------------------------------------------

int P_DMShop::GetBuyPrice(int iObjectToSell) 
{
	Entity* pkObjectToAppreciate = m_pkObjectMan->GetObjectByNetWorkID(iObjectToSell);

	if(pkObjectToAppreciate == NULL)
		return -1;

	P_DMItem* pkItemToAppreciate = (P_DMItem*) 
		pkObjectToAppreciate->GetProperty("P_DMItem");

	if(pkItemToAppreciate == NULL)
		return -1;

	vector<ContainerInfo> kItemList;
	m_pkItems->GetItemList(&kItemList);

	// Finns det ett likadant föremål i affären?
	map<string, int>::iterator itItem;
	itItem = m_kPricemap.find(pkItemToAppreciate->GetName());

	// Returnera samma värde * m_fTradeRate
	if(itItem != m_kPricemap.end())
		return (itItem->second * m_fTradeRate);
	
	// Vi köper inga okända varor här.
	return -1;
}

Property* Create_P_DMShop()
{
	return new P_DMShop;
}