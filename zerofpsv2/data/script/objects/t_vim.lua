function Create()
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/fwarrior.mad");
			InitParameter("m_fScale","0.165");
		InitProperty("P_Event");
		InitProperty("P_Ml");
		SetParentObject();
		SetReturnObject();
end

function Init()
	AddAction(GetSelfID(),"Open");
	AddAction(GetSelfID(),"Close");

	PlayAnim(GetSelfID(), "run");
end

function Use(action)
	Print("Got Action:",action);

	if action == "Open" then
		PlayAnim(GetSelfID(), "open");
		SetNextAnim(GetSelfID(), "opened");
	end
	
	if action == "Close" then
		PlayAnim(GetSelfID(), "close");
		SetNextAnim(GetSelfID(), "closed");
	end

end
