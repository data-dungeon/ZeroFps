function Create()
	
	InitObject();
		IsStatic();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/cavepillar2.mad");
			InitParameter("m_fScale",0.15);
		InitProperty("P_Event");
		SetParentObject();
		SetReturnObject();
end

function Init()
end
