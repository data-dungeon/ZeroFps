function Create()
	InitObject();
		InitProperty("P_Spawn");	
			InitParameter("SpawnDelay",0.01);
			InitParameter("SpawnMode",1);			
--			InitParameter("Template","data/script/objects/characters/t_wolf.lua");			
			InitParameter("Template","data/script/objects/arghhh/cp.lua");						
			InitParameter("SpawnArea",16);			
			InitParameter("MaxSpawn",10000);			
		
		SetParentObject();
		SetReturnObject();
end

function Init()

end

function HeartBeat()

end