function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/money.mad");			
			InitParameter("m_fScale","1");		
		InitProperty("P_DMItem");	
			InitParameter("name","money");
			InitParameter("sizex",1);
			InitParameter("sizey",1);
			InitParameter("icon","stol.bmp");
			InitParameter("type","0");
		InitProperty("P_ScriptInterface");


end

function Init()
end

function FirstRun()
end
