function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/dm_machinegun_clip.mad");			
			InitParameter("m_fScale","1.0");		
		InitProperty("P_DMItem");	
			InitParameter("name","Machinegun clip");
			InitParameter("sizex",1);
			InitParameter("sizey",1);
			InitParameter("icon","machinegun_clip.bmp");
			InitParameter("type","4");
		InitProperty("P_ScriptInterface");


end

function Init()
end

function FirstRun()
end
