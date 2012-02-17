
require 'luview'
require 'lunum'


function luview.traits(opts)
   local t = {
      Position           = { 0.0, 0.0, 0.0 },
      Orientation        = { 0.0, 0.0, 0.0 },
      Color              = { 0.5, 0.5, 0.5 },
      Scale              =   1.0,
      LineWidth          =   1.0,
      Transparency       =   1.0,
   }
   if opts then
      for k,v in pairs(opts) do t[k] = v end
   end
   return t
end

function luview.PositionResponse(self)
   local ori = self.Traits.Orientation
   local pos = self.Traits.Position
   return {
      ["x"] = function() ori[1] = ori[1] + 3 end,
      ["y"] = function() ori[2] = ori[2] + 3 end,
      ["z"] = function() ori[3] = ori[3] + 3 end,
      ["a"] = function() pos[1] = pos[1] + 0.01 end,
      ["b"] = function() pos[2] = pos[2] + 0.01 end,
      ["c"] = function() pos[3] = pos[3] + 0.01 end,
   }
end

function luview.ColormapResponse(self)
   local src = self.raw_source
   local fil = luview.ColormapFilter
   local c = '1'
   return {
      ["0"] = function() self.DataSource.Colors = fil(src,0,c) end,
      ["1"] = function() self.DataSource.Colors = fil(src,1,c) end,
      ["2"] = function() self.DataSource.Colors = fil(src,2,c) end,
      ["3"] = function() self.DataSource.Colors = fil(src,3,c) end,
      ["4"] = function() self.DataSource.Colors = fil(src,4,c) end,
      ["5"] = function() self.DataSource.Colors = fil(src,5,c) end,
      ["6"] = function() self.DataSource.Colors = fil(src,6,c) end,
      ["w"] = function() self.Traits.LineWidth = self.Traits.LineWidth - 0.2 end,
      ["W"] = function() self.Traits.LineWidth = self.Traits.LineWidth + 0.2 end,
   }
end

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
            Response = luview.PositionResponse,
	    DataSource = nil
         }
end

local cells = lunum.loadtxt("/Users/jzrake/Work/luview/test/cells999.dat")

local Scene  =  {
   ObjFoc    =  nil,
   KeyFoc    =  nil,
   Actors    =  { 
                  boxactor{Scale=0.95, Color={0.3,0.8,0.3}},

		  { Artist   = luview.PointsListArtist,
		    Traits   = luview.traits{Position={-0.5,-0.5,-0.5}},
		    Response = luview.ColormapResponse,
		    raw_source = cells[':,6'],
 		    DataSource = { Positions=cells[':,0:3'],
				   Colors=luview.ColormapFilter(cells[':,6'],0) }
		 }
               },
   Camera    =  Camera,
   Keyboard  =  function(self, key, state)
                   if key == "n" then
                      self:NextObject()
                      return
                   end
                   if self.Camera:Response()[key] then
                      return self.Camera:Response()[key]()
                   end
                   if self.ObjFoc then
                      if self.ObjFoc.Response then
                         if self.ObjFoc:Response()[key] then
                            return self.ObjFoc:Response()[key]()
                         end
                      end
                   end
                   return nil
                end,

   NextObject = function(self)
                   if self.ObjFoc then self.ObjFoc.Traits.LineWidth = 1.0 end
                   self.KeyFoc, self.ObjFoc = next(self.Actors, self.KeyFoc)
                   if self.ObjFoc then self.ObjFoc.Traits.LineWidth = 3.0 end
                end
}


luview.Init()
luview.OpenWindow()
luview.RedrawScene(Scene)
