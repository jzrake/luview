
require 'luview'

function luview.traits(opts)
   local t = {
      Position           = { 0.0, 0.0, 0.0 },
      Orientation        = { 0.0, 0.0, 0.0 },
      Color              = { 0.5, 0.5, 0.5 },
      Scale              =   1.0,
      LineWidth          =   1.0,
      HasFocus           =   false
   }
   for k,v in pairs(opts) do t[k] = v end
   return t
end

luview.Init()
luview.OpenWindow()

local Camera = {

   Position={0,0,1.8},
   Angle={40,0},
   Response = function(self)
		 return {
		    [luview.KEY_RIGHT] = function() self.Angle[2] = self.Angle[2] + 3 end,
		    [luview.KEY_LEFT ] = function() self.Angle[2] = self.Angle[2] - 3 end,
		    [luview.KEY_DOWN ] = function() self.Angle[1] = self.Angle[1] + 3 end,
		    [luview.KEY_UP   ] = function() self.Angle[1] = self.Angle[1] - 3 end,
		 }
	      end
}

local function boxactor(t)
   return { Artist = luview.BoundingBoxArtist,
	    Traits = luview.traits(t),
	    Response =
	       function(self)
		  local ori = self.Traits.Orientation
		  return {
		     ["x"] = function() ori[1] = ori[1] + 3 end,
		     ["y"] = function() ori[2] = ori[2] + 3 end,
		     ["z"] = function() ori[3] = ori[3] + 3 end,
		  }
	       end
	 }
end


local Scene  =  {
   ObjFoc    =  nil,
   KeyFoc    =  nil,
   Actors    =  { boxactor{Scale=0.10, Color={0.1,0.7,0.9}},
		  boxactor{Scale=0.40, Color={0.5,0.7,0.5}},
		  boxactor{Scale=0.90, Color={0.9,0.7,0.1}}, },
   Camera    =  Camera,
   Keyboard  =  function(self, key, state)
		   if key == "n" then
		      self:NextObject()
		      return
		   end
		   if self.Camera:Response()[key] then return self.Camera:Response()[key]() end
		   if self.ObjFoc then
		      if self.ObjFoc:Response()[key] then return self.ObjFoc:Response()[key]() end
		   end
		   return nil
		end,
   NextObject = function(self)
		   if self.ObjFoc then self.ObjFoc.Traits.LineWidth = 1.0 end
		   self.KeyFoc, self.ObjFoc = next(self.Actors, self.KeyFoc)
		   if self.ObjFoc then self.ObjFoc.Traits.LineWidth = 3.0 end
		end
}

luview.RedrawScene(Scene)
