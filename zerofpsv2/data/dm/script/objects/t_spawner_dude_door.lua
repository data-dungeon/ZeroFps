function Create()

	

	InitObject();

		InitProperty("P_Mad");	

			InitParameter("m_kMadFile","/data/mad/dm/dm_door.mad");			

			InitParameter("m_fScale","1");		

		InitProperty("P_ScriptInterface");

		InitProperty("P_Spawn");	

			InitParameter("SpawnDelay",15);

			InitParameter("SpawnMode",1);			

			InitParameter("Template","data/script/objects/t_citizen_dude.lua");	

			InitParameter("SpawnArea",1);			

			InitParameter("MaxSpawn",6);	

		InitProperty("P_Sound");

end





function Init()

end





function OnSpawn()

	PlayAnim(SIGetSelfID(), "open");

	SetNextAnim(SIGetSelfID(), "idle");

	PlaySound (SIGetSelfID(), "dooropen.wav");	

end

