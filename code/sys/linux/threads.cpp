/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 GPL Source Code ("Doom 3 Source Code").

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include <SDL2/SDL_version.h>
#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_timer.h>

#include <idlib/precompiled.h>
#include <framework/Engine_precompiled.h>
#include <sys/posix/posix_public.h>
#include "sys/sys_public.h"
#include "framework/Common.h"

#include <unistd.h>
#include <fcntl.h>

static SDL_mutex	*mutex[MAX_CRITICAL_SECTIONS] = { };
static SDL_cond		*cond[MAX_TRIGGER_EVENTS] = { };
static bool			signaled[MAX_TRIGGER_EVENTS] = { };
static bool			waiting[MAX_TRIGGER_EVENTS] = { };

#define MAX_THREADS 10

static xthreadInfo	*thread[MAX_THREADS] = { };
static size_t		thread_count = 0;

/*
==============
Sys_Sleep
==============
*/
void Sys_Sleep(int msec) {
	SDL_Delay(msec);
}

/*
================
Sys_Milliseconds
================
*/
int Sys_Milliseconds() {
	return SDL_GetTicks();
}

/*
==================
Sys_InitThreads
==================
*/
void Sys_InitThreads() {
	// critical sections
	for (int i = 0; i < MAX_CRITICAL_SECTIONS; i++) {
		mutex[i] = SDL_CreateMutex();

		if (!mutex[i]) {
			Sys_Printf("ERROR: SDL_CreateMutex failed\n");
			return;
		}
	}

	// events
	for (int i = 0; i < MAX_TRIGGER_EVENTS; i++) {
		cond[i] = SDL_CreateCond();

		if (!cond[i]) {
			Sys_Printf("ERROR: SDL_CreateCond failed\n");
			return;
		}

		signaled[i] = false;
		waiting[i] = false;
	}

	// threads
	for (int i = 0; i < MAX_THREADS; i++)
		thread[i] = NULL;

	thread_count = 0;
}

/*
==================
Sys_ShutdownThreads
==================
*/

void Sys_ShutdownThreads() {
	// threads
	for (int i = 0; i < MAX_THREADS; i++) {
		if (!thread[i])
			continue;

		Sys_Printf("WARNING: Thread '%s' still running\n", thread[i]->name);
#if SDL_VERSION_ATLEAST(2, 0, 0)
		// TODO no equivalent in SDL2
#else
		SDL_KillThread(thread[i]->threadHandle);
#endif
		thread[i] = NULL;
	}

	// events
	for (int i = 0; i < MAX_TRIGGER_EVENTS; i++) {
		SDL_DestroyCond(cond[i]);
		cond[i] = NULL;
		signaled[i] = false;
		waiting[i] = false;
	}

	// critical sections
	for (int i = 0; i < MAX_CRITICAL_SECTIONS; i++) {
		SDL_DestroyMutex(mutex[i]);
		mutex[i] = NULL;
	}
}

/*
==================
Sys_EnterCriticalSection
==================
*/
void Sys_EnterCriticalSection(int index) {
	assert(index >= 0 && index < MAX_CRITICAL_SECTIONS);

	if (SDL_LockMutex(mutex[index]) != 0)
		common->Error("ERROR: SDL_LockMutex failed (%s)\n", SDL_GetError());
}

/*
==================
Sys_LeaveCriticalSection
==================
*/
void Sys_LeaveCriticalSection(int index) {
	assert(index >= 0 && index < MAX_CRITICAL_SECTIONS);

	if (SDL_UnlockMutex(mutex[index]) != 0)
		common->Error("ERROR: SDL_UnlockMutex failed (%s)\n", SDL_GetError());
}

/*
======================================================
wait and trigger events
we use a single lock to manipulate the conditions, CRITICAL_SECTION_SYS

the semantics match the win32 version. signals raised while no one is waiting stay raised until a wait happens (which then does a simple pass-through)

NOTE: we use the same mutex for all the events. I don't think this would become much of a problem
cond_wait unlocks atomically with setting the wait condition, and locks it back before exiting the function
the potential for time wasting lock waits is very low
======================================================
*/

/*
==================
Sys_WaitForEvent
==================
*/
void Sys_WaitForEvent(int index) {
	assert(index >= 0 && index < MAX_TRIGGER_EVENTS);

	Sys_EnterCriticalSection(CRITICAL_SECTION_SYS);

	assert(!waiting[index]);	// WaitForEvent from multiple threads? that wouldn't be good
	if (signaled[index]) {
		// emulate windows behaviour: signal has been raised already. clear and keep going
		signaled[index] = false;
	} else {
		waiting[index] = true;
		if (SDL_CondWait(cond[index], mutex[CRITICAL_SECTION_SYS]) != 0)
			common->Error("ERROR: SDL_CondWait failed\n");
		waiting[index] = false;
	}

	Sys_LeaveCriticalSection(CRITICAL_SECTION_SYS);
}

/*
==================
Sys_TriggerEvent
==================
*/
void Sys_TriggerEvent(int index) {
	assert(index >= 0 && index < MAX_TRIGGER_EVENTS);

	Sys_EnterCriticalSection(CRITICAL_SECTION_SYS);

	if (waiting[index]) {
		if (SDL_CondSignal(cond[index]) != 0)
			common->Error("ERROR: SDL_CondSignal failed\n");
	} else {
		// emulate windows behaviour: if no thread is waiting, leave the signal on so next wait keeps going
		signaled[index] = true;
	}

	Sys_LeaveCriticalSection(CRITICAL_SECTION_SYS);
}

/*
==================
Sys_CreateThread
==================
*/
void Sys_CreateThread(xthread_t function, void *parms, xthreadInfo& info, const char *name) {
	Sys_EnterCriticalSection();

#if SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_Thread *t = SDL_CreateThread(function, name, parms);
#else
	SDL_Thread *t = SDL_CreateThread(function, parms);
#endif

	if (!t) {
		common->Error("ERROR: SDL_thread for '%s' failed\n", name);
		Sys_LeaveCriticalSection();
		return;
	}

	info.name = name;
	info.threadHandle = t;
	info.threadId = SDL_GetThreadID(t);

	if (thread_count < MAX_THREADS)
		thread[thread_count++] = &info;
	else
		common->DPrintf("WARNING: MAX_THREADS reached\n");

	Sys_LeaveCriticalSection();
}

/*
==================
Sys_DestroyThread
==================
*/
void Sys_DestroyThread(xthreadInfo& info) {
	assert(info.threadHandle);

	SDL_WaitThread(info.threadHandle, NULL);

	info.name = NULL;
	info.threadHandle = NULL;
	info.threadId = 0;

	Sys_EnterCriticalSection();

	for (int i = 0; i < thread_count; i++) {
		if (&info == thread[i]) {
			thread[i] = NULL;

			int j;
			for (j = i + 1; j < thread_count; j++)
				thread[j - 1] = thread[j];

			thread[j - 1] = NULL;
			thread_count--;

			break;
		}
	}

	Sys_LeaveCriticalSection( );
}

/*
==================
Sys_GetThreadName
find the name of the calling thread
==================
*/
const char *Sys_GetThreadName(int *index) {
	const char *name;

	Sys_EnterCriticalSection();

	unsigned int id = SDL_ThreadID();

	for (int i = 0; i < thread_count; i++) {
		if (id == thread[i]->threadId) {
			if (index)
				*index = i;

			name = thread[i]->name;

			Sys_LeaveCriticalSection();

			return name;
		}
	}

	if (index)
		*index = -1;

	Sys_LeaveCriticalSection();

	return "main";
}


/*
========================
Sys_MutexCreate
========================
*/
void Sys_MutexCreate( mutexHandle_t& handle )
{
    pthread_mutexattr_t attr;

    pthread_mutexattr_init( &attr );
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK );
    pthread_mutex_init( &handle, &attr );

    pthread_mutexattr_destroy( &attr );
}

/*
========================
Sys_MutexDestroy
========================
*/
void Sys_MutexDestroy( mutexHandle_t& handle )
{
    pthread_mutex_destroy( &handle );
}

/*
========================
Sys_MutexLock
========================
*/
bool Sys_MutexLock( mutexHandle_t& handle, bool blocking )
{
    if( pthread_mutex_trylock( &handle ) != 0 )
    {
        if( !blocking )
        {
            return false;
        }
        pthread_mutex_lock( &handle );
    }
    return true;
}

/*
========================
Sys_MutexUnlock
========================
*/
void Sys_MutexUnlock( mutexHandle_t& handle )
{
    pthread_mutex_unlock( & handle );
}

/*
========================
Sys_SignalClear
========================
*/
void Sys_SignalClear( signalHandle_t& handle )
{
    // ResetEvent( handle );
    pthread_mutex_lock( &handle.mutex );

    // TODO: probably signaled could be atomically changed?
    handle.signaled = false;

    pthread_mutex_unlock( &handle.mutex );
}

/*
========================
Sys_SignalDestroy
========================
*/
void Sys_SignalDestroy( signalHandle_t& handle )
{
    // CloseHandle( handle );
    handle.signaled = false;
    handle.waiting = 0;
    pthread_mutex_destroy( &handle.mutex );
    pthread_cond_destroy( &handle.cond );
}

/*
========================
Sys_SignalRaise
========================
*/
void Sys_SignalRaise( signalHandle_t& handle )
{
    // SetEvent( handle );
    pthread_mutex_lock( &handle.mutex );

    if( handle.manualReset )
    {
        // signaled until reset
        handle.signaled = true;
        // wake *all* threads waiting on this cond
        pthread_cond_broadcast( &handle.cond );
    }
    else
    {
        // automode: signaled until first thread is released
        if( handle.waiting > 0 )
        {
            // there are waiting threads => release one
            pthread_cond_signal( &handle.cond );
        }
        else
        {
            // no waiting threads, save signal
            handle.signaled = true;
            // while the MSDN documentation is a bit unspecific about what happens
            // when SetEvent() is called n times without a wait inbetween
            // (will only one wait be successful afterwards or n waits?)
            // it seems like the signaled state is a flag, not a counter.
            // http://stackoverflow.com/a/13703585 claims the same.
        }
    }

    pthread_mutex_unlock( &handle.mutex );
}

/*
========================
Sys_Yield
========================
*/
void Sys_Yield()
{
#if defined(__ANDROID__) || defined(__APPLE__)
    sched_yield();
#else
    pthread_yield();
#endif
}

/*
========================
Sys_CPUCount

numLogicalCPUCores	- the number of logical CPU per core
numPhysicalCPUCores	- the total number of cores per package
numCPUPackages		- the total number of packages (physical processors)
========================
*/

//TODO: improve this.
void Sys_CPUCount( int& numLogicalCPUCores, int& numPhysicalCPUCores, int& numCPUPackages )
{
    static bool		init = false;
    static double	ret;

    static int		s_numLogicalCPUCores;
    static int		s_numPhysicalCPUCores;
    static int		s_numCPUPackages;

    int		fd, len, pos, end;
    char	buf[ 4096 ];
    char	number[100];

    if( init )
    {
        numPhysicalCPUCores = s_numPhysicalCPUCores;
        numLogicalCPUCores = s_numLogicalCPUCores;
        numCPUPackages = s_numCPUPackages;
    }

    s_numPhysicalCPUCores = 1;
    s_numLogicalCPUCores = 1;
    s_numCPUPackages = 1;

    fd = open( "/proc/cpuinfo", O_RDONLY );
    if( fd != -1 )
    {
        len = read( fd, buf, 4096 );
        close( fd );
        pos = 0;
        while( pos < len )
        {
            if( !idStr::Cmpn( buf + pos, "cpu cores", 9 ) )
            {
                pos = strchr( buf + pos, ':' ) - buf + 2;
                end = strchr( buf + pos, '\n' ) - buf;
                if( pos < len && end < len )
                {
                    idStr::Copynz( number, buf + pos, sizeof( number ) );
                    assert( ( end - pos ) > 0 && ( end - pos ) < sizeof( number ) );
                    number[ end - pos ] = '\0';

                    int processor = atoi( number );

                    if( ( processor ) > s_numPhysicalCPUCores )
                    {
                        s_numPhysicalCPUCores = processor;
                    }
                }
                else
                {
                    common->Printf( "failed parsing /proc/cpuinfo\n" );
                    break;
                }
            }
            else if( !idStr::Cmpn( buf + pos, "siblings", 8 ) )
            {
                pos = strchr( buf + pos, ':' ) - buf + 2;
                end = strchr( buf + pos, '\n' ) - buf;
                if( pos < len && end < len )
                {
                    idStr::Copynz( number, buf + pos, sizeof( number ) );
                    assert( ( end - pos ) > 0 && ( end - pos ) < sizeof( number ) );
                    number[ end - pos ] = '\0';

                    int coreId = atoi( number );

                    if( ( coreId ) > s_numLogicalCPUCores )
                    {
                        s_numLogicalCPUCores = coreId;
                    }
                }
                else
                {
                    common->Printf( "failed parsing /proc/cpuinfo\n" );
                    break;
                }
            }

            pos = strchr( buf + pos, '\n' ) - buf + 1;
        }
    }

    common->Printf( "/proc/cpuinfo CPU processors: %d\n", s_numPhysicalCPUCores );
    common->Printf( "/proc/cpuinfo CPU logical cores: %d\n", s_numLogicalCPUCores );

    numPhysicalCPUCores = s_numPhysicalCPUCores;
    numLogicalCPUCores = s_numLogicalCPUCores;
    numCPUPackages = s_numCPUPackages;
}

/*
========================
Sys_SignalWait
========================
*/
bool Sys_SignalWait( signalHandle_t& handle, int timeout )
{
    //DWORD result = WaitForSingleObject( handle, timeout == idSysSignal::WAIT_INFINITE ? INFINITE : timeout );
    //assert( result == WAIT_OBJECT_0 || ( timeout != idSysSignal::WAIT_INFINITE && result == WAIT_TIMEOUT ) );
    //return ( result == WAIT_OBJECT_0 );

    int status;
    pthread_mutex_lock( &handle.mutex );

    if( handle.signaled ) // there is a signal that hasn't been used yet
    {
        if( ! handle.manualReset ) // for auto-mode only one thread may be released - this one.
            handle.signaled = false;

        status = 0; // success!
    }
    else // we'll have to wait for a signal
    {
        ++handle.waiting;
        if( timeout == idSysSignal::WAIT_INFINITE )
        {
            status = pthread_cond_wait( &handle.cond, &handle.mutex );
        }
        else
        {
            timespec ts;

            clock_gettime( CLOCK_REALTIME, &ts );

            // DG: handle timeouts > 1s better
            ts.tv_nsec += ( timeout % 1000 ) * 1000000; // millisec to nanosec
            ts.tv_sec  += timeout / 1000;
            if( ts.tv_nsec >= 1000000000 ) // nanoseconds are more than one second
            {
                ts.tv_nsec -= 1000000000; // remove one second in nanoseconds
                ts.tv_sec += 1; // add one second to seconds
            }
            // DG end
            status = pthread_cond_timedwait( &handle.cond, &handle.mutex, &ts );
        }
        --handle.waiting;
    }

    pthread_mutex_unlock( &handle.mutex );

    assert( status == 0 || ( timeout != idSysSignal::WAIT_INFINITE && status == ETIMEDOUT ) );

    return ( status == 0 );
}

/*
================
Sys_Microseconds
================
*/

//TODO: double check this.
static uint64_t sys_microTimeBase = 0;

uint64_t Sys_Microseconds()
{
    uint64_t curtime;
    struct timespec ts;

    clock_gettime( CLOCK_MONOTONIC_RAW, &ts );

    if( !sys_microTimeBase )
    {
        sys_microTimeBase = ts.tv_sec;
        return ts.tv_nsec / 1000;
    }

    curtime = ( ts.tv_sec - sys_microTimeBase ) * 1000000 + ts.tv_nsec / 1000;

    return curtime;
}

/*
========================
Sys_SignalCreate
========================
*/
void Sys_SignalCreate( signalHandle_t& handle, bool manualReset )
{
    // handle = CreateEvent( NULL, manualReset, FALSE, NULL );

    handle.manualReset = manualReset;
    // if this is true, the signal is only set to nonsignaled when Clear() is called,
    // else it's "auto-reset" and the state is set to !signaled after a single waiting
    // thread has been released

    // the inital state is always "not signaled"
    handle.signaled = false;
    handle.waiting = 0;
#if 0
    pthread_mutexattr_t attr;

	pthread_mutexattr_init( &attr );
	pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK );
	//pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_DEFAULT );
	pthread_mutex_init( &mutex, &attr );
	pthread_mutexattr_destroy( &attr );
#else
    pthread_mutex_init( &handle.mutex, NULL );
#endif

    pthread_cond_init( &handle.cond, NULL );
}

/*
========================
Sys_DestroyThread
========================
*/
void Sys_DestroyThread( uintptr_t threadHandle )
{
    if( threadHandle == 0 )
    {
        return;
    }

    char	name[128];
    name[0] = '\0';

#if defined(DEBUG_THREADS)
    Sys_GetThreadName( ( pthread_t )threadHandle, name, sizeof( name ) );
#endif

    if( pthread_join( ( pthread_t )threadHandle, NULL ) != 0 )
    {
        idLib::common->FatalError( "ERROR: pthread_join %s failed\n", name );
    }
}

/*
========================
Sys_Createthread
========================
*/
typedef void* ( *pthread_function_t )( void* );

uintptr_t Sys_CreateThread( xthread_t function, void* parms, xthreadPriority priority, const char* name, core_t core, int stackSize, bool suspended )
{
    pthread_attr_t attr;
    pthread_attr_init( &attr );

    if( pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE ) != 0 )
    {
        idLib::common->FatalError( "ERROR: pthread_attr_setdetachstate %s failed\n", name );
        return ( uintptr_t )0;
    }

    pthread_t handle;
    if( pthread_create( ( pthread_t* )&handle, &attr, ( pthread_function_t )function, parms ) != 0 )
    {
        idLib::common->FatalError( "ERROR: pthread_create %s failed\n", name );
        return ( uintptr_t )0;
    }

#if defined(DEBUG_THREADS)
    if( Sys_SetThreadName( handle, name ) != 0 )
	{
		idLib::common->Warning( "Warning: pthread_setname_np %s failed\n", name );
		return ( uintptr_t )0;
	}
#endif

    pthread_attr_destroy( &attr );


#if 0
    // RB: realtime policies require root privileges

	// all Linux threads have one of the following scheduling policies:

	// SCHED_OTHER or SCHED_NORMAL: the default policy,  priority: [-20..0..19], default 0

	// SCHED_FIFO: first in/first out realtime policy

	// SCHED_RR: round-robin realtime policy

	// SCHED_BATCH: similar to SCHED_OTHER, but with a throughput orientation

	// SCHED_IDLE: lower priority than SCHED_OTHER

	int schedulePolicy = SCHED_OTHER;
	struct sched_param scheduleParam;

	int error = pthread_getschedparam( handle, &schedulePolicy, &scheduleParam );
	if( error != 0 )
	{
		idLib::common->FatalError( "ERROR: pthread_getschedparam %s failed: %s\n", name, strerror( error ) );
		return ( uintptr_t )0;
	}

	schedulePolicy = SCHED_FIFO;

	int minPriority = sched_get_priority_min( schedulePolicy );
	int maxPriority = sched_get_priority_max( schedulePolicy );

	if( priority == THREAD_HIGHEST )
	{
		//  we better sleep enough to do this
		scheduleParam.__sched_priority = maxPriority;
	}
	else if( priority == THREAD_ABOVE_NORMAL )
	{
		scheduleParam.__sched_priority = Lerp( minPriority, maxPriority, 0.75f );
	}
	else if( priority == THREAD_NORMAL )
	{
		scheduleParam.__sched_priority = Lerp( minPriority, maxPriority, 0.5f );
	}
	else if( priority == THREAD_BELOW_NORMAL )
	{
		scheduleParam.__sched_priority = Lerp( minPriority, maxPriority, 0.25f );
	}
	else if( priority == THREAD_LOWEST )
	{
		scheduleParam.__sched_priority = minPriority;
	}

	// set new priority
	error = pthread_setschedparam( handle, schedulePolicy, &scheduleParam );
	if( error != 0 )
	{
		idLib::common->FatalError( "ERROR: pthread_setschedparam( name = %s, policy = %i, priority = %i ) failed: %s\n", name, schedulePolicy, scheduleParam.__sched_priority, strerror( error ) );
		return ( uintptr_t )0;
	}

	pthread_getschedparam( handle, &schedulePolicy, &scheduleParam );
	if( error != 0 )
	{
		idLib::common->FatalError( "ERROR: pthread_getschedparam %s failed: %s\n", name, strerror( error ) );
		return ( uintptr_t )0;
	}
#endif

    // Under Linux, we don't set the thread affinity and let the OS deal with scheduling

    return ( uintptr_t )handle;
}

/*
========================
Sys_InterlockedIncrement
========================
*/
interlockedInt_t Sys_InterlockedIncrement( interlockedInt_t& value )
{
    // return InterlockedIncrementAcquire( & value );
    return __sync_add_and_fetch( &value, 1 );
}

/*
========================
Sys_InterlockedDecrement
========================
*/
interlockedInt_t Sys_InterlockedDecrement( interlockedInt_t& value )
{
    // return InterlockedDecrementRelease( & value );
    return __sync_sub_and_fetch( &value, 1 );
}