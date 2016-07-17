# DontStarveLuaJIT for Windows
	LuaJIT for DontStarve (compatible with DS, RoG, SW, DST)

	Tested revisions（已测试版本）:  v181038-v181319 WIN32_STEAM

####  The first test version of DontStarveLuaJIT for Windows 

####  这是DontStarveLuaJIT For Windows 的第一个测试版本！

####  PLEASE BACKUP YOUR SAVES BEFORE APPLYING THIS PATCH. 

####  在启用这个补丁之前，请务必备份你的所有存档。

####  YOU MUST HAVE THIS MOD INSTALLED ON BOTH SERVER AND CLIENT FOR DON'T STARVE TOGETHER!

####  如果您打算将这个MOD安装到联机版本（Don't Starve Together），请确保在客户端和服务端都安装了这个MOD！

-----------------------------------------------------

#####  SOME SAVES CREATED WITHOUT THIS PATCH CANNOT BE LOADED DUE TO EXTRA
#####  DATA WRITTEN BY SOME MODS USING ENGINE-RELATED FUNCTIONS (SUCH AS lua_dump).
#####  THESE DATA CANNOT BE RECOGNIZED BY LUAJIT.

#####  一些在这个补丁使用之前创建的存档可能无法被正确加载。
#####  这是由于一些MODs使用了引擎相关的函数向存档内写入了数据。
#####  这些数据不能被LUAJIT正确解析。

-------------------------------------------------------

##Installation（安装）: 

##### Step 1:
	Copy all files from "DontStarveLuaJIT/bin/" to "[Your Don't Starve Directory]/bin/"

	复制"DontStarveLuaJIT/bin/"目录下的所有文件至"[您的Don't Starve安装目录]/bin/"

##### Step 2:

	Open "[Your Don't Starve Directory]/data/scripts/main.lua" with an text editor.
	Add following lines at the beginning:

	使用文本编辑器打开“[您的Don't Starve安装目录]/data/scripts/main.lua”文件。
	在开头添加如下代码：

```lua
local function unpackex (t, index, limit)
	if (index <= limit) then
		return t[index], unpackex(t, index + 1, limit)
	end
end

unpack = function (t)
	local cnt = 0
	for k in pairs(t) do
		if (type(k) == "number") then
			cnt = k
		end
	end

	return unpackex(t, 1, cnt)
end

```

##### Step 3: (FOR DST，仅针对联机版)
	If it's DST, open "[Your Don't Starve Directory]/data/scripts/util.lua" with an text editor.
	Locate the following lines:

	如果是联机版，还需要使用文本编辑器打开“[您的Don't Starve安装目录]/data/scripts/util.lua”文件。
	定位到如下代码行：	
	

```lua	
function RunInSandboxSafe(untrusted_code, error_handler)
	error_handler = error_handler or function (str) print("Klei, you have missed this line: " .. str) end --<<<<
	if untrusted_code:byte(1) == 27 then return nil, "binary bytecode prohibited" end
	local untrusted_function, message = loadstring(untrusted_code)
	if not untrusted_function then return nil, message end
	setfenv(untrusted_function, {} )
	return xpcall(untrusted_function, error_handler )
end
```
	Add one line at --<<<< mark as above.

	在-<<<<标记处如上所示添加一行代码。
	
	Save util.lua. 

	保存 util.lua 文件。



	If it's DST, open "[Your Don't Starve Directory]/data/scripts/networkclientrpc.lua" with an text editor.
	Locate the following lines (Search 'Generate RPC codes from table of handlers'):

	如果是联机版，还需要使用文本编辑器打开“[您的Don't Starve安装目录]/data/scripts/networkclientrpc.lua”文件。
	定位到如下代码行（搜索“Generate RPC codes from table of handlers”就能找到）：	

```lua
--Generate RPC codes from table of handlers

local i = 1
for k, v in pairs(RPC_HANDLERS) do
    RPC[k] = i
    i = i + 1
end
i = nil
```
	
	Replace them with:

	使用如下的代码替换掉上面的内容：

```lua
--Generate RPC codes from table of handlers
local temp = {}
for k, v in pairs(RPC_HANDLERS) do
    table.insert(temp, k)
end

table.sort(temp)

for k, v in ipairs(temp) do
    RPC[v] = k
end
```
	Save networkclientrpc.lua. 

	保存 networkclientrpc.lua 文件。

##Compile（编译）: 

	The project 'lua51' in solution must be compiled under MSVC9 (Visual Studio 2008) to generate 
	binary-compatible code for dontstarve_steam.exe. To compile luajit, please launch Visual C++ 
	Build Prompt and run msvcbuild.bat.

	为确保与饥荒主程序的二进制兼容性，解决方案中的lua51必须使用MSVC9（即VS2008）来编译。
	如果您要自行编译luajit，请在Visual C++的控制台中运行luajit目录下的msvcbuild.bat。
	
	lua51.dll for DST is build from 'lua51' project with predefined macro 'DST'.
	
	向工程lua51添加前置宏定义'DST'可以编译出针对DST版本的lua51.dll。

##Acknowledgements（致谢）: 

	Great thanks to the following players from Baidu Tieba for testing and suggestions!
	
	感谢百度贴吧以下吧友对于MOD的测试和建议！
	
	风雨凌芸、子恒Clark、359368170、lild100、kkrbdsgc、__PeakChen、o裙下臣o、 LC_1992、
	pikry、沉睡森丶林、可待year、绝世鱼人、王太太平、力玄破、渊_雎、风雪归途、幻想草莓梦、
	sharpwind95
	
