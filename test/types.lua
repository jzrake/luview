

require 'luview'


local V = luview.Vec3:new{1,2,3}
local T = luview.Traits:new()


local V = luview.Vec3:new{1,2,3}
V[1] = 55.0
print("55, 2, 3 ?= ", V[1],V[2],V[3], "?=", V)

local T = luview.Traits:new{Position={.2,.3,.4}, LineWidth=77.7}
T.Scale = 33.3
print("33.3 ?= ", T.Scale)
print("77.7 ?= ", T.LineWidth)
print("(0.2, 0.3, 0.4) ?= ", T.Position)


T:update{Scale=44.4}
print("44.4 ?= ", T.Scale)

T.Color = { 555,666,777 }
print("555,666,777 ?= ", T.Color)

T.Color[3] = T.Color[3] + 3
print("555,666,780 ?= ", T.Color)

T.Position = { 9,9,9 }
print("9,9,9 ?= ", T.Position)

