function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/bag.mad");
			InitParameter("m_fScale","1");
		InitProperty("P_Event");
		InitProperty("P_Item");
		InitProperty("P_Ml");
		SetParentObject();
		SetReturnObject();
end

function Init()
	SetContainerSize ("5");
	SetItemName ("Bag");

	AddAction(GetSelfID(),"Look");
	AddAction(GetSelfID(),"PickUp");
	AddAction(GetSelfID(),"Eat");

	SetIcon ("bag.bmp", "bag_a.bmp");
end

	
function Use(action)
	if action == "Look" then
		MessageCaracter(GetCurrentPCID(),"It's a small bag of leather.")
	end

	if action == "PickUp" then
		GetPickedUpBy (GetCurrentPCID());
	end

	if action == "Eat" then
		UseOn (GetCurrentPCID());
		MessageCaracter(GetCurrentPCID(),"The bag was tasty.")
	end

end

