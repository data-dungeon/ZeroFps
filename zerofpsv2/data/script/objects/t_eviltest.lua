bla = {};

function Create()
	
	InitObject();
		InitProperty("P_Mad");
			InitParameter("m_kMadFile","/data/mad/babe.mad");
			InitParameter("m_fScale",0.1);	
		SetParentObject();
		SetReturnObject();
end

function Init()
--Print("im a ball");
end

function Destroy()
--	Print("im dieing");
	bla[GetSelfID()] = nil;
end


