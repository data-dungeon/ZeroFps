function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/dm_barrel.mad");			
			InitParameter("m_fScale","1");		

		InitProperty("P_DMItem");	
			InitParameter("name","police_bomb");
			InitParameter("sizex",4);
			InitParameter("sizey",4);
--			InitParameter("icon","shotgun.bmp");
			InitParameter("type","5"); -- default, only backpack
		InitProperty("P_ScriptInterface");


end

function FirstRun()
end

function Init()
	SetTeam (SIGetSelfID(), 10);
end


