// ExportOptions.cpp
//

#include "maya_precompiled.h"
#include "maya_main.h"
#include "ExportOptions.h"

#define DEFAULT_ANIM_EPSILON	0.125f
#define DEFAULT_QUAT_EPSILON	( 1.0f / 8192.0f )

/*
==============================================================================================
	idExportOptions
==============================================================================================
*/

/*
====================
idExportOptions::Reset
====================
*/
void idExportOptions::Reset(const char* commandline) {
	scale = 1.0f;
	type = WRITE_MESH;
	startframe = -1;
	endframe = -1;
	ignoreMeshes = false;
	clearOrigin = false;
	clearOriginAxis = false;
	framerate = 24;
	align = "";
	rotate = 0.0f;
	commandLine = commandline;
	prefix = "";
	jointThreshold = 0.05f;
	ignoreScale = false;
	xyzPrecision = DEFAULT_ANIM_EPSILON;
	quatPrecision = DEFAULT_QUAT_EPSILON;
	cycleStart = -1;

	src.Clear();
	dest.Clear();

	tokens.SetTokens(commandline);

	keepjoints.Clear();
	renamejoints.Clear();
	remapjoints.Clear();
	exportgroups.Clear();
	skipmeshes.Clear();
	keepmeshes.Clear();
	groups.Clear();
}

/*
====================
idExportOptions::idExportOptions
====================
*/
idExportOptions::idExportOptions(const char* commandline, const char* ospath) {
	idStr		token;
	idNamePair	joints;
	int			i;
	idAnimGroup* group;
	idStr		sourceDir;
	idStr		destDir;

	Reset(commandline);

	token = tokens.NextToken("Missing export command");
	if (token == "mesh") {
		type = WRITE_MESH;
	}
	else if (token == "anim") {
		type = WRITE_ANIM;
	}
	else if (token == "camera") {
		type = WRITE_CAMERA;
	}
	else {
		common->Error("Unknown export command '%s'", token.c_str());
	}

	src = tokens.NextToken("Missing source filename");
	dest = src;

	for (token = tokens.NextToken(); token != ""; token = tokens.NextToken()) {
		if (token == "-force") {
			// skip
		}
		else if (token == "-game") {
			// parse game name
			game = tokens.NextToken("Expecting game name after -game");

		}
		else if (token == "-rename") {
			// parse joint to rename
			joints.from = tokens.NextToken("Missing joint name for -rename.  Usage: -rename [joint name] [new name]");
			joints.to = tokens.NextToken("Missing new name for -rename.  Usage: -rename [joint name] [new name]");
			renamejoints.Append(joints);

		}
		else if (token == "-prefix") {
			prefix = tokens.NextToken("Missing name for -prefix.  Usage: -prefix [joint prefix]");
			// jmarshall
		}
		else if (token == "-material_prefix") {
			material_prefix = tokens.NextToken("Missing name for -material_prefix");
			// jmarshall end
		}
		else if (token == "-parent") {
			// parse joint to reparent
			joints.from = tokens.NextToken("Missing joint name for -parent.  Usage: -parent [joint name] [new parent]");
			joints.to = tokens.NextToken("Missing new parent for -parent.  Usage: -parent [joint name] [new parent]");
			remapjoints.Append(joints);

		}
		else if (!token.Icmp("-sourcedir")) {
			// parse source directory
			sourceDir = tokens.NextToken("Missing filename for -sourcedir.  Usage: -sourcedir [directory]");

		}
		else if (!token.Icmp("-destdir")) {
			// parse destination directory
			destDir = tokens.NextToken("Missing filename for -destdir.  Usage: -destdir [directory]");

		}
		else if (token == "-dest") {
			// parse destination filename
			dest = tokens.NextToken("Missing filename for -dest.  Usage: -dest [filename]");

		}
		else if (token == "-range") {
			// parse frame range to export
			token = tokens.NextToken("Missing start frame for -range.  Usage: -range [start frame] [end frame]");
			startframe = atoi(token);
			token = tokens.NextToken("Missing end frame for -range.  Usage: -range [start frame] [end frame]");
			endframe = atoi(token);

			if (startframe > endframe) {
				common->Error("Start frame is greater than end frame.");
			}

		}
		else if (!token.Icmp("-cycleStart")) {
			// parse start frame of cycle
			token = tokens.NextToken("Missing cycle start frame for -cycleStart.  Usage: -cycleStart [first frame of cycle]");
			cycleStart = atoi(token);

		}
		else if (token == "-scale") {
			// parse scale
			token = tokens.NextToken("Missing scale amount for -scale.  Usage: -scale [scale amount]");
			scale = atof(token);

		}
		else if (token == "-align") {
			// parse align joint
			align = tokens.NextToken("Missing joint name for -align.  Usage: -align [joint name]");

		}
		else if (token == "-rotate") {
			// parse angle rotation
			token = tokens.NextToken("Missing value for -rotate.  Usage: -rotate [yaw]");
			rotate = -atof(token);

		}
		else if (token == "-nomesh") {
			ignoreMeshes = true;

		}
		else if (token == "-clearorigin") {
			clearOrigin = true;
			clearOriginAxis = true;

		}
		else if (token == "-clearoriginaxis") {
			clearOriginAxis = true;

		}
		else if (token == "-ignorescale") {
			ignoreScale = true;

		}
		else if (token == "-xyzprecision") {
			// parse quaternion precision
			token = tokens.NextToken("Missing value for -xyzprecision.  Usage: -xyzprecision [precision]");
			xyzPrecision = atof(token);
			if (xyzPrecision < 0.0f) {
				common->Error("Invalid value for -xyzprecision.  Must be >= 0");
			}

		}
		else if (token == "-quatprecision") {
			// parse quaternion precision
			token = tokens.NextToken("Missing value for -quatprecision.  Usage: -quatprecision [precision]");
			quatPrecision = atof(token);
			if (quatPrecision < 0.0f) {
				common->Error("Invalid value for -quatprecision.  Must be >= 0");
			}

		}
		else if (token == "-jointthreshold") {
			// parse joint threshold
			token = tokens.NextToken("Missing weight for -jointthreshold.  Usage: -jointthreshold [minimum joint weight]");
			jointThreshold = atof(token);

		}
		else if (token == "-skipmesh") {
			token = tokens.NextToken("Missing name for -skipmesh.  Usage: -skipmesh [name of mesh to skip]");
			skipmeshes.AddUnique(token);

		}
		else if (token == "-keepmesh") {
			token = tokens.NextToken("Missing name for -keepmesh.  Usage: -keepmesh [name of mesh to keep]");
			keepmeshes.AddUnique(token);

		}
		else if (token == "-jointgroup") {
			token = tokens.NextToken("Missing name for -jointgroup.  Usage: -jointgroup [group name] [joint1] [joint2]...[joint n]");
			group = groups.Ptr();
			for (i = 0; i < groups.Num(); i++, group++) {
				if (group->name == token) {
					break;
				}
			}

			if (i >= groups.Num()) {
				// create a new group
				group = &groups.Alloc();
				group->name = token;
			}

			while (tokens.TokenAvailable()) {
				token = tokens.NextToken();
				if (token[0] == '-') {
					tokens.UnGetToken();
					break;
				}

				group->joints.AddUnique(token);
			}
		}
		else if (token == "-group") {
			// add the list of groups to export (these don't affect the hierarchy)
			while (tokens.TokenAvailable()) {
				token = tokens.NextToken();
				if (token[0] == '-') {
					tokens.UnGetToken();
					break;
				}

				group = groups.Ptr();
				for (i = 0; i < groups.Num(); i++, group++) {
					if (group->name == token) {
						break;
					}
				}

				if (i >= groups.Num()) {
					common->Error("Unknown group '%s'", token.c_str());
				}

				exportgroups.AddUnique(group);
			}
		}
		else if (token == "-keep") {
			// add joints that are kept whether they're used by a mesh or not
			while (tokens.TokenAvailable()) {
				token = tokens.NextToken();
				if (token[0] == '-') {
					tokens.UnGetToken();
					break;
				}
				keepjoints.AddUnique(token);
			}
		}
		else {
			common->Error("Unknown option '%s'", token.c_str());
		}
	}

	token = src;
	src = ospath;
	src.BackSlashesToSlashes();
	src.AppendPath(sourceDir);
	src.AppendPath(token);

	token = dest;
	dest = ospath;
	dest.BackSlashesToSlashes();
	dest.AppendPath(destDir);
	dest.AppendPath(token);

	// Maya only accepts unix style path separators
	src.BackSlashesToSlashes();
	dest.BackSlashesToSlashes();

	if (skipmeshes.Num() && keepmeshes.Num()) {
		common->Error("Can't use -keepmesh and -skipmesh together.");
	}
}

/*
====================
idExportOptions::jointInExportGroup
====================
*/
bool idExportOptions::jointInExportGroup(const char* jointname) {
	int			i;
	int			j;
	idAnimGroup* group;

	if (!exportgroups.Num()) {
		// if we don't have any groups specified as export then export every joint
		return true;
	}

	// search through all exported groups to see if this joint is exported
	for (i = 0; i < exportgroups.Num(); i++) {
		group = exportgroups[i];
		for (j = 0; j < group->joints.Num(); j++) {
			if (group->joints[j] == jointname) {
				return true;
			}
		}
	}

	return false;
}
