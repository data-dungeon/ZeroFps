function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/dm_box.mad");
			InitParameter("m_fScale","1");
			InitParameter("m_iShadowGroup","1");	
		InitProperty("P_ShadowBlob");
			InitParameter("scale","1");
end
