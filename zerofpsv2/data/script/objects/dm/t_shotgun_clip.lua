function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/shotgun_clip.mad");			
			InitParameter("m_fScale","1.5");		
		InitProperty("P_DMItem");	
			InitParameter("name","ShotgunClip");
			InitParameter("sizex",1);
			InitParameter("sizey",1);
			InitParameter("icon","shotgun_clip.bmp");
			InitParameter("type","4");
		InitProperty("P_ScriptInterface");


end

function Init()
end

function FirstRun()
end
