function Create()

	

	InitObject();

		InitProperty("P_Mad");	

			InitParameter("m_kMadFile","/data/mad/dm/dm_grenade.mad");			

			InitParameter("m_fScale","1.0");		

		InitProperty("P_DMItem");	

			InitParameter("name","Grenade");

			InitParameter("sizex",1);

			InitParameter("sizey",1);

			InitParameter("icon","grenade_disk.bmp");

			InitParameter("type","7");

		InitProperty("P_ScriptInterface");





end



function Init()

end



function FirstRun()

end



function Use(UserID)

	ID = RunScript("data/script/objects/weapons/grenade_unsafe.lua",UserID);



	r = GetObjectRot(UserID);

	p = GetObjectPos(UserID);



	p[2] = p[2] + 2;



	SetObjectPos(ID, p);



	r[1] = r[1] * -15;

	r[2] = -8;

	r[3] = r[3] * -15;

	

	SetVelTo(ID,r,25);

end



