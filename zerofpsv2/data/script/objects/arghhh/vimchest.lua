function Create()
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/vimchest.mad");
			InitParameter("m_fScale","0.5");
		InitProperty("P_Event");
		InitProperty("P_Ml");
		SetParentObject();
		SetReturnObject();
end

function Init()
	AddAction(GetSelfID(),"Open");
	AddAction(GetSelfID(),"Close");

	-- Set Correct Open/Close Status
	IsOpen = GetLocalDouble(GetSelfID(), "IsOpen");
	if IsOpen == 0 then
		PlayAnim(GetSelfID(), "closed");
	end
	if IsOpen == 1 then
		PlayAnim(GetSelfID(), "opened");
	end

end

function Use(action)
	Print("Got Action:",action);
 
	IsOpen = GetLocalDouble(GetSelfID(), "IsOpen");

	if action == "Open" then
		if IsOpen == 0 then
			PlayAnim(GetSelfID(), "open");
			SetNextAnim(GetSelfID(), "opened");
			SetLocalDouble(GetSelfID(), "IsOpen", 1);
		end 
	end
	
	if action == "Close" then
		if IsOpen == 1 then
			PlayAnim(GetSelfID(), "close");
			SetNextAnim(GetSelfID(), "closed");
			SetLocalDouble(GetSelfID(), "IsOpen", 0);
		end 
	end

end
