function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/gun.mad");			
			InitParameter("m_fScale","1");		
		InitProperty("P_DMItem");	
			InitParameter("name","gun");
			InitParameter("sizex",2);
			InitParameter("sizey",2);
			InitParameter("icon","stol.bmp");
		InitProperty("P_DMGun");
		InitProperty("P_ScriptInterface");


end

function FirstRun()
end

function HeartBeat()

end