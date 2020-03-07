# IcedTech
New idTech 4 engine with virtual texturing, recast navmesh, CaS, GLSL, Radiant.NET and more!

Darklight Arena:
Going forward IcedTech is not compatible with Doom 3 vanilla singleplayer content, soon it will be a standalone game. We are using recast navmesh for our bots, and all the old
AI and AAS code has been removed. If you want to use the version of IcedTech that is compatible with Doom 3, use our Doom 3 compatible branch https://github.com/jmarshall23/IcedTech/tree/Doom-3-Vanilla-Compatible

Radaint.NET
If you are having trouble compiling radiant.net in Visual Studio 2019, please install redist/BuildTools_Full.exe or you can download it from https://www.microsoft.com/en-us/download/confirmation.aspx?id=48159

AUDIO:
This codebase uses the Doom 3 BFG audio system. To get audio in Doom 3 retail, first extract pak003 to your base folder, open up sound/convert.bat and fix the file paths. Then run convert.bat and wait about 10 minutes :). This will be fixed in a future release. 
