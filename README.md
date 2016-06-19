# DontStarveLuaJIT for Windows
_LuaJIT for DontStarve (compatible with DS, RoG, SW, not with DST)_

Tested revisions:  v181038-v181319 WIN32_STEAM

Installation（安装）: 

1. Copy all files from "DontStarveLuaJIT/bin/" to "Steam/SteamApps/common/dont_starve/bin/"

复制"DontStarveLuaJIT/bin/"目录下的所有文件至"Steam/SteamApps/common/dont_starve/bin/"

2. Offical lua distribution 5.1.4+ and LuaJIT refine the concept of variant arguments and 'arg' is no longer supported. Lua programmers should use {...} instead. The developers of Don't Starve obey the rules except 'mods.lua' because '...' cannot be passed across closure boundary. So they simply write 'arg', which leads to runtime error in LuaJIT and advanced lua engines because 'arg' is reinterpreted as a global variable (but it's nil). Now we must declare 'arg' manually.

Lua官方所发布的新版lua以及LuaJIT修正了不定长参数的概念，现在lua不再支持'arg'这个语法糖来表示不定参数的列表。Lua程序员如果需要，可以显式地使用{...}来使用它。Don't Starve的开发者在绝大多数情况下都遵守了这个变动。然而，在mods.lua中，他们发现'...'表示的不定参数不能穿越闭包边界，因此使用了旧的用法。现在我们需要手动定义'arg'，以保证程序可以正确执行。

 Open "Steam/SteamApps/common/dont_starve/data/scripts/mods.lua" with an text editor. Locate the following lines:

使用文本编辑器打开“Steam/SteamApps/common/dont_starve/data/scripts/mods.lua”文件。定位到如下代码行：

```lua
local runmodfn = function(fn,mod,modtype)
	return (function(...)
		if fn then
			local arg = {...} --<<<<
			local status, r = xpcall( function() return fn(unpack(arg)) end, debug.traceback)
			if not status then
				print("error calling "..modtype.." in mod "..ModInfoname(mod.modname)..": \n"..r)
				ModManager:RemoveBadMod(mod.modname,r)
				ModManager:DisplayBadMods()
			else
				return r
			end
		end
	end)
end
```
Add one line at --<<<< mark as above.

在-<<<<标记处如上所示添加一行代码。

Save mods.lua. 

保存 mods.lua 文件。


NOTICE: Some mods, for example, 'Smart Craft Pot', are still using 'arg' in its code, which leads to crash when player activates related function. Here's an example for fixing it (take 'Smart Craft Pot' as example).

注意：有些MOD，例如'Smart Craft Pot'，也使用了旧式的'arg'语法。当用户触发了含有这种语法的函数时，游戏将崩溃。以'Smart Craft Pot'为例，我们也需要修复它：

 Open "Steam/SteamApps/common/dont_starve/mods/workshop-662872357/scripts/widgets/foodcrafting.lua" with an text editor. Locate the following lines (in fact, the file:line of crashing will be displayed in Don't Starve crash text):


使用文本编辑器打开“Steam/SteamApps/common/dont_starve/data/scripts/mods.lua”文件。定位到如下代码行（实际上，崩溃的文件：行号在崩溃的时候会显示在Don't Starve 的崩溃界面上，很容易找）：

```lua
function FoodCrafting:_GetContainerIngredients(...)
	local ings = {}

	-- for _,container in ipairs(arg) do -- >>>
	for _,container in ipairs({...}) do -- <<<
		local slots = container.slots or container.itemslots or {}

  	for k,v in pairs(slots) do
			local amt = v.components.stackable and v.components.stackable.stacksize or 1
    	table.insert(ings, {name=v.prefab,amt=amt})
  	end
	end
  return ings
end
```

Modify code as above.

如上所示修改即可。
