function Create()
	
	InitObject();
		IsStatic();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/well.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
		InitProperty("P_Ml");
		
		SetParentObject();
		SetReturnObject();
end

function Init()
	AddAction(GetSelfID(),"Eat");
end

function Use(action)
	if action == "Eat" then
		MessageCaracter(GetCurrentPCID(),"The water tastes bitter.");	
	end
end
