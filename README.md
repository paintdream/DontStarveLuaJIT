# DontStarveLuaJIT for Windows
	LuaJIT for DontStarve (compatible with DS, RoG, SW, DST)

	Tested revisions（已测试版本）:  v181038-v181319 WIN32_STEAM

####  The first test version of DontStarveLuaJIT for Windows 

####  这是DontStarveLuaJIT For Windows 的第一个测试版本！

####  PLEASE BACKUP YOUR SAVES BEFORE APPLYING THIS PATCH. 

####  在启用这个补丁之前，请务必备份你的所有存档。

-------------------------------------------------------

##Installation（安装）: 

##### Step 1:
	Copy all files from "DontStarveLuaJIT/bin/" to "[Your Don't Starve [Together] Directory]/bin/"

	复制"DontStarveLuaJIT/bin/"目录下的所有文件至"[您的Don't Starve [Together] 安装目录]/bin/"


##Compilation（编译）: 

	The following part is for developers only.
	普通玩家止步。这部分内容是给开发者看的。

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
	
