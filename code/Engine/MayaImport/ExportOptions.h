// ExportOptions.h
//

typedef enum {
	WRITE_MESH,
	WRITE_ANIM,
	WRITE_CAMERA
} exportType_t;

/*
==============================================================================================
	idTokenizer
==============================================================================================
*/

class idTokenizer {
private:
	int					currentToken;
	idStrList			tokens;

public:
	idTokenizer() { Clear(); };
	void				Clear(void) { currentToken = 0;	tokens.Clear(); };

	int					SetTokens(const char* buffer);
	const char* NextToken(const char* errorstring = NULL);

	bool				TokenAvailable(void) { return currentToken < tokens.Num(); };
	int					Num(void) { return tokens.Num(); };
	void				UnGetToken(void) { if (currentToken > 0) { currentToken--; } };
	const char* GetToken(int index) { if ((index >= 0) && (index < tokens.Num())) { return tokens[index]; } else { return NULL; } };
	const char* CurrentToken(void) { return GetToken(currentToken); };
};

/*
====================
idTokenizer::SetTokens
====================
*/
ID_INLINE int idTokenizer::SetTokens(const char* buffer) {
	const char* cmd;

	Clear();

	// tokenize commandline
	cmd = buffer;
	while (*cmd) {
		// skip whitespace
		while (*cmd && isspace(*cmd)) {
			cmd++;
		}

		if (!*cmd) {
			break;
		}

		idStr& current = tokens.Alloc();
		while (*cmd && !isspace(*cmd)) {
			current += *cmd;
			cmd++;
		}
	}

	return tokens.Num();
}

/*
====================
idTokenizer::NextToken
====================
*/
ID_INLINE const char* idTokenizer::NextToken(const char* errorstring) {
	if (currentToken < tokens.Num()) {
		return tokens[currentToken++];
	}

	if (errorstring) {
		common->Error("Error: %s", errorstring);
	}

	return NULL;
}

/*
==============================================================================================
	idExportOptions
==============================================================================================
*/

class idNamePair {
public:
	idStr	from;
	idStr	to;
};

class idAnimGroup {
public:
	idStr		name;
	idStrList	joints;
};

class idExportOptions {
private:
	idTokenizer				tokens;

	void					Reset(const char* commandline);

public:
	idStr					commandLine;
	idStr					src;
	idStr					dest;
	idStr					game;
	idStr					prefix;
	// jmarshall
	idStr					material_prefix;
	// jmarshall end
	float					scale;
	exportType_t			type;
	bool					ignoreMeshes;
	bool					clearOrigin;
	bool					clearOriginAxis;
	bool					ignoreScale;
	int						startframe;
	int						endframe;
	int						framerate;
	float					xyzPrecision;
	float					quatPrecision;
	idStr					align;
	idList<idNamePair>		renamejoints;
	idList<idNamePair>		remapjoints;
	idStrList				keepjoints;
	idStrList				skipmeshes;
	idStrList				keepmeshes;
	idList<idAnimGroup*>	exportgroups;
	idList<idAnimGroup>		groups;
	float					rotate;
	float					jointThreshold;
	int						cycleStart;

	idExportOptions(const char* commandline, const char* ospath);

	bool					jointInExportGroup(const char* jointname);
};
