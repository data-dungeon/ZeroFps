function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/pot.mad");
			InitParameter("m_fScale","1");
		InitProperty("P_Event");
		InitProperty("P_Item");
		InitProperty("P_Ml");
		SetParentObject();
		SetReturnObject();
end

function Init()
	SetContainerSize ("7");
	SetItemName ("Pot");

	AddAction(GetSelfID(),"Look");
	AddAction(GetSelfID(),"PickUp");

	SetIcon ("bag.bmp", "bag_a.bmp");
end

	
function Use(action)
	if action == "Look" then
		MessageCaracter(GetCurrentPCID(),"It's a pot made of clay.")
	end

	if action == "PickUp" then
		GetPickedUpBy (GetCurrentPCID());
	end

end

