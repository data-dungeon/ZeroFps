function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/helmet.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
		InitProperty("P_Item");
		InitProperty("P_Ml");
		SetParentObject();
		SetReturnObject();	
end

function Init()
	SetDefenceBonus ("krosskada", 9)
	SetDefenceBonus ("huggskada", 6)
	SetDefenceBonus ("stickskada", 7)

	EquipOn ("head_top");

	SetItemName ("Helmet");
end
