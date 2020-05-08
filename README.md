# DontStarveLuaJIT
	LuaJIT for DontStarve (compatible with DS, RoG, SW, DST for Windows, Dedicated Server for Linux)

####  PLEASE BACKUP YOUR SAVES BEFORE APPLYING THIS PATCH. 

####  在启用这个补丁之前，请务必备份你的所有存档。

All technique details available at: https://zhuanlan.zhihu.com/p/24570361 (Chinese only).

我把[本文所有技术细节、原理](https://zhuanlan.zhihu.com/p/24570361)发在了知乎专栏上，如果对项目有疑问可以先看看文章。

-------------------------------------------------------

## Installation（安装）: 

### Windows

	Copy all files from folder "win" to "[Your Don't Starve [Together] Directory]/bin/"

	复制发布包"win"目录下的所有文件至"[您的Don't Starve [Together] 安装目录]/bin/"
	
### Linux

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
	sharpwind95、乔碧萝、辣椒小皇纸
	
