function Create()
	
	InitObject();
		IsStatic();	
			InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/roadsign.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
		InitProperty("P_Ml");
		
		SetParentObject();
		SetReturnObject();
end

function Init()
	AddAction(GetSelfID(),"Look");
end

function Use(action)
	if action == "Look" then		
		MessageCaracter(GetCurrentPCID(),"West: Town of good people");		
		MessageCaracter(GetCurrentPCID(),"East: Dungeon of evil monsters");				
	end

end