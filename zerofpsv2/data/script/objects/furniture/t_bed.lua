function Create()
	
	InitObject();
		IsStatic();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/bed.mad");
			InitParameter("m_fScale","1");
		InitProperty("P_Event");
		SetParentObject();
		SetReturnObject();
end

function Init()
end
