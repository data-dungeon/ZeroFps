function Create()
	
	InitObject();
		IsStatic();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/galler.mad");
			InitParameter("m_fScale","1");
		InitProperty("PSystemProperty");
			InitParameter("PSType", "poisonsmoke");
		InitProperty("P_Event");
		SetParentObject();
		SetReturnObject();
end

function Init()
end
