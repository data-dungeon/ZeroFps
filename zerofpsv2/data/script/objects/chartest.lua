function Create()
	
	InitObject();
		InitProperty("P_Mad");
			InitParameter("m_kMadFile","/data/mad/apple.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
		InitProperty("P_Item");
		InitProperty("P_Ml");
		InitProperty("P_Sound");
		InitProperty("P_Tcs");
			InitParameter("radius",-1);
			InitParameter("gravity", "true");
			InitParameter("character", "true");
			InitParameter("leglength", 0.5);			
			InitParameter("group",1);
			InitParameter("groupflag",4);				
			InitParameter("walkablegroupflag",0);				
			InitParameter("walkablegroupflag",4);							
			
		--InitProperty("P_AmbientSound");
		--	InitParameter("FileName","/data/sound/WoodenPanelClose.wav");
		--	InitParameter("AreaSize", 5);
		--	InitParameter("ManagedByAudioSys", 1);
		
		SetParentObject();
		SetReturnObject();
end

function Init()
	AddAction(GetSelfID(),"Look");
	AddAction(GetSelfID(),"PickUp");
	AddAction(GetSelfID(),"Eat");
	SetItemName ("Apple");
	SetIcon ("apple.bmp", "apple_a.bmp");
end
	
function Use(action)
	if action == "Look" then
		MessageCaracter(GetCurrentPCID(),"A tasty looking apple")
end

	if action == "PickUp" then
		GetPickedUpBy (GetCurrentPCID());
end

	if action == "Eat" then
		UseOn (GetCurrentPCID());
		MessageCaracter(GetCurrentPCID(),"The apple was tasty.")
	end

end
