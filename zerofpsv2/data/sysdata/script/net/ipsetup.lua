IpList = 
{ 
	{ name = "lh-Vim",   ip = "127.0.0.1:4242", character_name = "vim",  password = "miv" },	-- 1

}

function SetupIP()

	local users = 1 -- change to new number if iplist change size

	for i=1, users, 1 do 
		AddServer( IpList[i].name, IpList[i].ip, IpList[i].character_name, IpList[i].password )
	end

	SetDefaultServer("zeb")

end