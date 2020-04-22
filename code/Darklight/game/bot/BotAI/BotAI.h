// BotAI.h
//

//
// BOT_AINODE - creates a new bot ai node class.
//
#define BOT_AINODE(classname, varname) class classname : public rvmBotAIBotActionBase { \
								virtual void Think(bot_state_t* bs); \
								virtual const char*			GetName() { return #classname; } \
							  }; \
							  extern classname varname;							

//
// rvmBotAIBotActionBase
//
class rvmBotAIBotActionBase {
public:
	virtual void				Think(bot_state_t* bs) = 0;
	virtual const char*			GetName() = 0;
protected:
	bool		BotIsDead(bot_state_t* bs);
	bool		BotReachedGoal(bot_state_t* bs, bot_goal_t* goal);
	int			BotGetItemLongTermGoal(bot_state_t* bs, int tfl, bot_goal_t* goal);
	void		BotChooseWeapon(bot_state_t* bs);
	int			BotFindEnemy(bot_state_t* bs, int curenemy);
	bool		EntityIsDead(idEntity* entity);
	float		BotEntityVisible(int viewer, idVec3 eye, idAngles viewangles, float fov, int ent);
	float		BotEntityVisibleTest(int viewer, idVec3 eye, idAngles viewangles, float fov, int ent, bool allowHeightTest);
	void		BotUpdateBattleInventory(bot_state_t* bs, int enemy);
	float		BotAggression(bot_state_t* bs);
	int			BotWantsToRetreat(bot_state_t* bs);
	void		BotBattleUseItems(bot_state_t* bs);
	void		BotAimAtEnemy(bot_state_t* bs);
	void		BotCheckAttack(bot_state_t* bs);
	bool		BotWantsToChase(bot_state_t* bs);
	int			BotNearbyGoal(bot_state_t* bs, int tfl, bot_goal_t* ltg, float range);
	void		BotGetRandomPointNearPosition(idVec3 point, idVec3 &randomPoint, float radius);
	int			BotMoveInRandomDirection(bot_state_t* bs);
	void		BotAttackMove(bot_state_t* bs, int tfl);
	void		BotMoveToGoal(bot_state_t* bs, bot_goal_t* goal);
	int			BotMoveInDirection(bot_state_t* bs, idVec3 dir, float speed, int type);
public:
	static int	WP_MACHINEGUN;
	static int	WP_SHOTGUN;
	static int	WP_PLASMAGUN;
	static int	WP_ROCKET_LAUNCHER;
};

//
// Bot AI Nodes 
//
BOT_AINODE(rvmBotAIBotSeekLTG, botAIActionSeekLTG);
BOT_AINODE(rvmBotAIBotRespawn, botAIRespawn);
BOT_AINODE(rvmBotAIBotBattleRetreat, botAIBattleRetreat);
BOT_AINODE(rvmBotAIBotBattleFight, botAIBattleFight);
BOT_AINODE(rvmBotAIBotBattleChase, botAIBattleChase);
BOT_AINODE(rvmBotAIBotBattleNBG, botAIBattleNBG);
BOT_AINODE(rvmBotAIBotSeekNBG, botAISeekNBG);