function Create()
	
	InitObject();
		IsStatic();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/tree2.mad");
			InitParameter("m_fScale","1");
			InitParameter("m_bCanBeInvisible","true");
		InitProperty("P_Event");
		SetParentObject();
		SetReturnObject();
end

function Init()
end
