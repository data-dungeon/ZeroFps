function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/armour.mad");			
			InitParameter("m_fScale","1.0");		
		InitProperty("P_DMItem");	
			InitParameter("name","Light armour");
			InitParameter("sizex",3);
			InitParameter("sizey",3);
			InitParameter("icon","armour.bmp");
			InitParameter("type","3");
		InitProperty("P_ScriptInterface");


end

function Init()
	SetItemArmourBonus(SIGetSelfID(), 7);
	SetItemMaxLifeBonus(SIGetSelfID(), 20);
	SetItemSpeedBonus(SIGetSelfID(), -3);
end

function FirstRun()
end

function Use(UserID)
	Heal(UserID);
end
