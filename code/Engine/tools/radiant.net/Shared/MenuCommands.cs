#if RADIANT_NET
namespace radiant.net.Shared
{
    class MenuCommands
    {
        public enum RadiantNetMenuCommands_t {
#else
        enum RadiantNetMenuCommands_t {
#endif
            // File Menu
            RADIANT_NET_COMMAND_NEW = 0,
            RADIANT_NET_COMMAND_OPEN,
            RADIANT_NET_COMMAND_SAVE,
            RADIANT_NET_COMMAND_SAVEAS,
            RADIANT_NET_COMMAND_SAVESELECTION,
            RADIANT_NET_COMMAND_SAVEREGION,
            RADIANT_NET_COMMAND_NEWPROJECT,
            RADIANT_NET_COMMAND_LOADPROJECT,
            RADIANT_NET_COMMAND_PROJECTSETTINGS,
            RADIANT_NET_COMMAND_POINTFILE,
            RADIANT_NET_COMMAND_EXIT

            // Edit Menu
        };
#if RADIANT_NET
    }
}

#endif