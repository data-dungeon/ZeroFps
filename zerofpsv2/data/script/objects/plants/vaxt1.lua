function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/vaxt.mad");
			InitParameter("m_fScale","1");
		InitProperty("P_Event");
		InitProperty("P_ShadowBlob");		
		SetParentObject();
		SetReturnObject();

end

function Init()
end
