function Create()
	
	InitObject();
		IsStatic();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/torch.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Light");
			InitParameter("Mode",1);
			InitParameter("Quadratic_Atten",0.01);
		InitProperty("P_AmbientSound");
			InitParameter("FileName","/data/sound/fire.wav");
		InitProperty("P_Event");
		InitProperty("P_Ml");
		
		SetParentObject();
		SetReturnObject();
end

function Init()
	AddAction(GetSelfID(),"Open");
	AddAction(GetSelfID(),"Close");

--	IsLit = GetLocalDouble(GetSelfID(), "IsLit");
--	if IsLit == 0 then
--		PlayAnim(GetSelfID(), "closed");
--	end
--	if IsLit == 1 then
--		PlayAnim(GetSelfID(), "opened");
--	end
end

function Use(action)
	Print("Got Action:",action);
 
	IsLit = GetLocalDouble(GetSelfID(), "IsLit");

	if action == "Open" then
		-- Light it
		if IsLit == 0 then
			SetPSystem(GetSelfID(), "torch");
			SetLocalDouble(GetSelfID(), "IsLit", 1);
		end 
	end
	
	if action == "Close" then
		-- Turn it off
		if IsLit == 1 then
			SetPSystem(GetSelfID());
			SetLocalDouble(GetSelfID(), "IsLit", 0);
		end 
	end

end