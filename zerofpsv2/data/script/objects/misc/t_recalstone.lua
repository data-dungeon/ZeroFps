function Create()
	InitObject();
		IsStatic();	
			InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/recallstone.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
		InitProperty("P_Ml");
		
		SetParentObject();
		SetReturnObject();
end

function Init()
	AddAction(GetSelfID(),"SetRecallPos");
	AddAction(GetSelfID(),"Look");	
end

function Use(action)
	if action == "SetRecallPos" then
		SetRecalPosition(GetCurrentPCID(),GetObjectPos(GetCurrentPCID()));
		MessageCaracter(GetCurrentPCID(),"Recall position set, you may now safly logout =)");		
	end
	
	if action == "Look" then
		MessageCaracter(GetCurrentPCID(),"One of those old magic recall stones");		
	end

end