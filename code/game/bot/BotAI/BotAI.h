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
};

//
// Bot AI Nodes 
//
BOT_AINODE(rvmBotAIBotSeekLTG, botAIActionSeekLTG);
BOT_AINODE(rvmBotAIBotRespawn, botAIRespawn);