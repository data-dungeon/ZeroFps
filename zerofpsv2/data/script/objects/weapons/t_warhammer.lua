function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/warhmr.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
		InitProperty("P_Item");
		InitProperty("P_Ml");
		SetParentObject();
		SetReturnObject();	
end

function Init()
	SetAttackBonus("krossskada", 25);
	UsesSkill ("krossvapen");
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