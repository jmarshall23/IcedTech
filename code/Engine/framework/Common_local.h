// Common_local.h
//

#define	MAX_PRINT_MSG_SIZE	4096
#define MAX_WARNING_LIST	256

// writes si_version to the config file - in a kinda obfuscated way
//#define ID_WRITE_VERSION

//
// rvmNetworkType_t
//
enum rvmNetworkType_t {
	NETWORK_TYPE_NONE = 0,
	NETWORK_TYPE_SERVER,
	NETWORK_TYPE_CLIENT
};

struct rvmNetworkServerState_t;
struct rvmNetworkClient_t;
struct rvmNetworkServer_t;

//
// rvmNetworkPacketQueue_t
//
class rvmNetworkPacketQueue_t{
public:
	rvmNetworkPacketQueue_t(const idBitMsg& msg);
	~rvmNetworkPacketQueue_t();

	int clientNum;
	idBitMsg _msg;	

	idListNode<rvmNetworkPacketQueue_t> listNode;
private:
	byte* packetData;
	int packetDataLen;
};

/*
==================
rvmNetworkPacketQueue_t::rvmNetworkPacketQueue_t
==================
*/
ID_INLINE rvmNetworkPacketQueue_t::rvmNetworkPacketQueue_t(const idBitMsg &msg) {
	clientNum = -1;
	packetData = new byte[msg.GetSize()];
	packetDataLen = msg.GetSize();

	memcpy(packetData, msg.GetData(), packetDataLen);

	_msg.Init(packetData, packetDataLen);
	_msg.SetSize(packetDataLen);
}

ID_INLINE rvmNetworkPacketQueue_t::~rvmNetworkPacketQueue_t() {
	clientNum = -1;
	if(packetData != NULL) {
		delete packetData;
	}
}


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

	virtual void				ServerSetUserCmdForClient(int clientNum, struct usercmd_t& cmd);

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

	virtual int					AllocateClientSlotForBot(int maxPlayersOnServer);
	virtual int					ServerSetBotUserCommand(int clientNum, int frameNum, const usercmd_t& cmd);
	virtual void				ServerSendReliableMessageExcluding(int clientNum, const idBitMsg& msg);
	virtual int					ServerSetBotUserName(int clientNum, const char* playerName);
	virtual void				ServerSendReliableMessage(int clientNum, const idBitMsg& msg);
	virtual void				ClientSendReliableMessage(const idBitMsg& msg);
	virtual int					ClientGetTimeSinceLastPacket(void);
	virtual float				Get_com_engineHz_latched(void);
	virtual int64_t				Get_com_engineHz_numerator(void);
	virtual int64_t				Get_com_engineHz_denominator(void);
	virtual void				ExecuteClientMapChange(const char* mapName, const char* gameType);

#ifdef ID_DEDICATED
	virtual bool				IsDedicatedServer(void) { return true; }
#else
	virtual bool				IsDedicatedServer(void) { return false; }
#endif
	virtual bool				IsClient(void);
	virtual bool				IsServer(void);
	virtual bool				IsMultiplayer(void);
	virtual void				DisconnectFromServer(void);
	virtual void				KillServer(void);
	virtual int					ServerGetClientTimeSinceLastPacket(int clientNum);
	virtual int					ServerGetClientPing(int clientNum);
	virtual int					GetLocalClientNum(void);

	virtual int					GetGameFrame(void) { return gameFrame; }

	virtual void				SetUserInfoKey(int clientNum, const char* key, const char* value);
	virtual void				BindUserInfo(int clientNum);

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

// New network code.
private:
	void						InitNetwork(void);
	void						ShutdownNetwork(void);

	static void					SpawnServer_f(const idCmdArgs& args);
	static void					Connect_f(const idCmdArgs& args);
	void						SpawnServer(void);
	void						NetworkFrame(int numGameFrames);
	void						ConnectToServer(const char* ip);

	void						NewClient(int clientNum, void* peer);

	rvmNetworkType_t			networkType;
	rvmNetworkServerState_t*	serverState;

	idList<rvmNetworkClient_t*> networkClients;
	rvmNetworkServer_t*			networkServer;

	idLinkedList<rvmNetworkPacketQueue_t, &rvmNetworkPacketQueue_t::listNode> serverPacketQueue;
	idLinkedList<rvmNetworkPacketQueue_t, &rvmNetworkPacketQueue_t::listNode> clientPacketQueue;

	int							localClientNum;

	usercmd_t*					userCmds;

	int							gameFrame;			// Frame number of the local game
	double						gameTimeResidual;	// left over msec from the last game frame
};

extern idCommonLocal			commonLocal;