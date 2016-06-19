# DontStarveLuaJIT for Windows
_LuaJIT for DontStarve (compatible with DS, RoG, SW, not with DST)_

Tested revisions（已测试版本）:  v181038-v181319 WIN32_STEAM

###  The first test version of DontStarveLuaJIT for Windows 

###  这是DontStarveLuaJIT For Windows 的第一个测试版本！

###  PLEASE BACKUP YOUR SAVES BEFORE APPLYING THIS PATCH. 

###  在启用这个补丁之前，请务必备份你的所有存档。


##Installation（安装）: 

1	Copy all files from "DontStarveLuaJIT/bin/" to "Steam/SteamApps/common/dont_starve/bin/"

	复制"DontStarveLuaJIT/bin/"目录下的所有文件至"Steam/SteamApps/common/dont_starve/bin/"

2	Open "Steam/SteamApps/common/dont_starve/data/scripts/mods.lua" with an text editor. Locate the following lines:

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

3	Open "Steam/SteamApps/common/dont_starve/data/scripts/modutil.lua" with an text editor. 
	
	Replace all 'arg = {...}' with 'local arg = {...}' (quote mark not included)

	使用文本编辑器打开“Steam/SteamApps/common/dont_starve/data/scripts/modutil.lua”文件。
	
	将所有的'arg = {...}'替换为'local arg = {...}' （不包含引号）

---------------------------------------------------

The reason for modification: Official lua distribution 5.1.4+ and LuaJIT refined the concept of variant arguments so grammar sugar 'arg' was no longer supported. Lua programmers should use {...} instead. The developers of Don't Starve obeyed the rules except 'mods.lua' because '...' cannot be passed across closure boundary. So they simply wrote 'arg', which leads to runtime error in LuaJIT and advanced lua engines,  since 'arg' is reinterpreted as a global variable (but it's nil). Now we must declare 'arg' manually.

这样修改的原因是：Lua官方所发布的新版lua以及LuaJIT修正了不定长参数的概念，现在lua不再支持'arg'这个语法糖来表示不定参数的列表。Lua程序员如果需要，可以显式地使用{...}来代替它。Don't Starve的开发者在绝大多数情况下都遵守了这个变动。然而，在mods.lua中，他们发现'...'表示的不定参数不能穿越闭包边界，因此使用了旧的用法。现在我们需要手动定义'arg'，以保证程序可以正确执行。

---------------------------------------------------

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

##Build（生成）: 

The project 'lua51' in solution must be compiled under MSVC9 (Visual Studio 2008) to generate binary-compatible code for dontstarve_steam.exe. To compile luajit, please launch Visual C++ Build Prompt and run msvcbuild.bat.

为确保与饥荒主程序的二进制兼容性，解决方案中的lua51必须使用MSVC9（即VS2008）来编译。如果您要自行编译luajit，请在Visual C++的控制台中运行luajit目录下的msvcbuild.bat。
