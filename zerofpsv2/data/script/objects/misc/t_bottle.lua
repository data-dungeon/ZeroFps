function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/bottle.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
		InitProperty("P_Item");
		InitProperty("P_Ml");
		
		SetParentObject();
		SetReturnObject();
end

function Init()
	SetDrawingOrder(5);
	AddAction(GetSelfID(),"Look");
	AddAction(GetSelfID(),"PickUp");
	AddAction(GetSelfID(),"Eat");
	SetIcon ("bottle.bmp", "bottle_a.bmp");
end

	
function Use(action)
	if action == "Look" then
		MessageCaracter(GetCurrentPCID(),"Is is..COKE!")
	end

	if action == "PickUp" then
		GetPickedUpBy (GetCurrentPCID());
	end

	if action == "Eat" then
		UseOn (GetCurrentPCID());
		MessageCaracter(GetCurrentPCID(),"You feel less tired.")
	end
end
