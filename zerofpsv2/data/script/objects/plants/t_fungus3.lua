function Create()
	
	InitObject();
		IsStatic();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/fungus.mad");
			InitParameter("m_fScale","2");
		InitProperty("P_Event");
		SetParentObject();
		SetReturnObject();
end

function Init()
end
