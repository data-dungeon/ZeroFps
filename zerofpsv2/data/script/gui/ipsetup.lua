IpList = 
{ 
	{ name = "vim",   ip = "192.168.0.153:4242", character_name = "vimus",  password = "olle" },	-- 1
	{ name = "hugo",  ip = "192.168.0.156:4242", character_name = "hugge",  password = "olle" },	-- 2
	{ name = "dvoid", ip = "192.168.0.178:4242", character_name = "nisse",  password = "olle" },	-- 3
	{ name = "zeb",   ip = "192.168.0.160:4242", character_name = "zeblar", password = "olle" },	-- 4
	{ name = "zeb2",  ip = "192.168.0.170:4242", character_name = "zebbe",  password = "olle" },	-- 5
	{ name = "gubb",  ip = "192.168.0.176:4242", character_name = "gubbe",  password = "olle" },	-- 6
	{ name = "zerom", ip = "192.168.0.154:4242", character_name = "zerre",  password = "olle" }	-- 7
}

function SetupIP()

	local users = 7 -- change to new number if iplist change size

	for i=1, users, 1 do 
		AddServer( IpList[i].name, IpList[i].ip, IpList[i].character_name, IpList[i].password )
	end

	SetDefaultServer("zeb")

end