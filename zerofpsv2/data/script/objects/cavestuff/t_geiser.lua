function Create()
	
	InitObject();
		IsStatic();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/geiser.mad");
			InitParameter("m_fScale","1");
		InitProperty("PSystemProperty");
			InitParameter("PSType", "geisersmoke");
		InitProperty("P_Event");
		SetParentObject();
		SetReturnObject();
end

function Init()
end
