/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).  

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

#include "engine_precompiled.h"
#pragma hdrstop

#include "tr_local.h"

class idCinematicLocal : public idCinematic {
public:
							idCinematicLocal();
	virtual					~idCinematicLocal();

	virtual bool			InitFromFile( const char *qpath, bool looping );
	virtual cinData_t		ImageForTime( int milliseconds );
	virtual int				AnimationLength();
	virtual void			Close();
	virtual void			ResetTime(int time);

private:
};

idCinematicLocal::idCinematicLocal() {

}

idCinematicLocal::~idCinematicLocal() {

}

bool idCinematicLocal::InitFromFile(const char* qpath, bool looping) {
	return false;
}

cinData_t idCinematicLocal::ImageForTime(int milliseconds) {
	cinData_t data;
	return data;
}

int idCinematicLocal::AnimationLength() {
	return 0;
}

void idCinematicLocal::Close() {
	
}

void idCinematicLocal::ResetTime(int time) {

}

// initialize cinematic play back data
void idCinematic::InitCinematic(void) {

}

// shutdown cinematic play back data
void idCinematic::ShutdownCinematic(void) {

}

// allocates and returns a private subclass that implements the methods
// This should be used instead of new
idCinematic* idCinematic::Alloc() {
	return NULL;
}