function Create()
	InitObject();
		InitProperty("P_Spawn");	
			InitParameter("SpawnDelay",30);
			InitParameter("SpawnMode",1);			
			InitParameter("Template","data/script/objects/characters/t_goblin.lua");			
			InitParameter("SpawnArea",8);			
			InitParameter("MaxSpawn",5);			
		
		SetParentObject();
		SetReturnObject();
end

function Init()

end

function HeartBeat()

end