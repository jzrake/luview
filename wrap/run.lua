


local function test_method_calls()
   local dog = tests.Dog()
   local cat = tests.Cat()

   for number, animal in pairs({dog, cat}) do
      print("for animal type " .. animal:get_type())
      animal:speak()
      animal:eat(number)
   end

   local owner = tests.PetOwner()

   cat:set_name("orange")
   dog:set_name("murphy")

   owner:set_dog(dog)
   owner:set_cat(cat)

   print(cat)
   print("orange =?", owner:get_cat():get_name())
end

local function test_casting()
   local dog = tests.Dog()
   local cat = tests.Cat()
   local jacko = tests.Poodle()

   print(dog, cat, jacko)
   local owner = tests.PetOwner()

   owner:set_dog(dog)
   print(owner:get_dog():eat(10))

   owner:set_dog(jacko)
   print(owner:get_dog():eat(10))
end

local function test_gc()
   things = { }

   for i=1,10 do
      things[i] = tests.Dog()
   end

   print("should collect now...")

   things[1] = nil
   things[2] = nil
   things[3] = nil
   things[4] = nil

   collectgarbage()

   things[1] = tests.Dog()
   things[2] = tests.Dog()
   things[3] = tests.Dog()
   things[4] = tests.Dog()


   print("collected?")

   for k,v in pairs(LuaCppObject) do
      print(k,v)
   end
end



test_casting()
test_method_calls()
test_gc()
