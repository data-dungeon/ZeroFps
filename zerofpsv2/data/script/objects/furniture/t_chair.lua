function Create()
	
	InitObject();
		InitProperty("P_DMItem");	
			InitParameter("name","stol");
			InitParameter("sizex",3);
			InitParameter("sizey",4);
			InitParameter("icon","stol.bmp");
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/chair.mad");
			InitParameter("m_fScale","1");
		InitProperty("P_Event");
		InitProperty("P_ShadowBlob");		
		SetParentObject();
		SetReturnObject();
end

function Init()
end
