function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/bread.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
		InitProperty("P_Item");
		InitProperty("P_Ml");
		--		InitProperty("P_Tcs");
--			InitParameter("radius",-1);
--			InitParameter("gravity","true");
		SetParentObject();
		SetReturnObject();
end

function Init()
	AddAction(GetSelfID(),"Look");
	AddAction(GetSelfID(),"PickUp");
	AddAction(GetSelfID(),"Eat");
	SetIcon ("bread.bmp", "bread_a.bmp");
end

function Use(action)
	if action == "Look" then
		MessageCaracter(GetCurrentPCID(),"A hot baked bread!");
	end
	if action == "PickUp" then
		GetPickedUpBy (GetCurrentPCID());
	end
	if action == "Eat" then
		UseOn (GetCurrentPCID());
		MessageCaracter(GetCurrentPCID(),"The break tasted good.");
	end
end
--function Collission()
--	Bounce(GetSelfID());	
--end