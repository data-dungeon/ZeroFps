function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/dmbox.mad");
			InitParameter("m_fScale","1.0");
		
		SetParentObject();
		SetReturnObject();
end

