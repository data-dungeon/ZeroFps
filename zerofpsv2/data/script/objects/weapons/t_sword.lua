function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/sword.mad");
			InitParameter("m_fScale",0.1);
		InitProperty("P_Event");
		InitProperty("P_Item");
		InitProperty("P_Ml");
		SetParentObject();
		SetReturnObject();	
end

function Init()
	UsesSkill ("huggvapen");
	SetAttackBonus ("huggskada", 20);

	EquipOn("RightHand")
	AddAction(GetSelfID(),"PickUp")

	SetIcon ("dagger.bmp", "dagger_a.bmp");
	SetEquipmentCategory( "Weapon" );
	EquipOn ("lefthand");
	EquipOn ("righthand");

	SetItemName ("Sword of power");
end

function Use(action)
	if action == "PickUp" then
		GetPickedUpBy (GetCurrentPCID());
	end
end