function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/cleaver.mad");
			InitParameter("m_fScale",2.125);
		InitProperty("P_Event");
		InitProperty("P_Item");
		InitProperty("P_Ml");
		SetParentObject();
		SetReturnObject();	
end

function Init()
	UsesSkill ("krossvapen");
	SetAttackBonus ("krossskada", 50);

	EquipOn("righthand")
	Print("Trying to add action PickUp")
	AddAction(GetSelfID(),"PickUp")
end

function Use(action)

	Print(action)

	if action == "PickUp" then
		-- Lägg till detta objekt till inventoryt
		-- Ta bort detta objekt ur världen
		id = GetCurrentPCID()
		Print(id)
		Equip( id, "Hand" )
		RemoveObject(GetSelfID())
	end
end