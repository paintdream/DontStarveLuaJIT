# DontStarveLuaJIT
LuaJIT for DontStarve (compatible with DS, RoG, SW, not with DST)

Installation（安装）:

1. Copy all files from "DontStarveLuaJIT/bin/" to "Steam/SteamApps/common/dont_starve/bin/"
复制"DontStarveLuaJIT/bin/"目录下的所有文件至"Steam/SteamApps/common/dont_starve/bin/"
2. Open "Steam/SteamApps/common/dont_starve/data/scripts/mods.lua" with an text editor. Locate the following lines:
使用文本编辑器打开“Steam/SteamApps/common/dont_starve/data/scripts/mods.lua”文件。定位到如下代码行：

local runmodfn = function(fn,mod,modtype)
	return (function(...)
		if fn then
			local arg = {...} --<<<< insert this line of code. 在这里加上这行代码
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

Save mods.lua. 
保存 mods.lua 文件。