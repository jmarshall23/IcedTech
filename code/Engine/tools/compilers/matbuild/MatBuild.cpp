// MatBuild.cpp
//

#include "tools_precompiled.h"

/*
================
MatBuild_f
================
*/
void MatBuild_f(const idCmdArgs& args) {
	if (args.Argc() < 2) {
		common->Warning("Usage: matbuild <folder>\n");
		return;
	}

	idFileList *fileList = fileSystem->ListFiles(va("textures/%s", args.Argv(1)), ".png");
	idStr mtrBuffer;

	idStrList list = fileList->GetList();
	for(int i = 0; i < fileList->GetNumFiles(); i++)
	{
		idStr pngpath = list[i];

		if(strstr(pngpath.c_str(), "_normal")) {
			continue;
		}

		if (strstr(pngpath.c_str(), "_spec")) {
			continue;
		}

		pngpath = pngpath.StripFileExtension();

		mtrBuffer += va("textures/%s/%s\n", args.Argv(1), pngpath.c_str());
		mtrBuffer += va("{\n");
		mtrBuffer += va("\tqer_editorimage textures/%s/%s.png\n", args.Argv(1), pngpath.c_str());
		mtrBuffer += va("\tdiffusemap textures/%s/%s.png\n", args.Argv(1), pngpath.c_str());
		mtrBuffer += va("\tbumpmap textures/%s/%s_normal.png\n", args.Argv(1), pngpath.c_str());
		mtrBuffer += va("\tspecularmap textures/%s/%s_spec.png\n", args.Argv(1), pngpath.c_str());
		mtrBuffer += va("}\n");
	}

	fileSystem->FreeFileList(fileList);

	idStr mtrName = args.Argv(1);
	fileSystem->WriteFile(va("materials/%s_built.mtr", mtrName.c_str()), mtrBuffer.c_str(), mtrBuffer.Length());
}