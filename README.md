# DontStarveLuaJIT
	LuaJIT for DontStarve (compatible with DS, RoG, SW, DST for Windows, Dedicated Server for Linux)

####  PLEASE BACKUP YOUR SAVES BEFORE APPLYING THIS PATCH. 

####  在启用这个补丁之前，请务必备份你的所有存档。

All technique details available at: https://zhuanlan.zhihu.com/p/24570361 (Chinese only).

我把[本文所有技术细节、原理](https://zhuanlan.zhihu.com/p/24570361)发在了知乎专栏上，如果对项目有疑问可以先看看文章。

-------------------------------------------------------

## Installation/Uninstallation（安装）: 

### Windows

	Copy all files from folder "win" to "[Your Don't Starve [Together] Directory]/bin/"
	复制发布包"win"目录下的所有文件至"[您的Don't Starve [Together] 安装目录]/bin/"
	
	Launch the game, press ` and type:
	启动游戏，按`打开控制台，输入：
	
	print(_VERSION)
	
	And you can see message started with "LuaJIT".
	你可以看到LuaJIT字样的输出，这就说明已经安装成功。

	If you want uninstall this patch, just remove these copied files (i.e. WINMM.DLL, lua51.dll, lua51DS.dll) from the bin folder.
	如果需要卸载本Patch，只需要删掉您这些新复制的文件（WINMM.DLL, lua51.DLL, lua51DS.dll）即可。
	
### Linux
	It's highly recommended to compile the release yourself. See the section 'Compilation' below.
	强烈建议自行编译linux版本，具体操作方式请看下文“编译”一节。

	Copy all files from folder "linux" to "[Your Don't Starve [Together] Directory]/bin/lib32/", start game with the following environment setting:
	复制发布包"linux"目录下的所有文件至"[您的Don't Starve [Together] 安装目录]/bin/lib32"，使用如下环境参数启动游戏：
	
	export LD_PRELOAD=libpreload.so

	NOTICE: libpreload.so/liblua51.so/liblua51DS.so may rely on some C-runtime libraries, which are not present on your computer.
	Try to use ldd to find dependencies and install them.
	注意，由于Linux平台的特殊性，发行包中提供的libpreload.so、liblua51.so、liblua51DS.so可能无法直接在您的计算机上执行。
	如果您遇到安装失败，则一般是由于liblua51DS.so的依赖没有安装导致的。您可以使用ldd命令来查看缺少哪些依赖并安装它们，比如：

	ldd liblua51DS.so
	
	For example, if 'readline' was reported missing, then you could have it installed by following instructions(Debian/Ubuntu):
	比如提示readline找不到，那么就使用如下命令安装(Debian/Ubuntu)：
	
	sudo apt-get install lib32readline6
	
	If your system are suggrested to install newer version of lib32realine, e.g. lib32readline8. Making a soft link of it to could work.
	如果您的系统比较新（如Debian buster），可能只能安装高版本的lib32readline8，您可以制作一个软链接到lib32readline6来使用。
	
	ln -s /lib32/libreadline.so.8 /lib32/libreadline.so.6
	

## Compilation（编译）: 

	The following part is for developers only.
	**普通玩家止步。这部分内容是给开发者看的。**

### Windows

	For Windows version, the project 'lua51' in solution must be compiled with MSVC9 (Visual Studio 2008) to generate 
	binary-compatible code for dontstarve_steam.exe. To compile luajit, please launch Visual C++ 
	Build Prompt and run msvcbuild.bat.
	对于Windows版本，为确保与饥荒主程序的二进制兼容性，解决方案中的lua51必须使用MSVC9（即VS2008）来编译。
	如果您要自行编译luajit，请在Visual C++的控制台中运行luajit目录下的msvcbuild.bat。
	
### Linux

	For linux version, the project 'lua51' must be compiled with GCC 4.8.4:i386, which is builtin compiler tool of Ubuntu 14.04 LTS.
	对于Linux版本，lua51工程需要使用GCC 4.8.4:i386编译。您可以安装Ubuntu 14.04 LTS版本，使用自带的GCC即可。

## Acknowledgements（致谢）: 

	Great thanks to the following players from Baidu Tieba for testing and suggestions!
	感谢百度贴吧以下吧友对于MOD的测试和建议！
	
	风雨凌芸、子恒Clark、359368170、lild100、kkrbdsgc、__PeakChen、o裙下臣o、 LC_1992、
	pikry、沉睡森丶林、可待year、绝世鱼人、王太太平、力玄破、渊_雎、风雪归途、幻想草莓梦、
	sharpwind95、乔碧萝、辣椒小皇纸、☆风铃草☆
	

## TO MOD DEVELOPERS（对于MOD开发者）：

	There are some differences in debug library (including setfenv/getfenv) and cannot be simply fixed. Please avoid calling them with tail call because LuaJIT will not count tail call as individual call. As a result, the layer of debugging stack info messed up:
	由于LuaJIT的debug函数库（以及setfenv/getfenv）与Lua的行为有所不同，因此一些依赖debug库的mod可能无法正确运行。由于部分debug函数在涉及函数层次的时候，LuaJIT的实现不会记入尾调用所在函数的层次，因而与原版Lua出现偏差。由于GemCore用途较广，因而特此在GemCore加载时做了特殊处理，使得它可以正确运行。luajit不会记录每一个栈帧上尾调用的次数，因而完美的解决方案比较困难。对于其他使用了debug库的mod而言，建议MOD作者不要使用尾调用来调用debug库的函数，例如如下代码：

	function a(option)
		return debug.getinfo(1, "option")
	end

	Just append a 'nil' while calling them so they won't be a tailcall.
	建议在调用时对返回值序列加一个nil来避免luajit走入尾调用的逻辑：

	function a(option)
		return debug.getinfo(1, "option"), nil
	end

	Special thanks to ☆风铃草☆ for locating this problem.
	特别感谢☆风铃草☆帮助查找和定位本问题~
