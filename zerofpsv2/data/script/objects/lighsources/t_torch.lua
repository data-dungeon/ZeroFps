function Create()
	
	InitObject();
		IsStatic();
		InitProperty("P_PSystem");
			InitParameter("PSType", "torch");
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/torch.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Light");
			InitParameter("Mode",1);
			InitParameter("Quadratic_Atten",0.003);
		InitProperty("P_AmbientSound");
			InitParameter("FileName","/data/sound/fire.wav");
		InitProperty("P_Event");
		InitProperty("P_Ml");
		
		SetParentObject();
		SetReturnObject();
end

