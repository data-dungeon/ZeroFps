function Create()
	InitObject();
		InitProperty("P_Spawn");	
			InitParameter("SpawnDelay",3);
			InitParameter("SpawnMode",1);			
			InitParameter("Template","data/script/objects/demoobjects/t_demogoblin.lua");			
			InitParameter("SpawnArea",1);			
			InitParameter("MaxSpawn",10);			
		
		SetParentObject();
		SetReturnObject();
end
