# DontStarveLuaJIT for Windows
	LuaJIT for DontStarve (compatible with DS, RoG, SW, DST)

	Tested revisions（已测试版本）:  v181038-v181319 WIN32_STEAM

####  The first test version of DontStarveLuaJIT for Windows 

####  这是DontStarveLuaJIT For Windows 的第一个测试版本！

####  PLEASE BACKUP YOUR SAVES BEFORE APPLYING THIS PATCH. 

####  在启用这个补丁之前，请务必备份你的所有存档。

####  SOME OLD SAVES CANNOT BE LOADED WITH THIS PATCH APPLIED DUE TO EXTRA
####  DATA WRITTEN BY SOME MODS USING ENGINE-RELATED FUNCTIONS (SUCH AS lua_dump).
####  THESE DATA CANNOT BE RECOGNIZED BY LUAJIT.

####  一些在这个补丁使用之前创建的存档可能无法被正确加载。
####  这是由于一些MODs使用了引擎相关的函数向存档内写入了数据。
####  这些数据不能被LUAJIT正确解析。

####  在启用这个补丁之前，请务必备份你的所有存档。

##Installation（安装）: 

##### Step 1:
	Copy all files from "DontStarveLuaJIT/bin/" to "[Your Don't Starve Directory]/bin/"

	复制"DontStarveLuaJIT/bin/"目录下的所有文件至"[您的Don't Starve安装目录]/bin/"



##Build（生成）: 

	The project 'lua51' in solution must be compiled under MSVC9 (Visual Studio 2008) to generate 
	binary-compatible code for dontstarve_steam.exe. To compile luajit, please launch Visual C++ 
	Build Prompt and run msvcbuild.bat.

	为确保与饥荒主程序的二进制兼容性，解决方案中的lua51必须使用MSVC9（即VS2008）来编译。
	如果您要自行编译luajit，请在Visual C++的控制台中运行luajit目录下的msvcbuild.bat。
	
	lua51.dll for DST is build from 'lua51' project with predefined macro 'DST'.
	
	向工程lua51添加前置宏定义'DST'可以编译出针对DST版本的lua51.dll。
