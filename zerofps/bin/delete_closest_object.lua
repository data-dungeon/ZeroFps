-- script test for game

player = game:GetPlayer()
closest_object = game:GetClosestObject( player )
Print( game:GetObjectType(closest_object) )
game:DestroyObject( closest_object )