// Bot_char.h
//

#define MAX_CHAR_STATS				256

//
// idBotCharacterStatsManager
//
class idBotCharacterStatsManager {
public:
	idBotCharacterStatsManager();

	// Inits the stats manager.
	void	Init(void);

	// Loads a character file.
	bot_character_t*				BotLoadCharacterFromFile(const char* charfile, int skill);

	// Free character file.
	void							FreeCharacterFile(bot_character_t* ch);

	// Returns the default character stats profile.
	bot_character_t*				GetDefaultCharProfile(void) { return default_char_profile; }

	int								CheckCharacteristicIndex(bot_character_t* ch, int index);
	float							Characteristic_Float(bot_character_t* ch, int index);
	void							Characteristic_String(bot_character_t* ch, int index, char* buf, int size);
	float							Characteristic_BFloat(bot_character_t* ch, int index, float min, float max);
private:
	bot_character_t* AllocBotCharacter(void);

	bot_character_t*				default_char_profile;

	bot_character_t					charStatsList[MAX_CHAR_STATS];
};

extern idBotCharacterStatsManager characterStatsManager;