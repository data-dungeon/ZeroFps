
--///////////////////////////////////////////////////////////////////////////////
--//
--// Mission: Another mission
--// 
--///////////////////////////////////////////////////////////////////////////////

MissionInfo = { name="Dr Gun", difficulty=1, xp=100, cash=500, success=0 } -- difficulty = Level

NumCivilians = 0
NumCiviliansWounded = 0
CivWounded = {} -- lista p� vilka civilians som har blivit skadade
InjureList = {} -- en annan lista som s�ks igenom f�r att se om n�gra skadeskjutna civila har d�tt

MissionText = 
{ 
	short = "Dr Gun",
	long  = [[Dr Gun\n\nMr. Wilhelm Von Leath wont you to work for him for a while.\n\nYou are hired to damage 5 persons. 
	 You can pick anyone, he does't care. When you are done return to base. You will get 1000 dollar in cash
	 when the mission is done]]
}

function OnMissionStart()

	Print( "starting mission Dr Gun" )

	CivWounded = {} -- t�m listan

	local team = 1 -- team civilians
	local civ_list = GetCharsByFraction(team) -- h�mta en lista p� alla civila
	NumCivilians = civ_list[1] -- f�rsta arg �r vektorns size

	local counter = 0 
	for x = 2, NumCivilians+1, 1
	do
		local obj_id = civ_list[x]
		local life = GetCharStats( obj_id, 0 ) -- 0 = Life
		
		CivWounded[counter] = obj_id
		counter = counter + 1

		CivWounded[counter] = life
		counter = counter + 1
	end

end

function OnMissionSuccess()

		

end

function OnMissionFailed()



end

-- Anropas var 3:e sekund
function IsMissionDone()

	-- Kolla om n�gra civila har blivit skadade
	for x = 0, NumCivilians-1, 1
	do
		local obj = CivWounded[x*2] 

		if obj > -1 then -- hoppa f�rbi dom som redan har blivit r�knade som skadade

			local prev_life = CivWounded[x*2+1] 
			local curr_life = GetCharStats( obj, 0 ) -- 0 = Life
			local dmg = prev_life - curr_life

			if dmg > 0 then
				
				CivWounded[x*2] = -1 -- markera att denna inte skall r�knas mer

				InjureList[NumCiviliansWounded] = obj -- l�gg till till lista �ver skadeskjutna gubbar

				NumCiviliansWounded = NumCiviliansWounded + 1 -- �ka antalet skadade personer
			end

		end

	end

	-- Kolla om n�gra har blivit skadeskjutna till d�ds

	if NumCiviliansWounded > 0 then

		for y = 0, NumCiviliansWounded-1, 1 
		do
			local obj_wounded = InjureList[y]

			if obj_wounded > -1 then -- om objektet inte redan �r d�d

				if IsDead(obj_wounded) == 1 then

					Print("--------- YOU ARE NOT SUPPOSED TO KILL PEOPLE! ------------")

					NumCiviliansWounded = NumCiviliansWounded - 1 -- minska antalet skadade personer

					InjureList[y] = -1
				end

			end
		end

	end

	Print("Person wounded: ", NumCiviliansWounded)

	if NumCiviliansWounded == 3 then

		MissionInfo.success = 1 -- Uppdraget har lyckats!

	end

end

function IsMissionFailed()



end