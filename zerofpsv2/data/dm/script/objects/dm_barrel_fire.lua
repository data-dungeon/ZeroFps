function Create()
	
	InitObject();
		InitProperty("P_PSystem");
			InitParameter("PSType", "barrel_fire");
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/dm_barrel_fire.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_ShadowBlob");
			InitParameter("scale","1.5");

		SetParentObject();
		SetReturnObject();
end

