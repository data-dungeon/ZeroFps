function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/spear.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
		InitProperty("P_Ml");
		InitProperty("P_Item");
		SetParentObject();
		SetReturnObject();	
end

function Init()
--	SetAttackBonus("stickskada", 20);
	UsesSkill ("stickvapen");
	EquipOn ("RightHand");
	EquipOn ("LeftHand");
	EquipOn ("lefthand");
	EquipOn ("righthand");
end

function Use(action)
end