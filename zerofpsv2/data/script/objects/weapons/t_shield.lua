function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/shield.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
		InitProperty("P_Item");
		InitProperty("P_Ml");
		SetParentObject();
		SetReturnObject();	
end

function Init()
	SetDefenceBonus ("krosskada", 5)
	SetDefenceBonus ("huggskada", 5)
	SetDefenceBonus ("stickskada", 2)

	EquipOn ("lefthand");
	EquipOn ("righthand");
	EquipOn("LeftHand")

	SetEquipmentCategory( "Shield" );

	SetItemName ("Wooden shield");
end
