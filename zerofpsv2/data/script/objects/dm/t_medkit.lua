function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/dm_medkit.mad");			
			InitParameter("m_fScale","1.0");		
		InitProperty("P_DMItem");	
			InitParameter("name","Medkit");
			InitParameter("sizex",1);
			InitParameter("sizey",1);
			InitParameter("icon","medkit.bmp");
			InitParameter("type","7");
		InitProperty("P_ScriptInterface");


end

function Init()
end

function FirstRun()
end

function Use(UserID)
	Heal(UserID);
end
