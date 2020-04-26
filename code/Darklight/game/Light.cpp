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

#include "game_precompiled.h"
#pragma hdrstop

#include "Game_local.h"

/*
===============================================================================

  idLight

===============================================================================
*/

const idEventDef EV_Light_SetShader( "setShader", "s" );
const idEventDef EV_Light_GetLightParm( "getLightParm", "d", 'f' );
const idEventDef EV_Light_SetLightParm( "setLightParm", "df" );
const idEventDef EV_Light_SetLightParms( "setLightParms", "ffff" );
const idEventDef EV_Light_SetRadiusXYZ( "setRadiusXYZ", "fff" );
const idEventDef EV_Light_SetRadius( "setRadius", "f" );
const idEventDef EV_Light_On( "On", NULL );
const idEventDef EV_Light_Off( "Off", NULL );
const idEventDef EV_Light_FadeOut( "fadeOutLight", "f" );
const idEventDef EV_Light_FadeIn( "fadeInLight", "f" );

CLASS_DECLARATION( idEntity, idLight )
	EVENT( EV_Light_SetShader,		idLight::Event_SetShader )
	EVENT( EV_Light_GetLightParm,	idLight::Event_GetLightParm )
	EVENT( EV_Light_SetLightParm,	idLight::Event_SetLightParm )
	EVENT( EV_Light_SetLightParms,	idLight::Event_SetLightParms )
	EVENT( EV_Light_SetRadiusXYZ,	idLight::Event_SetRadiusXYZ )
	EVENT( EV_Light_SetRadius,		idLight::Event_SetRadius )
	EVENT( EV_Hide,					idLight::Event_Hide )
	EVENT( EV_Show,					idLight::Event_Show )
	EVENT( EV_Light_On,				idLight::Event_On )
	EVENT( EV_Light_Off,			idLight::Event_Off )
	EVENT( EV_Activate,				idLight::Event_ToggleOnOff )
	EVENT( EV_PostSpawn,			idLight::Event_SetSoundHandles )
	EVENT( EV_Light_FadeOut,		idLight::Event_FadeOut )
	EVENT( EV_Light_FadeIn,			idLight::Event_FadeIn )
END_CLASS


/*
================
idGameEdit::ParseSpawnArgsToRenderLight

parse the light parameters
this is the canonical renderLight parm parsing,
which should be used by dmap and the editor
================
*/
void idGameEdit::ParseSpawnArgsToRenderLight( const idDict *args, idRenderLight *renderLight ) {
	const char	*texture;
	idVec3	color;
	const char* temp;

	renderLight->SetLightChannel(LIGHT_CHANNEL_WORLD, true);

	idVec3 origin;
	if (!args->GetVector("light_origin", "", origin)) {
		args->GetVector( "origin", "", origin );
	}
	renderLight->SetOrigin(origin);

	// Set the unique light id for this light.
	const char* name = args->GetString("name");
	renderLight->SetName(name);
	renderLight->SetUniqueLightId(idStr::Hash(name));

	// Setup the lighting channels.
	{
		idStr litChannel = args->GetString("litchannel", "");
		if (litChannel.Length() > 0) {
			renderLight->ClearLightChannel(); // If we have light channels set, we have to opt into any channel we want to use.

			idParser litChannelParser;
			litChannelParser.LoadMemory(litChannel, litChannel.Length(), "litChannel");
			while (!litChannelParser.EndOfFile()) {
				renderLight->SetLightChannel(litChannelParser.ParseInt(), true);
			}
		}
	}

	if(args->GetBool("realtime", "0")) {
		renderLight->SetLightRenderType(LIGHT_RENDER_DYNAMIC);
	}

	temp = args->GetString("ies");
	if (temp[0] != '\0') {
		idImage* image = renderSystem->FindIESImage(temp);		
		renderLight->SetIESTexture(image);
	}

	if (args->FindKey("light_target") == NULL) {
		// create a point light
		renderLight->SetPointLight(true);

		// allow an optional relative center of light and shadow offset
		renderLight->SetLightCenter(args->GetVector("light_center", "0 0 0"));
		if (args->FindKey("light_radius") != NULL) {
			renderLight->SetLightRadius(args->GetVector("light_radius"));
		}
		else {
			float radius;
			args->GetFloat("light", "300", radius);
			renderLight->SetLightRadius(idVec3(radius, radius, radius));
		}		
	}
	else
	{
		renderLight->SetTarget(args->GetVector("light_target", "0 0 0"));
		renderLight->SetUp(args->GetVector("light_up", "0 0 0"));
		renderLight->SetRight(args->GetVector("light_right", "0 0 0"));
		renderLight->SetStart(args->GetVector("light_start", "0 0 0"));

		if (args->FindKey("light_end") != NULL) {
			renderLight->SetEnd(args->GetVector("light_end", "0 0 0"));
		}
		else {
			renderLight->SetEnd(renderLight->GetTarget());
		}
	}

	// get the rotation matrix in either full form, or single angle form
	idAngles angles;
	idMat3 mat;
	if ( !args->GetMatrix( "light_rotation", "1 0 0 0 1 0 0 0 1", mat ) ) {
		if ( !args->GetMatrix( "rotation", "1 0 0 0 1 0 0 0 1", mat ) ) {
	   		args->GetFloat( "angle", "0", angles[ 1 ] );
   			angles[ 0 ] = 0;
			angles[ 1 ] = idMath::AngleNormalize360( angles[ 1 ] );
	   		angles[ 2 ] = 0;
			mat = angles.ToMat3();
		}
	}

	// fix degenerate identity matrices
	mat[0].FixDegenerateNormal();
	mat[1].FixDegenerateNormal();
	mat[2].FixDegenerateNormal();

	renderLight->SetAxis(mat);

	// check for other attributes
	args->GetVector( "_color", "1 1 1", color );
	renderLight->SetLightColor(color);
	renderLight->SetNoShadows(args->GetBool("noshadows", "0"));
	renderLight->SetNoSpecular(args->GetBool("nospecular", "0"));
	renderLight->SetParallelLight(args->GetBool("parallel", "0"));
	renderLight->SetAmbientLight(args->GetBool("ambient", "0"));
}

/*
================
idLight::UpdateChangeableSpawnArgs
================
*/
void idLight::UpdateChangeableSpawnArgs( const idDict *source ) {

	idEntity::UpdateChangeableSpawnArgs( source );

	if ( source ) {
		source->Print();
	}
	FreeSoundEmitter( true );
	gameEdit->ParseSpawnArgsToRefSound( source ? source : &spawnArgs, &refSound );
	if ( refSound.shader && !refSound.waitfortrigger ) {
		StartSoundShader( refSound.shader, SND_CHANNEL_ANY, 0, false, NULL );
	}

	gameEdit->ParseSpawnArgsToRenderLight( source ? source : &spawnArgs, renderLight );

	UpdateVisuals();
}

/*
================
idLight::idLight
================
*/
idLight::idLight() {	
	renderLight = NULL;
	localLightOrigin	= vec3_zero;
	localLightAxis		= mat3_identity;
	levels				= 0;
	currentLevel		= 0;
	baseColor			= vec3_zero;
	breakOnTrigger		= false;
	count				= 0;
	triggercount		= 0;
	lightParent			= NULL;
	fadeFrom.Set( 1, 1, 1, 1 );
	fadeTo.Set( 1, 1, 1, 1 );
	fadeStart			= 0;
	fadeEnd				= 0;
	soundWasPlaying		= false;
}

/*
================
idLight::~idLight
================
*/
idLight::~idLight() {
	if(renderLight) {
		gameRenderWorld->FreeRenderLight(renderLight);
		renderLight = NULL;
	}
}

/*
================
idLight::Save

archives object for save game file
================
*/
void idLight::Save( idSaveGame *savefile ) const {
	savefile->WriteRenderLight( renderLight );
	
	//savefile->WriteBool( renderLight->prelightModel != NULL );

	savefile->WriteVec3( localLightOrigin );
	savefile->WriteMat3( localLightAxis );

	savefile->WriteString( brokenModel );
	savefile->WriteInt( levels );
	savefile->WriteInt( currentLevel );

	savefile->WriteVec3( baseColor );
	savefile->WriteBool( breakOnTrigger );
	savefile->WriteInt( count );
	savefile->WriteInt( triggercount );
	savefile->WriteObject( lightParent );

	savefile->WriteVec4( fadeFrom );
	savefile->WriteVec4( fadeTo );
	savefile->WriteInt( fadeStart );
	savefile->WriteInt( fadeEnd );
	savefile->WriteBool( soundWasPlaying );
}

/*
================
idLight::Restore

unarchives object from save game file
================
*/
void idLight::Restore( idRestoreGame *savefile ) {
	bool hadPrelightModel;

	savefile->ReadRenderLight( renderLight );

	savefile->ReadBool( hadPrelightModel );
	//renderLight.prelightModel = renderModelManager->CheckModel( va( "_prelight_%s", name.c_str() ) );
	//if ( ( renderLight.prelightModel == NULL ) && hadPrelightModel ) {
	//	assert( 0 );
	//	if ( developer.GetBool() ) {
	//		// we really want to know if this happens
	//		gameLocal.Error( "idLight::Restore: prelightModel '_prelight_%s' not found", name.c_str() );
	//	} else {
	//		// but let it slide after release
	//		gameLocal.Warning( "idLight::Restore: prelightModel '_prelight_%s' not found", name.c_str() );
	//	}
	//}

	savefile->ReadVec3( localLightOrigin );
	savefile->ReadMat3( localLightAxis );

	savefile->ReadString( brokenModel );
	savefile->ReadInt( levels );
	savefile->ReadInt( currentLevel );

	savefile->ReadVec3( baseColor );
	savefile->ReadBool( breakOnTrigger );
	savefile->ReadInt( count );
	savefile->ReadInt( triggercount );
	savefile->ReadObject( reinterpret_cast<idClass *&>( lightParent ) );

	savefile->ReadVec4( fadeFrom );
	savefile->ReadVec4( fadeTo );
	savefile->ReadInt( fadeStart );
	savefile->ReadInt( fadeEnd );
	savefile->ReadBool( soundWasPlaying );

	SetLightLevel();
}

/*
================
idLight::Spawn
================
*/
void idLight::Spawn( void ) {
	bool start_off;
	bool needBroken;
	const char *demonic_shader;

	BaseSpawn();

	// Allocate a new renderlight.
	renderLight = gameRenderWorld->AllocRenderLight();

	// Set the Unique Name for the light.
	uniqueLightName = va("%s_%d", spawnArgs.GetString("name"), gameLocal.uniqueLightCount++);

	// do the parsing the same way dmap and the editor do
	gameEdit->ParseSpawnArgsToRenderLight( &spawnArgs, renderLight );

	// we need the origin and axis relative to the physics origin/axis
	localLightOrigin = ( renderLight->GetOrigin() - GetPhysics()->GetOrigin() ) * GetPhysics()->GetAxis().Transpose();
	localLightAxis = renderLight->GetAxis() * GetPhysics()->GetAxis().Transpose();

	// set the base color from the shader parms
	baseColor = renderLight->GetLightColor();

	// set the number of light levels
	spawnArgs.GetInt( "levels", "1", levels );
	currentLevel = levels;
	if ( levels <= 0 ) {
		gameLocal.Error( "Invalid light level set on entity #%d(%s)", entityNumber, name.c_str() );
	}

	// make sure the demonic shader is cached
	if ( spawnArgs.GetString( "mat_demonic", NULL, &demonic_shader ) ) {
		declManager->FindType( DECL_MATERIAL, demonic_shader );
	}

	// game specific functionality, not mirrored in
	// editor or dmap light parsing

	// also put the light texture on the model, so light flares
	// can get the current intensity of the light
//	renderEntity->SetReferenceShader(renderLight->GetShader());

	spawnArgs.GetBool( "start_off", "0", start_off );
	if ( start_off ) {
		Off();
	}

	health = spawnArgs.GetInt( "health", "0" );
	spawnArgs.GetString( "broken", "", brokenModel );
	spawnArgs.GetBool( "break", "0", breakOnTrigger );
	spawnArgs.GetInt( "count", "1", count );

	triggercount = 0;

	fadeFrom.Set( 1, 1, 1, 1 );
	fadeTo.Set( 1, 1, 1, 1 );
	fadeStart			= 0;
	fadeEnd				= 0;

	// if we have a health make light breakable
	if ( health ) {
		idStr model = spawnArgs.GetString( "model" );		// get the visual model
		if ( !model.Length() ) {
			gameLocal.Error( "Breakable light without a model set on entity #%d(%s)", entityNumber, name.c_str() );
		}

		fl.takedamage	= true;

		// see if we need to create a broken model name
		needBroken = true;
		if ( model.Length() && !brokenModel.Length() ) {
			int	pos;

			needBroken = false;
		
			pos = model.Find( "." );
			if ( pos < 0 ) {
				pos = model.Length();
			}
			if ( pos > 0 ) {
				model.Left( pos, brokenModel );
			}
			brokenModel += "_broken";
			if ( pos > 0 ) {
				brokenModel += &model[ pos ];
			}
		}
	
		// make sure the model gets cached
		if ( !renderModelManager->CheckModel( brokenModel ) ) {
			if ( needBroken ) {
				gameLocal.Error( "Model '%s' not found for entity %d(%s)", brokenModel.c_str(), entityNumber, name.c_str() );
			} else {
				brokenModel = "";
			}
		}

		GetPhysics()->SetContents( spawnArgs.GetBool( "nonsolid" ) ? 0 : CONTENTS_SOLID );
	
		// make sure the collision model gets cached
		collisionModelManager->LoadModel( brokenModel, false );
	}

	PostEventMS( &EV_PostSpawn, 0 );

	UpdateVisuals();
}

/*
================
idLight::SetLightLevel
================
*/
void idLight::SetLightLevel( void ) {
	idVec3	color;
	float	intensity;

	intensity = ( float )currentLevel / ( float )levels;
	color = baseColor * intensity;
	renderLight->SetLightColor(color);
	renderEntity->SetShaderParms( SHADERPARM_RED, color[ 0 ]);
	renderEntity->SetShaderParms( SHADERPARM_GREEN, color[ 1 ]);
	renderEntity->SetShaderParms( SHADERPARM_BLUE, color[ 2 ]);
	PresentLightDefChange();
	PresentModelDefChange();
}

/*
================
idLight::GetColor
================
*/
void idLight::GetColor( idVec3 &out ) const {
	out = renderLight->GetLightColor();
}

/*
================
idLight::GetColor
================
*/
void idLight::GetColor( idVec4 &out ) const {
	out = renderLight->GetLightColor().ToVec4();
}

/*
================
idLight::SetColor
================
*/
void idLight::SetColor( float red, float green, float blue ) {
	baseColor.Set( red, green, blue );
	SetLightLevel();
}

/*
================
idLight::SetColor
================
*/
void idLight::SetColor( const idVec4 &color ) {
	baseColor = color.ToVec3();
	//renderLight->SetShaderParam(SHADERPARM_ALPHA, color[3]);
	renderEntity->SetShaderParms(SHADERPARM_ALPHA, color[ 3 ]);
	SetLightLevel();
}

/*
================
idLight::SetShader
================
*/
void idLight::SetShader( const char *shadername ) {
	// allow this to be NULL
	//renderLight->SetShader(declManager->FindMaterial( shadername, false ));
	//PresentLightDefChange();
}

/*
================
idLight::SetLightParm
================
*/
void idLight::SetLightParm( int parmnum, float value ) {
	//if ( ( parmnum < 0 ) || ( parmnum >= MAX_ENTITY_SHADER_PARMS ) ) {
	//	gameLocal.Error( "shader parm index (%d) out of range", parmnum );
	//}
	//
	//renderLight->SetShaderParam(parmnum, value);
	//PresentLightDefChange();
}

/*
================
idLight::SetLightParms
================
*/
void idLight::SetLightParms( float parm0, float parm1, float parm2, float parm3 ) {
	renderLight->SetLightColor(idVec3(parm0, parm1, parm2));
	renderEntity->SetShaderParms( SHADERPARM_RED, parm0);
	renderEntity->SetShaderParms( SHADERPARM_GREEN, parm1);
	renderEntity->SetShaderParms( SHADERPARM_BLUE, parm2);
	renderEntity->SetShaderParms( SHADERPARM_ALPHA, parm3);
	PresentLightDefChange();
	PresentModelDefChange();
}

/*
================
idLight::SetRadiusXYZ
================
*/
void idLight::SetRadiusXYZ( float x, float y, float z ) {
	renderLight->SetLightRadius(idVec3(x, y, z));
	PresentLightDefChange();
}

/*
================
idLight::SetRadius
================
*/
void idLight::SetRadius( float radius ) {
	renderLight->SetLightRadius(idVec3(radius, radius, radius));
	PresentLightDefChange();
}

/*
================
idLight::On
================
*/
void idLight::On( void ) {
	currentLevel = levels;
	// offset the start time of the shader to sync it to the game time
	//renderLight->SetShaderParam(SHADERPARM_TIMEOFFSET, -MS2SEC( gameLocal.time ));
	if ( ( soundWasPlaying || refSound.waitfortrigger ) && refSound.shader ) {
		StartSoundShader( refSound.shader, SND_CHANNEL_ANY, 0, false, NULL );
		soundWasPlaying = false;
	}
	SetLightLevel();
	BecomeActive( TH_UPDATEVISUALS );
}

/*
================
idLight::Off
================
*/
void idLight::Off( void ) {
	currentLevel = 0;
	// kill any sound it was making
	if ( refSound.referenceSound && refSound.referenceSound->CurrentlyPlaying() ) {
		StopSound( SND_CHANNEL_ANY, false );
		soundWasPlaying = true;
	}
	SetLightLevel();
	BecomeActive( TH_UPDATEVISUALS );
}

/*
================
idLight::Fade
================
*/
void idLight::Fade( const idVec4 &to, float fadeTime ) {
	GetColor( fadeFrom );
	fadeTo = to;
	fadeStart = gameLocal.time;
	fadeEnd = gameLocal.time + SEC2MS( fadeTime );
	BecomeActive( TH_THINK );
}

/*
================
idLight::FadeOut
================
*/
void idLight::FadeOut( float time ) {
	Fade( colorBlack, time );
}

/*
================
idLight::FadeIn
================
*/
void idLight::FadeIn( float time ) {
	idVec3 color;
	idVec4 color4;

	currentLevel = levels;
	spawnArgs.GetVector( "_color", "1 1 1", color );
	color4.Set( color.x, color.y, color.z, 1.0f );
	Fade( color4, time );
}

/*
================
idLight::Killed
================
*/
void idLight::Killed( idEntity *inflictor, idEntity *attacker, int damage, const idVec3 &dir, int location ) {
	BecomeBroken( attacker );
}

/*
================
idLight::BecomeBroken
================
*/
void idLight::BecomeBroken( idEntity *activator ) {
	const char *damageDefName;

	fl.takedamage = false;

	if ( brokenModel.Length() ) {
		SetModel( brokenModel );

		if ( !spawnArgs.GetBool( "nonsolid" ) ) {
			GetPhysics()->SetClipModel( new idClipModel( brokenModel.c_str() ), 1.0f );
			GetPhysics()->SetContents( CONTENTS_SOLID );
		}
	} else if ( spawnArgs.GetBool( "hideModelOnBreak" ) ) {
		SetModel( "" );
		GetPhysics()->SetContents( 0 );
	}

	if ( common->IsServer() ) {

		ServerSendEvent( EVENT_BECOMEBROKEN, NULL, true, -1 );

		if ( spawnArgs.GetString( "def_damage", "", &damageDefName ) ) {
			idVec3 origin = renderEntity->GetOrigin() + renderEntity->GetBounds().GetCenter() * renderEntity->GetAxis();
			gameLocal.RadiusDamage( origin, activator, activator, this, this, damageDefName );
		}

	}

		ActivateTargets( activator );

	// offset the start time of the shader to sync it to the game time
	renderEntity->SetShaderParms( SHADERPARM_TIMEOFFSET, -MS2SEC( gameLocal.time ));
//	renderLight->SetShaderParam(SHADERPARM_TIMEOFFSET, -MS2SEC( gameLocal.time ));

	// set the state parm
	renderEntity->SetShaderParms( SHADERPARM_MODE, 1);
//	renderLight->SetShaderParam( SHADERPARM_MODE, 1);

	// if the light has a sound, either start the alternate (broken) sound, or stop the sound
	const char *parm = spawnArgs.GetString( "snd_broken" );
	if ( refSound.shader || ( parm && *parm ) ) {
		StopSound( SND_CHANNEL_ANY, false );
		const idSoundShader *alternate = refSound.shader ? refSound.shader->GetAltSound() : declManager->FindSound( parm );
		if ( alternate ) {
			// start it with no diversity, so the leadin break sound plays
			refSound.referenceSound->StartSound( alternate, SND_CHANNEL_ANY, 0.0, 0 );
		}
	}

	parm = spawnArgs.GetString( "mtr_broken" );
	if ( parm && *parm ) {
		SetShader( parm );
	}

	UpdateVisuals();
}

/*
================
idLight::PresentLightDefChange
================
*/
void idLight::PresentLightDefChange( void ) {
	// let the renderer apply it to the world
	renderLight->UpdateRenderLight();
}

/*
================
idLight::PresentModelDefChange
================
*/
void idLight::PresentModelDefChange( void ) {
	if ( renderEntity->GetRenderModel() == NULL || IsHidden() ) {
		return;
	}

	// add to refresh list
	renderEntity->UpdateRenderEntity();
}

/*
================
idLight::Present
================
*/
void idLight::Present( void ) {
	// don't present to the renderer if the entity hasn't changed
	if ( !( thinkFlags & TH_UPDATEVISUALS ) ) {
		return;
	}

	// add the model
	idEntity::Present();

	// current transformation
	renderLight->SetAxis(localLightAxis * GetPhysics()->GetAxis());
	renderLight->SetOrigin(GetPhysics()->GetOrigin() + GetPhysics()->GetAxis() * localLightOrigin);

	// reference the sound for shader synced effects
	if ( lightParent ) {
		renderLight->SetReferenceSound(lightParent->GetSoundEmitter());
		renderEntity->SetReferenceSound(lightParent->GetSoundEmitter());
	}
	else {
		renderLight->SetReferenceSound(refSound.referenceSound);
		renderEntity->SetReferenceSound(refSound.referenceSound);
	}

	// update the renderLight and renderEntity to render the light and flare
	PresentLightDefChange();
	PresentModelDefChange();
}

/*
================
idLight::Think
================
*/
void idLight::Think( void ) {
	idVec4 color;

	if ( thinkFlags & TH_THINK ) {
		if ( fadeEnd > 0 ) {
			if ( gameLocal.time < fadeEnd ) {
				color.Lerp( fadeFrom, fadeTo, ( float )( gameLocal.time - fadeStart ) / ( float )( fadeEnd - fadeStart ) );
			} else {
				color = fadeTo;
				fadeEnd = 0;
				BecomeInactive( TH_THINK );
			}
			SetColor( color );
		}
	}

	RunPhysics();
	Present();
}

/*
================
idLight::GetPhysicsToSoundTransform
================
*/
bool idLight::GetPhysicsToSoundTransform( idVec3 &origin, idMat3 &axis ) {
	origin = localLightOrigin + renderLight->GetLightCenter();
	axis = localLightAxis * GetPhysics()->GetAxis();
	return true;
}

/*
================
idLight::FreeLightDef
================
*/
void idLight::FreeLightDef( void ) {
	if (renderLight != NULL) {
		gameRenderWorld->FreeRenderLight(renderLight);
		renderLight = NULL;
	}
}

/*
================
idLight::SaveState
================
*/
void idLight::SaveState( idDict *args ) {
	int i, c = spawnArgs.GetNumKeyVals();
	for ( i = 0; i < c; i++ ) {
		const idKeyValue *pv = spawnArgs.GetKeyVal(i);
		if ( pv->GetKey().Find( "editor_", false ) >= 0 || pv->GetKey().Find( "parse_", false ) >= 0 ) {
			continue;
		}
		args->Set( pv->GetKey(), pv->GetValue() );
	}
}

/*
===============
idLight::ShowEditingDialog
===============
*/
void idLight::ShowEditingDialog( void ) {
	if ( g_editEntityMode.GetInteger() == 1 ) {
		common->InitTool( EDITOR_LIGHT, &spawnArgs );
	} else {
		common->InitTool( EDITOR_SOUND, &spawnArgs );
	}
}

/*
================
idLight::Event_SetShader
================
*/
void idLight::Event_SetShader( const char *shadername ) {
	SetShader( shadername );
}

/*
================
idLight::Event_GetLightParm
================
*/
void idLight::Event_GetLightParm( int parmnum ) {
//	if ( ( parmnum < 0 ) || ( parmnum >= MAX_ENTITY_SHADER_PARMS ) ) {
//		gameLocal.Error( "shader parm index (%d) out of range", parmnum );
//	}
//
//	idThread::ReturnFloat( renderLight->GetShaderParam( parmnum ) );
}

/*
================
idLight::Event_SetLightParm
================
*/
void idLight::Event_SetLightParm( int parmnum, float value ) {
	SetLightParm( parmnum, value );
}

/*
================
idLight::Event_SetLightParms
================
*/
void idLight::Event_SetLightParms( float parm0, float parm1, float parm2, float parm3 ) {
	SetLightParms( parm0, parm1, parm2, parm3 );
}

/*
================
idLight::Event_SetRadiusXYZ
================
*/
void idLight::Event_SetRadiusXYZ( float x, float y, float z ) {
	SetRadiusXYZ( x, y, z );
}

/*
================
idLight::Event_SetRadius
================
*/
void idLight::Event_SetRadius( float radius ) {
	SetRadius( radius );
}

/*
================
idLight::Event_Hide
================
*/
void idLight::Event_Hide( void ) {
	Hide();
	PresentModelDefChange();
	Off();
}

/*
================
idLight::Event_Show
================
*/
void idLight::Event_Show( void ) {
	Show();
	PresentModelDefChange();
	On();
}

/*
================
idLight::Event_On
================
*/
void idLight::Event_On( void ) {
	On();
}

/*
================
idLight::Event_Off
================
*/
void idLight::Event_Off( void ) {
	Off();
}

/*
================
idLight::Event_ToggleOnOff
================
*/
void idLight::Event_ToggleOnOff( idEntity *activator ) {
	triggercount++;
	if ( triggercount < count ) {
		return;
	}

	// reset trigger count
	triggercount = 0;

	if ( breakOnTrigger ) {
		BecomeBroken( activator );
		breakOnTrigger = false;
		return;
	}

	if ( !currentLevel ) {
		On();
	}
	else {
		currentLevel--;
		if ( !currentLevel ) {
			Off();
		}
		else {
			SetLightLevel();
		}
	}
}

/*
================
idLight::Event_SetSoundHandles

  set the same sound def handle on all targeted lights
================
*/
void idLight::Event_SetSoundHandles( void ) {
	int i;
	idEntity *targetEnt;

	if ( !refSound.referenceSound ) {
		return;
	}

	for ( i = 0; i < targets.Num(); i++ ) {
		targetEnt = targets[ i ].GetEntity();
		if ( targetEnt && targetEnt->IsType( idLight::Type ) ) {
			idLight	*light = static_cast<idLight*>(targetEnt);
			light->lightParent = this;

			// explicitly delete any sounds on the entity
			light->FreeSoundEmitter( true );

			// manually set the refSound to this light's refSound
			light->renderEntity->SetReferenceSound((idSoundEmitter *)renderEntity->GetReferenceSound());

			// update the renderEntity to the renderer
			light->UpdateVisuals();
		}
	}
}

/*
================
idLight::Event_FadeOut
================
*/
void idLight::Event_FadeOut( float time ) {
	FadeOut( time );
}

/*
================
idLight::Event_FadeIn
================
*/
void idLight::Event_FadeIn( float time ) {
	FadeIn( time );
}

/*
================
idLight::ClientPredictionThink
================
*/
void idLight::ClientPredictionThink( void ) {
	Think();
}

/*
================
idLight::WriteToSnapshot
================
*/
void idLight::WriteToSnapshot( idBitMsg &msg ) const {

	GetPhysics()->WriteToSnapshot( msg );
	WriteBindToSnapshot( msg );

	msg.WriteByte( currentLevel );
	msg.WriteLong( PackColor( baseColor ) );
	// msg.WriteBits( lightParent.GetEntityNum(), GENTITYNUM_BITS );

/*	// only helps prediction
	msg.WriteLong( PackColor( fadeFrom ) );
	msg.WriteLong( PackColor( fadeTo ) );
	msg.WriteLong( fadeStart );
	msg.WriteLong( fadeEnd );
*/

	// FIXME: send renderLight.shader
	idVec3 lightRadius = renderLight->GetLightRadius();
	msg.WriteFloat( lightRadius[0], 5, 10 );
	msg.WriteFloat( lightRadius[1], 5, 10 );
	msg.WriteFloat( lightRadius[2], 5, 10 );

	idVec3 lightColor = renderLight->GetLightColor();
	msg.WriteFloat(lightColor[0]);
	msg.WriteFloat(lightColor[1]);
	msg.WriteFloat(lightColor[2]);

	WriteColorToSnapshot( msg );
}

/*
================
idLight::ReadFromSnapshot
================
*/
void idLight::ReadFromSnapshot( const idBitMsg &msg ) {
	idVec4	shaderColor;
	int		oldCurrentLevel = currentLevel;
	idVec3	oldBaseColor = baseColor;

	GetPhysics()->ReadFromSnapshot( msg );
	ReadBindFromSnapshot( msg );

	currentLevel = msg.ReadByte();
	if ( currentLevel != oldCurrentLevel ) {
		// need to call On/Off for flickering lights to start/stop the sound
		// while doing it this way rather than through events, the flickering is out of sync between clients
		// but at least there is no question about saving the event and having them happening globally in the world
		if ( currentLevel ) {
			On();
		} else {
			Off();
		}
	}
	UnpackColor( msg.ReadLong(), baseColor );
	// lightParentEntityNum = msg.ReadBits( GENTITYNUM_BITS );	

/*	// only helps prediction
	UnpackColor( msg.ReadLong(), fadeFrom );
	UnpackColor( msg.ReadLong(), fadeTo );
	fadeStart = msg.ReadLong();
	fadeEnd = msg.ReadLong();
*/

	// FIXME: read renderLight.shader
	idVec3 lightRadius;
	lightRadius[0] = msg.ReadFloat( 5, 10 );
	lightRadius[1] = msg.ReadFloat( 5, 10 );
	lightRadius[2] = msg.ReadFloat( 5, 10 );
	renderLight->SetLightRadius(lightRadius);

	idVec3 lightColor;

	lightColor.x = msg.ReadFloat();
	lightColor.y = msg.ReadFloat();
	lightColor.z = msg.ReadFloat();
	renderLight->SetLightColor(lightColor);

	ReadColorFromSnapshot( msg );

	{
		if ( ( currentLevel != oldCurrentLevel ) || ( baseColor != oldBaseColor ) ) {
			SetLightLevel();
		} else {
			PresentLightDefChange();
			PresentModelDefChange();
		}
	}
}

/*
================
idLight::ClientReceiveEvent
================
*/
bool idLight::ClientReceiveEvent( int event, int time, const idBitMsg &msg ) {

	switch( event ) {
		case EVENT_BECOMEBROKEN: {
			BecomeBroken( NULL );
			return true;
		}
		default: {
			return idEntity::ClientReceiveEvent( event, time, msg );
		}
	}
	return false;
}
