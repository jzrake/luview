
require 'luview'

function luview.traits(opts)
   local t = {
      Position           = { 0.0, 0.0, 0.0 },
      Color              = { 0.5, 0.5, 0.5 },
      Scale              =   1.0,
      LineWidth          =   2.0,
      KeyboardResponse   =   nil,
      CharacterResponse  =   nil,
   }
   for k,v in pairs(opts) do t[k] = v end
   return t
end

luview.Init()
luview.OpenWindow()

local LuviewActor = {
   Artist = luview.BoundingBoxArtist,
   Traits = luview.DefaultTraits,
   Spotlight = false
}

local Camera = { position={0,0,1.8}, angle={40,0} }
local Response = {
   [luview.KEY_RIGHT] = function() Camera.angle[2] = Camera.angle[2] + 3 end,
   [luview.KEY_LEFT ] = function() Camera.angle[2] = Camera.angle[2] - 3 end,
   [luview.KEY_DOWN ] = function() Camera.angle[1] = Camera.angle[1] + 3 end,
   [luview.KEY_UP   ] = function() Camera.angle[1] = Camera.angle[1] - 3 end,
}

local boxartist = luview.BoundingBoxArtist()

luview.RedrawScene(
   {  Draw      =  function()
                      boxartist:Draw(luview.traits{Scale=0.05, Color={0.1,0.1,0.2}})
                      boxartist:Draw(luview.traits{Scale=0.25, Color={0.2,0.2,0.6}})
                      boxartist:Draw(luview.traits{Scale=0.50, Color={0.3,0.5,0.9}})
                      boxartist:Draw(luview.traits{Scale=0.70, Color={0.5,0.8,0.5}})
                   end,
      Camera    =  Camera,
      Keyboard  =  function(key, state)
                      if Response[key] then Response[key]() end
                   end
   })
