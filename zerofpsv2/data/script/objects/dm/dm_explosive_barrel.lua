function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/dm_barrel.mad");			
			InitParameter("m_fScale","1");		

		InitProperty("P_DMCharacter");
		InitProperty("P_ScriptInterface");
		InitProperty("P_ShadowBlob");

end

function FirstRun()
end

function Init()
	SetTeam (SIGetSelfID(), 10);
end


function Dead()
	RunScript("data/script/objects/dm/t_explosion.lua",SIGetSelfID());
	Delete (SIGetSelfID());
end
