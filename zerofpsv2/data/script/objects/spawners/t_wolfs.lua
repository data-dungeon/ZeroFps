function Create()
	InitObject();
		InitProperty("P_Spawn");	
			InitParameter("SpawnDelay",5);
			InitParameter("SpawnMode",1);			
			InitParameter("Template","data/script/objects/characters/t_wolf.lua");			
			InitParameter("SpawnArea",16);			
			InitParameter("MaxSpawn",10);			
		
		SetParentObject();
		SetReturnObject();
end

function Init()

end

function HeartBeat()

end