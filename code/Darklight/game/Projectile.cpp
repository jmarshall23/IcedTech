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

	idProjectile

===============================================================================
*/


static const int BFG_DAMAGE_FREQUENCY			= 333;
static const float BOUNCE_SOUND_MIN_VELOCITY	= 200.0f;
static const float BOUNCE_SOUND_MAX_VELOCITY	= 400.0f;

const idEventDef EV_Explode( "<explode>", NULL );
const idEventDef EV_Fizzle( "<fizzle>", NULL );
const idEventDef EV_RadiusDamage( "<radiusdmg>", "e" );
const idEventDef EV_GetProjectileState( "getProjectileState", NULL, 'd' );

CLASS_DECLARATION( idEntity, idProjectile )
	EVENT( EV_Explode,				idProjectile::Event_Explode )
	EVENT( EV_Fizzle,				idProjectile::Event_Fizzle )
	EVENT( EV_Touch,				idProjectile::Event_Touch )
	EVENT( EV_RadiusDamage,			idProjectile::Event_RadiusDamage )
	EVENT( EV_GetProjectileState,	idProjectile::Event_GetProjectileState )
END_CLASS

/*
================
idProjectile::idProjectile
================
*/
idProjectile::idProjectile( void ) {
	owner				= NULL;
//	lightDefHandle		= -1;
	thrust				= 0.0f;
	thrust_end			= 0;
	smokeFly			= NULL;
	smokeFlyTime		= 0;
	state				= SPAWNED;
	lightOffset			= vec3_zero;
	lightStartTime		= 0;
	lightEndTime		= 0;
	lightColor			= vec3_zero;
	state				= SPAWNED;
	damagePower			= 1.0f;
	renderLight = NULL;
	memset( &projectileFlags, 0, sizeof( projectileFlags ) );

	// note: for net_instanthit projectiles, we will force this back to false at spawn time
	fl.networkSync		= true;

	netSyncPhysics		= false;

	trailFX = NULL;
}

/*
================
idProjectile::Spawn
================
*/
void idProjectile::Spawn( void ) {
	BaseSpawn();

	physicsObj.SetSelf( this );
	physicsObj.SetClipModel( new idClipModel( GetPhysics()->GetClipModel() ), 1.0f );
	physicsObj.SetContents( 0 );
	physicsObj.SetClipMask( 0 );
	physicsObj.PutToRest();
	SetPhysics( &physicsObj );

	// Create the FX particle.
	trailFX = gameLocal.CreateEffect(spawnArgs.GetString("trail_fx"), renderEntity->GetOrigin(), renderEntity->GetAxis(), false);
}

/*
================
idProjectile::Save
================
*/
void idProjectile::Save( idSaveGame *savefile ) const {

	owner.Save( savefile );

	projectileFlags_s flags = projectileFlags;
	LittleBitField( &flags, sizeof( flags ) );
	savefile->Write( &flags, sizeof( flags ) );

	savefile->WriteFloat( thrust );
	savefile->WriteInt( thrust_end );

	savefile->WriteRenderLight( renderLight );
//	savefile->WriteInt( (int)lightDefHandle );
	savefile->WriteVec3( lightOffset );
	savefile->WriteInt( lightStartTime );
	savefile->WriteInt( lightEndTime );
	savefile->WriteVec3( lightColor );

//	savefile->WriteParticle( smokeFly );
	savefile->WriteInt( smokeFlyTime );

	savefile->WriteInt( (int)state );

	savefile->WriteFloat( damagePower );

	savefile->WriteStaticObject( physicsObj );
	savefile->WriteStaticObject( thruster );
}

/*
================
idProjectile::Restore
================
*/
void idProjectile::Restore( idRestoreGame *savefile ) {

	owner.Restore( savefile );

	savefile->Read( &projectileFlags, sizeof( projectileFlags ) );
	LittleBitField( &projectileFlags, sizeof( projectileFlags ) );

	savefile->ReadFloat( thrust );
	savefile->ReadInt( thrust_end );

	savefile->ReadRenderLight( renderLight );
//	savefile->ReadInt( (int &)lightDefHandle );
	savefile->ReadVec3( lightOffset );
	savefile->ReadInt( lightStartTime );
	savefile->ReadInt( lightEndTime );
	savefile->ReadVec3( lightColor );

//	savefile->ReadParticle( smokeFly );
	savefile->ReadInt( smokeFlyTime );

	savefile->ReadInt( (int &)state );

	savefile->ReadFloat( damagePower );

	savefile->ReadStaticObject( physicsObj );
	RestorePhysics( &physicsObj );

	savefile->ReadStaticObject( thruster );
	thruster.SetPhysics( &physicsObj );

	if ( smokeFly != NULL ) {
		idVec3 dir;
		dir = physicsObj.GetLinearVelocity();
		dir.NormalizeFast();
//		gameLocal.smokeParticles->EmitSmoke( smokeFly, gameLocal.time, gameLocal.random.RandomFloat(), GetPhysics()->GetOrigin(), GetPhysics()->GetAxis() );
	}
}

/*
================
idProjectile::GetOwner
================
*/
idEntity *idProjectile::GetOwner( void ) const {
	return owner.GetEntity();
}

/*
================
idProjectile::Create
================
*/
void idProjectile::Create( idEntity *owner, const idVec3 &start, const idVec3 &dir ) {
	idDict		args;
	idStr		shaderName;
	idVec3		light_color;
	idVec3		light_offset;
	idVec3		tmp;
	idMat3		axis;

	Unbind();

	// align z-axis of model with the direction
	axis = dir.ToMat3();
	tmp = axis[2];
	axis[2] = axis[0];
	axis[0] = -tmp;

	physicsObj.SetOrigin( start );
	physicsObj.SetAxis( axis );

	physicsObj.GetClipModel()->SetOwner( owner );

	this->owner = owner;


	
	renderLight = gameRenderWorld->AllocRenderLight();
	float radius = spawnArgs.GetFloat("light_radius");
	spawnArgs.GetVector("light_color", "1 1 1", light_color);

	//renderLight->SetShader(declManager->FindMaterial( shaderName, false ));
	renderLight->SetPointLight(true);

	renderLight->SetLightRadius(idVec3(radius, radius, radius));
	renderLight->SetLightColor(light_color);

	spawnArgs.GetVector( "light_offset", "0 0 0", lightOffset );

	lightStartTime = 0;
	lightEndTime = 0;
	smokeFlyTime = 0;

	damagePower = 1.0f;

	UpdateVisuals();

	state = CREATED;

	if ( spawnArgs.GetBool( "net_fullphysics" ) ) {
		netSyncPhysics = true;
	}
}

/*
=================
idProjectile::~idProjectile
=================
*/
idProjectile::~idProjectile() {
	StopSound( SND_CHANNEL_ANY, false );
	FreeLightDef();

	if(trailFX != NULL) {
		gameLocal.RemoveEffect(trailFX);
		trailFX = NULL;
	}
}

/*
=================
idProjectile::FreeLightDef
=================
*/
void idProjectile::FreeLightDef( void ) {
	if (renderLight != NULL ) {
		gameRenderWorld->FreeRenderLight( renderLight );
		renderLight = NULL;
	}
}

/*
=================
idProjectile::Launch
=================
*/
void idProjectile::Launch( const idVec3 &start, const idVec3 &dir, const idVec3 &pushVelocity, const float timeSinceFire, const float launchPower, const float dmgPower ) {
	float			fuse;
	float			startthrust;
	float			endthrust;
	idVec3			velocity;
	idAngles		angular_velocity;
	float			linear_friction;
	float			angular_friction;
	float			contact_friction;
	float			bounce;
	float			mass;
	float			speed;
	float			gravity;
	idVec3			gravVec;
	idVec3			tmp;
	idMat3			axis;
	int				thrust_start;
	int				contents;
	int				clipMask;

	// allow characters to throw projectiles during cinematics, but not the player
	if ( owner.GetEntity() && !owner.GetEntity()->IsType( idPlayer::Type ) ) {
		cinematic = owner.GetEntity()->cinematic;
	} else {
		cinematic = false;
	}

	thrust				= spawnArgs.GetFloat( "thrust" );
	startthrust			= spawnArgs.GetFloat( "thrust_start" );
	endthrust			= spawnArgs.GetFloat( "thrust_end" );

	spawnArgs.GetVector( "velocity", "0 0 0", velocity );
	
	speed = velocity.Length() * launchPower;

	damagePower = dmgPower;

	spawnArgs.GetAngles( "angular_velocity", "0 0 0", angular_velocity );

	linear_friction		= spawnArgs.GetFloat( "linear_friction" );
	angular_friction	= spawnArgs.GetFloat( "angular_friction" );
	contact_friction	= spawnArgs.GetFloat( "contact_friction" );
	bounce				= spawnArgs.GetFloat( "bounce" );
	mass				= spawnArgs.GetFloat( "mass" );
	gravity				= spawnArgs.GetFloat( "gravity" );
	fuse				= spawnArgs.GetFloat( "fuse" );

	projectileFlags.detonate_on_world	= spawnArgs.GetBool( "detonate_on_world" );
	projectileFlags.detonate_on_actor	= spawnArgs.GetBool( "detonate_on_actor" );
	projectileFlags.randomShaderSpin	= spawnArgs.GetBool( "random_shader_spin" );

	if ( mass <= 0 ) {
		gameLocal.Error( "Invalid mass on '%s'\n", GetEntityDefName() );
	}

	thrust *= mass;
	thrust_start = SEC2MS( startthrust ) + gameLocal.time;
	thrust_end = SEC2MS( endthrust ) + gameLocal.time;

	lightStartTime = 0;
	lightEndTime = 0;

	if ( health ) {
		fl.takedamage = true;
	}

	gravVec = gameLocal.GetGravity();
	gravVec.NormalizeFast();

	Unbind();

	// align z-axis of model with the direction
	axis = dir.ToMat3();
	tmp = axis[2];
	axis[2] = axis[0];
	axis[0] = -tmp;

	contents = 0;
	clipMask = MASK_SHOT_RENDERMODEL;
	if ( spawnArgs.GetBool( "detonate_on_trigger" ) ) {
		contents |= CONTENTS_TRIGGER;
	}
	if ( !spawnArgs.GetBool( "no_contents" ) ) {
		contents |= CONTENTS_PROJECTILE;
		clipMask |= CONTENTS_PROJECTILE;
	}

	// don't do tracers on client, we don't know origin and direction
	if ( spawnArgs.GetBool( "tracers" ) && gameLocal.random.RandomFloat() > 0.5f ) {
		SetModel( spawnArgs.GetString( "model_tracer" ) );
		projectileFlags.isTracer = true;
	}

	physicsObj.SetMass( mass );
	physicsObj.SetFriction( linear_friction, angular_friction, contact_friction );
	if ( contact_friction == 0.0f ) {
		physicsObj.NoContact();
	}
	physicsObj.SetBouncyness( bounce );
	physicsObj.SetGravity( gravVec * gravity );
	physicsObj.SetContents( contents );
	physicsObj.SetClipMask( clipMask );
	physicsObj.SetLinearVelocity( axis[ 2 ] * speed + pushVelocity );
	physicsObj.SetAngularVelocity( angular_velocity.ToAngularVelocity() * axis );
	physicsObj.SetOrigin( start );
	physicsObj.SetAxis( axis );

	thruster.SetPosition( &physicsObj, 0, idVec3( GetPhysics()->GetBounds()[ 0 ].x, 0, 0 ) );

	if ( !common->IsClient() ) {
		if ( fuse <= 0 ) {
			// run physics for 1 second
			RunPhysics();
			PostEventMS( &EV_Remove, spawnArgs.GetInt( "remove_time", "1500" ) );
		} else if ( spawnArgs.GetBool( "detonate_on_fuse" ) ) {
			fuse -= timeSinceFire;
			if ( fuse < 0.0f ) {
				fuse = 0.0f;
			}
			PostEventSec( &EV_Explode, fuse );
		} else {
			fuse -= timeSinceFire;
			if ( fuse < 0.0f ) {
				fuse = 0.0f;
			}
			PostEventSec( &EV_Fizzle, fuse );
		}
	}

	if ( projectileFlags.isTracer ) {
		StartSound( "snd_tracer", SND_CHANNEL_BODY, 0, false, NULL );
	} else {
		StartSound( "snd_fly", SND_CHANNEL_BODY, 0, false, NULL );
	}

	smokeFlyTime = 0;
	const char *smokeName = spawnArgs.GetString( "smoke_fly" );
	if ( *smokeName != '\0' ) {
//		smokeFly = static_cast<const idDeclParticle *>( declManager->FindType( DECL_PARTICLE, smokeName ) );
		smokeFlyTime = gameLocal.time;
	}

	// used for the plasma bolts but may have other uses as well
	if ( projectileFlags.randomShaderSpin ) {
		float f = gameLocal.random.RandomFloat();
		f *= 0.5f;
		renderEntity->SetShaderParms(SHADERPARM_DIVERSITY, f);
	}

	UpdateVisuals();

	state = LAUNCHED;
}

/*
================
idProjectile::Think
================
*/
void idProjectile::Think( void ) {

	if ( thinkFlags & TH_THINK ) {
		if ( thrust && ( gameLocal.time < thrust_end ) ) {
			// evaluate force
			thruster.SetForce( GetPhysics()->GetAxis()[ 0 ] * thrust );
			thruster.Evaluate( gameLocal.time );
		}
	}

	// run physics
	RunPhysics();

	Present();

	// If we have a rocket trail, update the position.
	if (trailFX != NULL) {
		idVec3 dir = -GetPhysics()->GetLinearVelocity();
		dir.Normalize();

		trailFX->renderEntity->SetOrigin(GetPhysics()->GetOrigin());
		trailFX->renderEntity->SetAxis(dir.ToMat3());
	}

	// add the light
	if (renderLight && renderLight->GetLightRadius().x > 0.0f && g_projectileLights.GetBool() ) {
		renderLight->SetOrigin(GetPhysics()->GetOrigin() + GetPhysics()->GetAxis() * lightOffset);
		renderLight->SetAxis(GetPhysics()->GetAxis());
		//if ( ( lightDefHandle != -1 ) ) {
		//	if ( lightEndTime > 0 && gameLocal.time <= lightEndTime + gameLocal.GetMSec() ) {
		//		idVec3 color( 0, 0, 0 );
		//		if ( gameLocal.time < lightEndTime ) {
		//			float frac = ( float )( gameLocal.time - lightStartTime ) / ( float )( lightEndTime - lightStartTime );
		//			color.Lerp( lightColor, color, frac );
		//		} 
		//		renderLight->SetShaderParam(SHADERPARM_RED, color.x);
		//		renderLight->SetShaderParam(SHADERPARM_GREEN, color.y);
		//		renderLight->SetShaderParam(SHADERPARM_BLUE, color.z);
		//	} 
		//	renderLight->UpdateRenderLight();
		//}
	}
}

/*
=================
idProjectile::Collide
=================
*/
bool idProjectile::Collide( const trace_t &collision, const idVec3 &velocity ) {
	idEntity	*ent;
	idEntity	*ignore;
	const char	*damageDefName;
	idVec3		dir;
	float		push;
	float		damageScale;

	if ( state == EXPLODED || state == FIZZLED ) {
		return true;
	}

	// predict the explosion
	if ( common->IsClient() ) {
		if ( ClientPredictionCollide( this, spawnArgs, collision, velocity, !spawnArgs.GetBool( "net_instanthit" ) ) ) {
			Explode( collision, NULL );
			return true;
		}
		return false;
	}

	// remove projectile when a 'noimpact' surface is hit
	if ( ( collision.c.material != NULL ) && ( collision.c.material->GetSurfaceFlags() & SURF_NOIMPACT ) ) {
		PostEventMS( &EV_Remove, 0 );
		common->DPrintf( "Projectile collision no impact\n" );
		return true;
	}

	// get the entity the projectile collided with
	ent = gameLocal.entities[ collision.c.entityNum ];
	if ( ent == owner.GetEntity() ) {
		assert( 0 );
		return true;
	}

	// just get rid of the projectile when it hits a player in noclip
	if ( ent->IsType( idPlayer::Type ) && static_cast<idPlayer *>( ent )->noclip ) {
		PostEventMS( &EV_Remove, 0 );
		return true;
	}

	// direction of projectile
	dir = velocity;
	dir.Normalize();

	// projectiles can apply an additional impulse next to the rigid body physics impulse
	if ( spawnArgs.GetFloat( "push", "0", push ) && push > 0.0f ) {
		ent->ApplyImpulse( this, collision.c.id, collision.c.point, push * dir );
	}

	// MP: projectiles open doors
	if ( common->IsMultiplayer() && ent->IsType( idDoor::Type ) && !static_cast< idDoor * >(ent)->IsOpen() && !ent->spawnArgs.GetBool( "no_touch" ) ) {
		ent->ProcessEvent( &EV_Activate , this );
	}

	if ( ent->IsType( idActor::Type ) || ( ent->IsType( idAFAttachment::Type ) && static_cast<const idAFAttachment*>(ent)->GetBody()->IsType( idActor::Type ) ) ) {
		if ( !projectileFlags.detonate_on_actor ) {
			return false;
		}
	} else {
		if ( !projectileFlags.detonate_on_world ) {
			if ( !StartSound( "snd_ricochet", SND_CHANNEL_ITEM, 0, true, NULL ) ) {
				float len = velocity.Length();
				if ( len > BOUNCE_SOUND_MIN_VELOCITY ) {
					SetSoundVolume( len > BOUNCE_SOUND_MAX_VELOCITY ? 1.0f : idMath::Sqrt( len - BOUNCE_SOUND_MIN_VELOCITY ) * ( 1.0f / idMath::Sqrt( BOUNCE_SOUND_MAX_VELOCITY - BOUNCE_SOUND_MIN_VELOCITY ) ) );
					StartSound( "snd_bounce", SND_CHANNEL_ANY, 0, true, NULL );
				}
			}
			return false;
		}
	}

	SetOrigin( collision.endpos );
	SetAxis( collision.endAxis );

	// unlink the clip model because we no longer need it
	GetPhysics()->UnlinkClip();

	damageDefName = spawnArgs.GetString( "def_damage" );

	ignore = NULL;

	// if the hit entity takes damage
	if ( ent->fl.takedamage ) {
		if ( damagePower ) {
			damageScale = damagePower;
		} else {
			damageScale = 1.0f;
		}

		// if the projectile owner is a player
		if ( owner.GetEntity() && owner.GetEntity()->IsType( idPlayer::Type ) ) {
			// if the projectile hit an actor
			if ( ent->IsType( idActor::Type ) ) {
				idPlayer *player = static_cast<idPlayer *>( owner.GetEntity() );
				player->AddProjectileHits( 1 );
				damageScale *= player->PowerUpModifier( PROJECTILE_DAMAGE );
			}
		}

		if ( damageDefName[0] != '\0' ) {
			ent->Damage( this, owner.GetEntity(), dir, damageDefName, damageScale, CLIPMODEL_ID_TO_JOINT_HANDLE( collision.c.id ) );
			ignore = ent;
		}
	}

	// if the projectile causes a damage effect
	if ( spawnArgs.GetBool( "impact_damage_effect" ) ) {
		// if the hit entity has a special damage effect
		if ( ent->spawnArgs.GetBool( "bleed" ) ) {
			ent->AddDamageEffect( collision, velocity, damageDefName );
		} else {
			AddDefaultDamageEffect( collision, velocity );
		}
	}

	Explode( collision, ignore );

	return true;
}

/*
=================
idProjectile::DefaultDamageEffect
=================
*/
void idProjectile::DefaultDamageEffect( idEntity *soundEnt, const idDict &projectileDef, const trace_t &collision, const idVec3 &velocity ) {
	const char *decal, *sound, *typeName;
	surfTypes_t materialType;

	if ( collision.c.material != NULL ) {
		materialType = collision.c.material->GetSurfaceType();
	} else {
		materialType = SURFTYPE_METAL;
	}

	// get material type name
	typeName = gameLocal.sufaceTypeNames[ materialType ];

	// play impact sound
	sound = projectileDef.GetString( va( "snd_%s", typeName ) );
	if ( *sound == '\0' ) {
		sound = projectileDef.GetString( "snd_metal" );
	}
	if ( *sound == '\0' ) {
		sound = projectileDef.GetString( "snd_impact" );
	}
	if ( *sound != '\0' ) {
		soundEnt->StartSoundShader( declManager->FindSound( sound ), SND_CHANNEL_BODY, 0, false, NULL );
	}

	// project decal
	decal = projectileDef.GetString( va( "mtr_detonate_%s", typeName ) );
	if ( *decal == '\0' ) {
		decal = projectileDef.GetString( "mtr_detonate" );
	}
	if ( *decal != '\0' ) {
		gameLocal.ProjectDecal( collision.c.point, -collision.c.normal, 8.0f, true, projectileDef.GetFloat( "decal_size", "6.0" ), decal );
	}
}

/*
=================
idProjectile::AddDefaultDamageEffect
=================
*/
void idProjectile::AddDefaultDamageEffect( const trace_t &collision, const idVec3 &velocity ) {
	DefaultDamageEffect( this, spawnArgs, collision, velocity );

	if ( common->IsServer() && fl.networkSync ) {
		idBitMsg	msg;
		byte		msgBuf[MAX_EVENT_PARAM_SIZE];
		int			excludeClient;

		if ( spawnArgs.GetBool( "net_instanthit" ) ) {
			excludeClient = owner.GetEntityNum();
		} else {
			excludeClient = -1;
		}

		msg.Init( msgBuf, sizeof( msgBuf ) );
		msg.BeginWriting();
		msg.WriteFloat( collision.c.point[0] );
		msg.WriteFloat( collision.c.point[1] );
		msg.WriteFloat( collision.c.point[2] );
		msg.WriteDir( collision.c.normal, 24 );
		msg.WriteLong( ( collision.c.material != NULL ) ? gameLocal.ServerRemapDecl( -1, DECL_MATERIAL, collision.c.material->Index() ) : -1 );
		msg.WriteFloat( velocity[0], 5, 10 );
		msg.WriteFloat( velocity[1], 5, 10 );
		msg.WriteFloat( velocity[2], 5, 10 );
		ServerSendEvent( EVENT_DAMAGE_EFFECT, &msg, false, excludeClient );
	}
}

/*
================
idProjectile::Killed
================
*/
void idProjectile::Killed( idEntity *inflictor, idEntity *attacker, int damage, const idVec3 &dir, int location ) {
	if ( spawnArgs.GetBool( "detonate_on_death" ) ) {
		trace_t collision;

		memset( &collision, 0, sizeof( collision ) );
		collision.endAxis = GetPhysics()->GetAxis();
		collision.endpos = GetPhysics()->GetOrigin();
		collision.c.point = GetPhysics()->GetOrigin();
		collision.c.normal.Set( 0, 0, 1 );
		Explode( collision, NULL );
		physicsObj.ClearContacts();
		physicsObj.PutToRest();
	} else {
		Fizzle();
	}
}

/*
================
idProjectile::Fizzle
================
*/
void idProjectile::Fizzle( void ) {

	if ( state == EXPLODED || state == FIZZLED ) {
		return;
	}

	StopSound( SND_CHANNEL_BODY, false );
	StartSound( "snd_fizzle", SND_CHANNEL_BODY, 0, false, NULL );

	// fizzle FX
	const char *psystem = spawnArgs.GetString( "smoke_fuse" );
	if ( psystem && *psystem ) {
//FIXME:SMOKE		gameLocal.particles->SpawnParticles( GetPhysics()->GetOrigin(), vec3_origin, psystem );
	}

	// we need to work out how long the effects last and then remove them at that time
	// for example, bullets have no real effects
	if ( smokeFly && smokeFlyTime ) {
		smokeFlyTime = 0;
	}

	fl.takedamage = false;
	physicsObj.SetContents( 0 );
	physicsObj.GetClipModel()->Unlink();
	physicsObj.PutToRest();

	Hide();
	FreeLightDef();

	state = FIZZLED;

	if ( common->IsClient() ) {
		return;
	}

	CancelEvents( &EV_Fizzle );
	PostEventMS( &EV_Remove, spawnArgs.GetInt( "remove_time", "1500" ) );
}

/*
================
idProjectile::Event_RadiusDamage
================
*/
void idProjectile::Event_RadiusDamage( idEntity *ignore ) {
	const char *splash_damage = spawnArgs.GetString( "def_splash_damage" );
	if ( splash_damage[0] != '\0' ) {
		gameLocal.RadiusDamage( physicsObj.GetOrigin(), this, owner.GetEntity(), ignore, this, splash_damage, damagePower );
	}
}

/*
================
idProjectile::Event_RadiusDamage
================
*/
void idProjectile::Event_GetProjectileState( void ) {
	idThread::ReturnInt( state );
}

/*
================
idProjectile::Explode
================
*/
void idProjectile::Explode( const trace_t &collision, idEntity *ignore ) {
	const char *fxname, *light_shader, *sndExplode, *hitfxname;
	float		light_fadetime;
	idVec3		normal;
	int			removeTime;

	if ( state == EXPLODED || state == FIZZLED ) {
		return;
	}

	idVec3 dir = collision.endpos - owner->GetOrigin();
	dir.Normalize();

	// Alert any bots near were we just exploded.
	gameLocal.AlertBots(owner->Cast<idPlayer>(), collision.endpos);

	// Spawn Debris
	if (common->IsServer() && collision.c.material != NULL && collision.c.entityNum == ENTITYNUM_WORLD) {
		idVec3 reflectedDir = idMath::ReflectVector(dir, collision.c.normal);
		gameLocal.SpawnDebris(collision.endpos, reflectedDir.ToMat3(), collision.c.material->GetName());
	}

	// Play the impact particle.
	gameLocal.CreateEffect(spawnArgs.GetString("impact_fx"), collision.c.point, collision.c.normal.ToAngles().ToMat3(), false);

	// Remove the trail FX.
	if(trailFX) {
		gameLocal.RemoveEffect(trailFX);
		trailFX = NULL;
	}

	// stop sound
	StopSound( SND_CHANNEL_BODY2, false );

	// play explode sound
	switch ( ( int ) damagePower ) {
		case 2: sndExplode = "snd_explode2"; break;
		case 3: sndExplode = "snd_explode3"; break;
		case 4: sndExplode = "snd_explode4"; break;
		default: sndExplode = "snd_explode"; break;
	}
	StartSound( sndExplode, SND_CHANNEL_BODY, 0, true, NULL );

	// we need to work out how long the effects last and then remove them at that time
	// for example, bullets have no real effects
	if ( smokeFly && smokeFlyTime ) {
		smokeFlyTime = 0;
	}

	Hide();
	FreeLightDef();

	if ( spawnArgs.GetVector( "detonation_axis", "", normal ) ) {
		GetPhysics()->SetAxis( normal.ToMat3() );
	}
	GetPhysics()->SetOrigin( collision.endpos + 2.0f * collision.c.normal );

	// default remove time
	removeTime = spawnArgs.GetInt( "remove_time", "1500" );

	// change the model, usually to a PRT
	fxname = NULL;
	if ( g_testParticle.GetInteger() == TEST_PARTICLE_IMPACT ) {
		fxname = g_testParticleName.GetString();
	} else {
		fxname = spawnArgs.GetString( "model_detonate" );
	}

	int surfaceType = collision.c.material != NULL ? collision.c.material->GetSurfaceType() : SURFTYPE_METAL;
	if ( !( fxname && *fxname ) ) {
		if ( ( surfaceType == SURFTYPE_NONE ) || ( surfaceType == SURFTYPE_METAL ) || ( surfaceType == SURFTYPE_STONE ) ) {
			fxname = spawnArgs.GetString( "model_smokespark" );
		} else if ( surfaceType == SURFTYPE_RICOCHET ) {
			fxname = spawnArgs.GetString( "model_ricochet" );
		} else {
			fxname = spawnArgs.GetString( "model_smoke" );
		}
	}

	if ( fxname && *fxname ) {
		SetModel( fxname );
		//renderEntity.shaderParms[SHADERPARM_RED] = 
		//renderEntity.shaderParms[SHADERPARM_GREEN] = 
		//renderEntity.shaderParms[SHADERPARM_BLUE] = 
		//renderEntity.shaderParms[SHADERPARM_ALPHA] = 1.0f;
		//renderEntity.shaderParms[SHADERPARM_TIMEOFFSET] = -MS2SEC( gameLocal.time );
		//renderEntity.shaderParms[SHADERPARM_DIVERSITY] = gameLocal.random.CRandomFloat();
		Show();
		removeTime = ( removeTime > 3000 ) ? removeTime : 3000;
	}

	// explosion light
	//light_shader = spawnArgs.GetString( "mtr_explode_light_shader" );
	//if ( *light_shader ) {
	//	renderLight.shader = declManager->FindMaterial( light_shader, false );
	//	renderLight.pointLight = true;
	//	renderLight.lightRadius[0] =
	//	renderLight.lightRadius[1] =
	//	renderLight.lightRadius[2] = spawnArgs.GetFloat( "explode_light_radius" );
	//	spawnArgs.GetVector( "explode_light_color", "1 1 1", lightColor );
	//	renderLight.shaderParms[SHADERPARM_RED] = lightColor.x;
	//	renderLight.shaderParms[SHADERPARM_GREEN] = lightColor.y;
	//	renderLight.shaderParms[SHADERPARM_BLUE] = lightColor.z;
	//	renderLight.shaderParms[SHADERPARM_ALPHA] = 1.0f;
	//	renderLight.shaderParms[SHADERPARM_TIMEOFFSET] = -MS2SEC( gameLocal.time );
	//	light_fadetime = spawnArgs.GetFloat( "explode_light_fadetime", "0.5" );
	//	lightStartTime = gameLocal.time;
	//	lightEndTime = gameLocal.time + SEC2MS( light_fadetime );
	//	BecomeActive( TH_THINK );
	//}

	fl.takedamage = false;
	physicsObj.SetContents( 0 );
	physicsObj.PutToRest();

	state = EXPLODED;

	if ( common->IsClient() ) {
		return;
	}

	// alert the ai
	gameLocal.AlertAI( owner.GetEntity() );

	// bind the projectile to the impact entity if necesary
	if ( gameLocal.entities[collision.c.entityNum] && spawnArgs.GetBool( "bindOnImpact" ) ) {
		Bind( gameLocal.entities[collision.c.entityNum], true );
	}

	// splash damage
	if ( !projectileFlags.noSplashDamage ) {
		float delay = spawnArgs.GetFloat( "delay_splash" );
		if ( delay ) {
			if ( removeTime < delay * 1000 ) {
				removeTime = ( delay + 0.10 ) * 1000;
			}
			PostEventSec( &EV_RadiusDamage, delay, ignore );
		} else {
			Event_RadiusDamage( ignore );
		}
	}

	CancelEvents( &EV_Explode );
	PostEventMS( &EV_Remove, removeTime );
}

/*
================
idProjectile::GetVelocity
================
*/
idVec3 idProjectile::GetVelocity( const idDict *projectile ) {
	idVec3 velocity;

	projectile->GetVector( "velocity", "0 0 0", velocity );
	return velocity;
}

/*
================
idProjectile::GetGravity
================
*/
idVec3 idProjectile::GetGravity( const idDict *projectile ) {
	float gravity;

	gravity = projectile->GetFloat( "gravity" );
	return idVec3( 0, 0, -gravity );
}

/*
================
idProjectile::Event_Explode
================
*/
void idProjectile::Event_Explode( void ) {
	trace_t collision;

	memset( &collision, 0, sizeof( collision ) );
	collision.endAxis = GetPhysics()->GetAxis();
	collision.endpos = GetPhysics()->GetOrigin();
	collision.c.point = GetPhysics()->GetOrigin();
	collision.c.normal.Set( 0, 0, 1 );
	AddDefaultDamageEffect( collision, collision.c.normal );
	Explode( collision, NULL );
}

/*
================
idProjectile::Event_Fizzle
================
*/
void idProjectile::Event_Fizzle( void ) {
	Fizzle();
}

/*
================
idProjectile::Event_Touch
================
*/
void idProjectile::Event_Touch( idEntity *other, trace_t *trace ) {

	if ( IsHidden() ) {
		return;
	}

	if ( other != owner.GetEntity() ) {
		trace_t collision;

		memset( &collision, 0, sizeof( collision ) );
		collision.endAxis = GetPhysics()->GetAxis();
		collision.endpos = GetPhysics()->GetOrigin();
		collision.c.point = GetPhysics()->GetOrigin();
		collision.c.normal.Set( 0, 0, 1 );
		AddDefaultDamageEffect( collision, collision.c.normal );
		Explode( collision, NULL );
	}
}

/*
=================
idProjectile::ClientPredictionCollide
=================
*/
bool idProjectile::ClientPredictionCollide( idEntity *soundEnt, const idDict &projectileDef, const trace_t &collision, const idVec3 &velocity, bool addDamageEffect ) {
	idEntity *ent;

	// remove projectile when a 'noimpact' surface is hit
	if ( collision.c.material && ( collision.c.material->GetSurfaceFlags() & SURF_NOIMPACT ) ) {
		return false;
	}

	// get the entity the projectile collided with
	ent = gameLocal.entities[ collision.c.entityNum ];
	if ( ent == NULL ) {
		return false;
	}

	// don't do anything if hitting a noclip player
	if ( ent->IsType( idPlayer::Type ) && static_cast<idPlayer *>( ent )->noclip ) {
		return false;
	}

	if ( ent->IsType( idActor::Type ) || ( ent->IsType( idAFAttachment::Type ) && static_cast<const idAFAttachment*>(ent)->GetBody()->IsType( idActor::Type ) ) ) {
		if ( !projectileDef.GetBool( "detonate_on_actor" ) ) {
			return false;
		}
	} else {
		if ( !projectileDef.GetBool( "detonate_on_world" ) ) {
			return false;
		}
	}

	// if the projectile causes a damage effect
	if ( addDamageEffect && projectileDef.GetBool( "impact_damage_effect" ) ) {
		// if the hit entity does not have a special damage effect
		if ( !ent->spawnArgs.GetBool( "bleed" ) ) {
			// predict damage effect
			DefaultDamageEffect( soundEnt, projectileDef, collision, velocity );
		}
	}
	return true;
}

/*
================
idProjectile::ClientPredictionThink
================
*/
void idProjectile::ClientPredictionThink( void ) {
	if ( renderEntity->GetRenderModel() == NULL ) {
		return;
	}
	Think();
}

/*
================
idProjectile::WriteToSnapshot
================
*/
void idProjectile::WriteToSnapshot( idBitMsgDelta &msg ) const {
	msg.WriteBits( owner.GetSpawnId(), 32 );
	msg.WriteBits( state, 3 );
	msg.WriteBits( fl.hidden, 1 );
	if ( netSyncPhysics ) {
		msg.WriteBits( 1, 1 );
		physicsObj.WriteToSnapshot( msg );
	} else {
		msg.WriteBits( 0, 1 );
		const idVec3 &origin	= physicsObj.GetOrigin();
		const idVec3 &velocity	= physicsObj.GetLinearVelocity();

		msg.WriteFloat( origin.x );
		msg.WriteFloat( origin.y );
		msg.WriteFloat( origin.z );

		msg.WriteDeltaFloat( 0.0f, velocity[0], RB_VELOCITY_EXPONENT_BITS, RB_VELOCITY_MANTISSA_BITS );
		msg.WriteDeltaFloat( 0.0f, velocity[1], RB_VELOCITY_EXPONENT_BITS, RB_VELOCITY_MANTISSA_BITS );
		msg.WriteDeltaFloat( 0.0f, velocity[2], RB_VELOCITY_EXPONENT_BITS, RB_VELOCITY_MANTISSA_BITS );		
	}
}

/*
================
idProjectile::ReadFromSnapshot
================
*/
void idProjectile::ReadFromSnapshot( const idBitMsgDelta &msg ) {
	projectileState_t newState;

	owner.SetSpawnId( msg.ReadBits( 32 ) );
	newState = (projectileState_t) msg.ReadBits( 3 );
	if ( msg.ReadBits( 1 ) ) {
		Hide();
	} else {
		Show();
	}

	while( state != newState ) {
		switch( state ) {
			case SPAWNED: {
				Create( owner.GetEntity(), vec3_origin, idVec3( 1, 0, 0 ) );
				break;
			}
			case CREATED: {
				// the right origin and direction are required if you want bullet traces
				Launch( vec3_origin, idVec3( 1, 0, 0 ), vec3_origin );
				break;
			}
			case LAUNCHED: {
				if ( newState == FIZZLED ) {
					Fizzle();
				} else {
					trace_t collision;
					memset( &collision, 0, sizeof( collision ) );
					collision.endAxis = GetPhysics()->GetAxis();
					collision.endpos = GetPhysics()->GetOrigin();
					collision.c.point = GetPhysics()->GetOrigin();
					collision.c.normal.Set( 0, 0, 1 );
					Explode( collision, NULL );
				}
				break;
			}
			case FIZZLED:
			case EXPLODED: {
				StopSound( SND_CHANNEL_BODY2, false );
				gameEdit->ParseSpawnArgsToRenderEntity( &spawnArgs, renderEntity );
				state = SPAWNED;
				break;
			}
		}
	}

	if ( msg.ReadBits( 1 ) ) {
		physicsObj.ReadFromSnapshot( msg );
	} else {
		idVec3 origin;
		idVec3 velocity;
		idVec3 tmp;
		idMat3 axis;

		origin.x = msg.ReadFloat();
		origin.y = msg.ReadFloat();
		origin.z = msg.ReadFloat();

		velocity.x = msg.ReadDeltaFloat( 0.0f, RB_VELOCITY_EXPONENT_BITS, RB_VELOCITY_MANTISSA_BITS );
		velocity.y = msg.ReadDeltaFloat( 0.0f, RB_VELOCITY_EXPONENT_BITS, RB_VELOCITY_MANTISSA_BITS );
		velocity.z = msg.ReadDeltaFloat( 0.0f, RB_VELOCITY_EXPONENT_BITS, RB_VELOCITY_MANTISSA_BITS );

		physicsObj.SetOrigin( origin );
		physicsObj.SetLinearVelocity( velocity );

		// align z-axis of model with the direction
		velocity.NormalizeFast();
		axis = velocity.ToMat3();
		tmp = axis[2];
		axis[2] = axis[0];
		axis[0] = -tmp;
		physicsObj.SetAxis( axis );
	}

	if ( msg.HasChanged() ) {
		UpdateVisuals();
	}
}

/*
================
idProjectile::ClientReceiveEvent
================
*/
bool idProjectile::ClientReceiveEvent( int event, int time, const idBitMsg &msg ) {
	trace_t collision;
	idVec3 velocity;

	switch( event ) {
		case EVENT_DAMAGE_EFFECT: {
			memset( &collision, 0, sizeof( collision ) );
			collision.c.point[0] = msg.ReadFloat();
			collision.c.point[1] = msg.ReadFloat();
			collision.c.point[2] = msg.ReadFloat();
			collision.c.normal = msg.ReadDir( 24 );
			int index = gameLocal.ClientRemapDecl( DECL_MATERIAL, msg.ReadLong() );
			collision.c.material = ( index != -1 ) ? static_cast<const idMaterial *>( declManager->DeclByIndex( DECL_MATERIAL, index ) ) : NULL;
			velocity[0] = msg.ReadFloat( 5, 10 );
			velocity[1] = msg.ReadFloat( 5, 10 );
			velocity[2] = msg.ReadFloat( 5, 10 );
			DefaultDamageEffect( this, spawnArgs, collision, velocity );
			return true;
		}
		default: {
			return idEntity::ClientReceiveEvent( event, time, msg );
		}
	}
	return false;
}
