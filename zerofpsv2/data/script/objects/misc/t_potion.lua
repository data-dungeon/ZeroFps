function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/potion.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
		InitProperty("P_Ml");
		InitProperty("P_Item");
		
		SetParentObject();
		SetReturnObject();
end

function Init()
	AddAction(GetSelfID(),"Look");
	AddAction(GetSelfID(),"PickUp");
	AddAction(GetSelfID(),"Eat");
	SetIcon ("potion.bmp", "potion_a.bmp");
end

function Use(action)
	if action == "Look" then
		MessageCaracter(GetCurrentPCID(),"A small healing potion.")
	end

	if action == "PickUp" then
		GetPickedUpBy (GetCurrentPCID());
	end

	if action == "Eat" then
		UseOn (GetCurrentPCID());
		MessageCaracter(GetCurrentPCID(),"You feel healthier.")
	end
end
