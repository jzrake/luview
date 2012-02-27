
-- Make proxy object with property support.
-- Notes:
--   If key is found in <getters> (or <setters>), then
--     corresponding function is used, else lookup turns to the
--     <class> metatable (or first to <priv> if <is_expose_private> is true).
--   Given a proxy object <self>, <priv> can be obtained with
--     getmetatable(self).priv .
-- @param class - metatable acting as the object class.
-- @param priv - table containing private data for object.
-- @param getters - table of getter functions
--                  with keys as property names. (default is nil)
-- @param setters - table of setter functions,
--                  with keys as property names. (default is nil)
-- @param is_expose_private - Boolean whether to expose <priv> through proxy.
--                  (default is nil/false)
-- @version 3 - 20060921 (D.Manura)



local function make_proxy1(class, priv, getters, setters, is_expose_private)
   -- The following line must be done from C if priv is a userdata:
   -- setmetatable(priv, class) -- fallback priv lookups to class
   local fallback = is_expose_private and priv or class
   local index = getters and
      function(self, key)
         -- read from getter, else from fallback
         local func = getters[key]
         if func then return func(self) else return fallback[key] end
      end or class.__index -- default to fast property reads through table
   local newindex = setters and
      function(self, key, value)
         -- write to setter, else to proxy
         local func = setters[key]
         if func then func(self, value)
         else rawset(self, key, value) end
      end or fallback -- default to fast property writes through table
   local proxy_mt = { -- create metatable for proxy object
      __newindex = newindex,
      __index = index,
      __tostring = class.__tostring,
      priv = priv
   }
   local self = setmetatable({}, proxy_mt) -- create proxy object
   return self
end


local function make_proxy(class, priv, getters, setters)

   local index = getters and
      function(self, key)
	 local func = getters[key]
         if func then
	    return func(self)
	 elseif class[key] then
	    return class[key]
	 else
	    return rawget(self, key)
	 end
      end or class.__index
   
   local newindex = setters and
      function(self, key, value)
	 local func = setters[key]
         if func then func(self, value) else rawset(self, key, value) end
      end or class.__newindex

   return setmetatable({},
		       { __index    = index,
			 __newindex = newindex,
			 __tostring = class.__tostring,
			 priv       = priv })
end

return function(...)
          return make_proxy(...)
       end
