function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/dm_barrel.mad");
			InitParameter("m_fScale","1");		
		InitProperty("P_ShadowBlob");
			InitParameter("scale","1.5");
end
