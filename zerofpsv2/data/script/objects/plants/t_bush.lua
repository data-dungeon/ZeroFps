function Create()
	
	InitObject();
		IsStatic();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/bush.mad");
			InitParameter("m_fScale","1");
			InitParameter("m_bCanBeInvisible","true");
		SetParentObject();
		SetReturnObject();
end



