function Create()
	InitObject();
		InitProperty("P_Spawn");	
			InitParameter("SpawnDelay",2);
			InitParameter("SpawnMode",1);			
			InitParameter("Template","data/script/objects/dm/t_mob_woman.lua");			
			InitParameter("SpawnArea",1);			
			InitParameter("MaxSpawn",15);			
		InitProperty("P_Track");
		SetParentObject();
		SetReturnObject();
end

function Init()

end

