// Common_local.h
//

#define	MAX_PRINT_MSG_SIZE	4096
#define MAX_WARNING_LIST	256

// writes si_version to the config file - in a kinda obfuscated way
//#define ID_WRITE_VERSION

class idCommonLocal : public idCommon {
public:
	idCommonLocal(void);

	virtual void				Init(int argc, const char** argv, const char* cmdline);
	virtual void				Shutdown(void);
	virtual void				Quit(void);
	virtual bool				IsInitialized(void) const;
	virtual void				Frame(void);
	virtual void				GUIFrame(bool execCmd, bool network);
	virtual void				StartupVariable(const char* match, bool once);
	virtual void				InitTool(const toolFlag_t tool, const idDict* dict);
	virtual void				ActivateTool(bool active);
	virtual bool				InProductionMode(void) { return false; }
	virtual void				WriteConfigToFile(const char* filename);
	virtual void				WriteFlaggedCVarsToFile(const char* filename, int flags, const char* setCmd);
	virtual void				BeginRedirect(char* buffer, int buffersize, void (*flush)(const char*));
	virtual void				EndRedirect(void);
	virtual void				SetRefreshOnPrint(bool set);
	virtual void				Printf(const char* fmt, ...) id_attribute((format(printf, 2, 3)));
	virtual void				VPrintf(const char* fmt, va_list arg);
	virtual void				DPrintf(const char* fmt, ...) id_attribute((format(printf, 2, 3)));
	virtual void				Warning(const char* fmt, ...) id_attribute((format(printf, 2, 3)));
	virtual void				DWarning(const char* fmt, ...) id_attribute((format(printf, 2, 3)));
	virtual void				PrintWarnings(void);
	virtual void				ClearWarnings(const char* reason);
	virtual void				Error(const char* fmt, ...) id_attribute((format(printf, 2, 3)));
	virtual void				FatalError(const char* fmt, ...) id_attribute((format(printf, 2, 3)));
	virtual const idLangDict* GetLanguageDict(void);
	virtual void				Compress(byte* uncompressedMem, byte* compressedMem, int length, int& compressedLength);
	virtual void				Decompress(byte* compressedMem, byte* uncompressedMem, int compressedMemLength, int outputMemLength);

	virtual const char* KeysFromBinding(const char* bind);
	virtual const char* BindingFromKey(const char* key);

	virtual int					ButtonState(int key);
	virtual int					KeyState(int key);

	virtual bool				IsEditorRunning(void);
	virtual void				SetEditorRunning(bool isRunning);
	virtual bool				IsEditorGameRunning(void);
	virtual void				GetEditorGameWindow(int& width, int& height);

	virtual void				BeginLoadScreen(void);
	virtual void				EndLoadScreen(void);

#ifdef ID_DEDICATED
	virtual bool				IsDedicatedServer(void) { return true; }
#else
	virtual bool				IsDedicatedServer(void) { return false; }
#endif

	void						InitGame(void);
	void						ShutdownGame(bool reloading);

	// localization
	void						InitLanguageDict(void);
	void						LocalizeGui(const char* fileName, idLangDict& langDict);
	void						LocalizeMapData(const char* fileName, idLangDict& langDict);
	void						LocalizeSpecificMapData(const char* fileName, idLangDict& langDict, const idLangDict& replaceArgs);

	void						SetMachineSpec(void);

private:
	void						InitCommands(void);
	void						InitRenderSystem(void);
	void						InitSIMD(void);
	bool						AddStartupCommands(void);
	void						ParseCommandLine(int argc, const char** argv);
	void						ClearCommandLine(void);
	bool						SafeMode(void);
	void						CheckToolMode(void);
	void						CloseLogFile(void);
	void						WriteConfiguration(void);
	void						DumpWarnings(void);
	void						LoadGameDLL(void);
	void						UnloadGameDLL(void);
// jmarshall
#ifdef ID_ALLOW_TOOLS
	void						InitImGui(void);
#endif
// jmarshall end
	void						FilterLangList(idStrList* list, idStr lang);

	bool						com_fullyInitialized;
	bool						com_refreshOnPrint;		// update the screen every print for dmap
	int							com_errorEntered;		// 0, ERP_DROP, etc
	bool						com_shuttingDown;

	idFile* logFile;

	char						errorMessage[MAX_PRINT_MSG_SIZE];

	char* rd_buffer;
	int							rd_buffersize;
	void						(*rd_flush)(const char* buffer);

	idStr						warningCaption;
	idStrList					warningList;
	idStrList					errorList;

	INT_PTR						gameDLL;

	idLangDict					languageDict;

	bool						isEditorRunning;

#ifdef ID_WRITE_VERSION
	idCompressor* config_compressor;
#endif
};