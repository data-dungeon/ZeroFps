function Create()
	
	InitObject();
		IsStatic();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/candle.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Light");
			InitParameter("Mode",1);
			InitParameter("Quadratic_Atten",0.05);
		InitProperty("P_Event");
		InitProperty("P_Ml");
		InitProperty("P_Item");
		
		SetParentObject();
		SetReturnObject();
end

function Init()
	EquipOn ("LeftHand");
	AddAction(GetSelfID(),"Look");
	AddAction(GetSelfID(),"PickUp");
	SetIcon ("candle.bmp", "candle_a.bmp");
end

function Use(action)
	if action == "Look" then
		MessageCaracter(GetCurrentPCID(),"A small candle.")
	end

	if action == "PickUp" then
		GetPickedUpBy (GetCurrentPCID());
	end
end
