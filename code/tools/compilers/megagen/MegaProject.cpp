// MegaProject.cpp
//

#include "precompiled.h"

#include "MegaGen.h"

/*
======================
rvmMegaProject::rvmMegaProject
======================
*/
rvmMegaProject::rvmMegaProject()
{

}

/*
======================
rvmMegaProject::~rvmMegaProject
======================
*/
rvmMegaProject::~rvmMegaProject()
{
	for (int i = 0; i < megaLayers.Num(); i++)
	{
		delete megaLayers[i];
	}
	megaLayers.Clear();
}

/*
======================
rvmMegaProject::ParseLayer
======================
*/
bool rvmMegaProject::ParseLayer(MegaLayer *layer, idStr &layerStr) {
	idParser parser;
	idToken token;

	if (!parser.LoadMemory(layerStr, layerStr.Size(), "MegaLayer"))
		return false;

	parser.SetFlags(DECL_LEXER_FLAGS);

	if (!parser.ExpectTokenString("{")) {
		common->Warning("MegaLayer expected opening {");
		return false;
	}

	// Parse until EOF
	while (true)
	{
		if (parser.EndOfFile())
		{
			common->Warning("Unexpected EOF in MegaLayer!");
			return false;
		}

		parser.ReadToken(&token);

		if (token == "}")
		{
			break;
		}

		// Load in the albedo image for this layer.
		if (token == "albedo")
		{
			parser.ReadToken(&token);
			R_LoadTGA(token, &layer->albedoImage.data, &layer->albedoImage.width, &layer->albedoImage.height, nullptr);
			if (layer->albedoImage.data == NULL)
			{
				common->Warning("Failed to load albedo image %s\n", token.c_str());
				return false;
			}
		}
		else if (token == "mask")
		{
			parser.ReadToken(&token);
			R_LoadTGA(token, &layer->maskImage.data, &layer->maskImage.width, &layer->maskImage.height, nullptr);
			if (layer->maskImage.data == NULL)
			{
				common->Warning("Failed to load albedo image %s\n", token.c_str());
				return false;
			}
		}
		else
		{
			common->Warning("While parsing layer, unknown token %s\n", token.c_str());
			return false;
		}
	}

	return true;
}

/*
======================
rvmMegaProject::ParseProject
======================
*/
bool rvmMegaProject::ParseProject(idParser &parser) {
	int numLayers;

	// Check how many layers we have.
	if (!parser.ExpectTokenString("numlayers"))
	{
		parser.Warning("Expected numlayers token");
		return false;
	}
	numLayers = parser.ParseInt();

	// Parse the layers.
	for (int i = 0; i < numLayers; i++)
	{
		MegaLayer *megaLayer = new MegaLayer();
		idStr layerStr;

		// Check the layer name.
		if (!parser.ExpectTokenString(va("layer_%d", i)))
		{
			parser.Warning("Layer name invalid or unexpected token!");
			return false;
		}

		// Extract the text in the braced section.
		parser.ParseBracedSectionExact(layerStr);

		// Parse the layer.
		if (!ParseLayer(megaLayer, layerStr))
		{
			parser.Warning("Failed to parse megalayer");
			return false;
		}

		// Add the layer to the list.
		megaLayers.Append(megaLayer);
	}

	return true;
}