function Create()
	InitObject();
		InitProperty("P_Spawn");	
			InitParameter("SpawnDelay",3);
			InitParameter("SpawnMode",1);			
			InitParameter("Template","data/script/objects/demoobjects/t_demotroll.lua");			
			InitParameter("SpawnArea",1);			
			InitParameter("MaxSpawn",4);			
		
		SetParentObject();
		SetReturnObject();
end
