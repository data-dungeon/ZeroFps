function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/meat.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
		InitProperty("P_Item");
		InitProperty("P_Ml");
		
		SetParentObject();
		SetReturnObject();
end

function Init()
	AddAction(GetSelfID(),"Look");
	AddAction(GetSelfID(),"PickUp");
	AddAction(GetSelfID(),"Eat");
	SetIcon ("meat.bmp", "meat_a.bmp");
end

function Use(action)
	if action == "Look" then
		MessageCaracter(GetCurrentPCID(),"A bloody piece of steaked meat.")
	end

	if action == "PickUp" then
		GetPickedUpBy (GetCurrentPCID());
	end

	if action == "Eat" then
		UseOn (GetCurrentPCID());
		MessageCaracter(GetCurrentPCID(),"Yummy!")
	end
end
