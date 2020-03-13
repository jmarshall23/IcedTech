#include <dlfcn.h> //dlopen
#include <cstdio> //printf

#define DLL_NAME "./libDoomDLL.so"

typedef int (*DoomMainFn)( int, const char ** );

int main( int argc, const char **argv )
{
    DoomMainFn DoomMain;
    void *doomdll = dlopen(DLL_NAME, RTLD_NOW );
    if( !doomdll ){
        printf("[Launcher]Couldn't open the Doom dll! (%s) - error(%s)\n", DLL_NAME, dlerror());
        return 1;
    }

    DoomMain = (DoomMainFn)dlsym(doomdll, "DoomMain");

    if( !DoomMain ){
        printf("[Launcher]Couldn't find the DoomMain symbol in Doom dll! - error(%s)\n", dlerror());
        return 2;
    }

    return DoomMain( argc, argv );
}
