// ClientModel.cpp
//

#include "game_precompiled.h"
#pragma hdrstop

#include "../Game_local.h"

/*
===============================================================================

rvClientModel

===============================================================================
*/
CLASS_DECLARATION( rvClientEntity, rvClientModel )
END_CLASS


/*
================
rvClientModel::rvClientModel
================
*/
rvClientModel::rvClientModel ( void ) {
	memset ( &renderEntity, 0, sizeof(renderEntity) );
	worldAxis = mat3_identity;
	renderEntity = NULL;
}

/*
================
rvClientModel::~rvClientModel
================
*/
rvClientModel::~rvClientModel ( void ) {
	FreeEntityDef ( );
}

/*
================
rvClientModel::FreeEntityDef
================
*/
void rvClientModel::FreeEntityDef ( void ) {
	if (renderEntity != NULL) {
		gameRenderWorld->FreeRenderEntity(renderEntity);
		renderEntity = NULL;
	}	
}

/*
================
rvClientModel::Spawn
================
*/
void rvClientModel::Spawn ( void ) {
	const char* spawnarg;

	rvClientMoveable::Spawn();

	spawnArgs.GetString ( "classname", "", classname );

	renderEntity = gameRenderWorld->AllocRenderEntity();

	// parse static models the same way the editor display does
	gameEdit->ParseSpawnArgsToRenderEntity( &spawnArgs, renderEntity );

	renderEntity->SetEntityNum(entityNumber);
	renderEntity->SetLightChannel(LIGHT_CHANNEL_WORLD, true);

	spawnarg = spawnArgs.GetString( "model" );
	if ( spawnarg && *spawnarg ) {
		SetModel( spawnarg );
	}
}

/*
================
rvClientModel::Think
================
*/
void rvClientModel::Think ( void ) {
	rvClientMoveable::Think();

	if( bindMaster && (bindMaster->GetRenderEntity()->GetRenderModel() && bindMaster->GetModelDefHandle() == -1) ) {
		return;
	}

	idVec3 delta = renderEntity->GetOrigin() - worldOrigin;
	if(delta.Length() <= 0) {
		stopSimulation = true;
	}

	renderEntity->SetOrigin(worldOrigin);
	renderEntity->SetAxis(worldAxis * scale.GetCurrentValue(gameLocal.GetTime()));

	UpdateBind();
	Present();
}

/*
================
rvClientModel::Present
================
*/
void rvClientModel::Present(void) {
	// Hide client entities bound to a hidden entity
	if ( bindMaster && (bindMaster->IsHidden ( ) || (bindMaster->GetRenderEntity()->GetRenderModel() && bindMaster->GetModelDefHandle() == -1) ) ) {
		return;
	}

	renderEntity->SetOrigin(worldOrigin);
	renderEntity->SetAxis(worldAxis);
	renderEntity->UpdateRenderEntity();
}

/*
================
rvClientModel::SetCustomShader
================
*/
bool rvClientModel::SetCustomShader ( const char* shaderName ) {
	if ( shaderName == NULL ) {
		return false;
	}
	
	const idMaterial* material = declManager->FindMaterial( shaderName );

	if ( material == NULL ) {
		return false;
	}
	
	renderEntity->SetCustomShader(material);

	return true;
}

/*
================
rvClientModel::Save
================
*/
void rvClientModel::Save( idSaveGame *savefile ) const {
	savefile->WriteRenderEntity( renderEntity );
	//savefile->WriteInt( entityDefHandle );

	savefile->WriteString ( classname );	// cnicholson: Added unsaved var

}

/*
================
rvClientModel::Restore
================
*/
void rvClientModel::Restore( idRestoreGame *savefile ) {
	savefile->ReadRenderEntity( renderEntity );
//	savefile->ReadInt( entityDefHandle );

	savefile->ReadString ( classname );		// cnicholson: Added unrestored var

	// restore must retrieve entityDefHandle from the renderer
// 	if ( entityDefHandle != -1 ) {
// 		entityDefHandle = gameRenderWorld->AddEntityDef( &renderEntity );
// 	}
}

/*
================
rvClientModel::SetModel
================
*/
void rvClientModel::SetModel( const char* modelname ) {
// jmarshall 
	//FreeEntityDef();
// jmarshall end

	idRenderModel *model = renderModelManager->FindModel( modelname );

	if ( model ) {
		model->Reset();
	}

	renderEntity->SetRenderModel(model);
	renderEntity->SetCallback(NULL);
	renderEntity->SetNumJoints(0);
	renderEntity->SetJoints(NULL);

	if (model) {
		renderEntity->SetBounds(model->Bounds( renderEntity ));
	} else {
		idBounds bounds;
		bounds.Zero();
		renderEntity->SetBounds(bounds);
	}
}

/*
==============
rvClientModel::ProjectOverlay
==============
*/
void rvClientModel::ProjectOverlay( const idVec3 &origin, const idVec3 &dir, float size, const char *material ) {
	float s, c;
	idMat3 axis, axistemp;
	idVec3 localOrigin, localAxis[2];
	idPlane localPlane[2];

	// make sure the entity has a valid model handle
	if ( renderEntity != NULL ) {
		return;
	}

	// only do this on dynamic md5 models
	if ( renderEntity->GetRenderModel()->IsDynamicModel() != DM_CACHED ) {
		return;
	}

	idMath::SinCos16( gameLocal.random.RandomFloat() * idMath::TWO_PI, s, c );

	axis[2] = -dir;
	axis[2].NormalVectors( axistemp[0], axistemp[1] );
	axis[0] = axistemp[ 0 ] * c + axistemp[ 1 ] * -s;
	axis[1] = axistemp[ 0 ] * -s + axistemp[ 1 ] * -c;

	renderEntity->GetAxis().ProjectVector( origin - renderEntity->GetOrigin(), localOrigin );
	renderEntity->GetAxis().ProjectVector( axis[0], localAxis[0] );
	renderEntity->GetAxis().ProjectVector( axis[1], localAxis[1] );

	size = 1.0f / size;
	localAxis[0] *= size;
	localAxis[1] *= size;

	localPlane[0] = localAxis[0];
	localPlane[0][3] = -( localOrigin * localAxis[0] ) + 0.5f;

	localPlane[1] = localAxis[1];
	localPlane[1][3] = -( localOrigin * localAxis[1] ) + 0.5f;

	const idMaterial *mtr = declManager->FindMaterial( material );

	// project an overlay onto the model
	gameRenderWorld->ProjectOverlay(renderEntity->GetIndex(), localPlane, mtr );

	// make sure non-animating models update their overlay
	UpdateVisuals();
}

/*
================
rvClientModel::UpdateRenderEntity
================
*/
bool rvClientModel::UpdateRenderEntity( idRenderEntity *renderEntity, const renderView_t *renderView ) {
	if ( gameLocal.inCinematic && gameLocal.skipCinematic ) {
		return false;
	}

	idAnimator *animator = GetAnimator();
	if ( animator ) {
		return animator->CreateFrame( gameLocal.time, false );
	}

	return false;
}

/*
================
rvClientModel::ModelCallback

NOTE: may not change the game state whatsoever!
================
*/
bool rvClientModel::ModelCallback(idRenderEntity*renderEntity, const renderView_t *renderView ) {
	rvClientEntity *cent;

	cent = gameLocal.clientEntities[ renderEntity->GetEntityNum() ];
	if ( !cent ) {
		gameLocal.Error( "rvClientModel::ModelCallback: callback with NULL client entity '%d'", renderEntity->GetEntityNum() );
		return false;
	}

	if( !cent->IsType( rvClientModel::Type ) ) {
		gameLocal.Error( "rvClientModel::ModelCallback: callback with non-client model on client entity '%d'", renderEntity->GetEntityNum() );
		return false;
	}

	return ((rvClientModel*)cent)->UpdateRenderEntity( renderEntity, renderView );
}

/*
================
rvClientModel::GetPhysicsToVisualTransform
================
*/
bool rvClientModel::GetPhysicsToVisualTransform( idVec3 &origin, idMat3 &axis ) {
	return false;
}

/*
================
rvClientModel::UpdateModelTransform
================
*/
void rvClientModel::UpdateModelTransform( void ) {
	idVec3 origin;
	idMat3 axis;

	if ( GetPhysicsToVisualTransform( origin, axis ) ) {
		renderEntity->SetAxis(axis * worldAxis);
		renderEntity->SetOrigin(worldOrigin + origin * renderEntity->GetAxis());
	} else {
		renderEntity->SetAxis(worldAxis);
		renderEntity->SetOrigin(worldOrigin);
	}
}

/*
================
rvClientModel::UpdateModel
================
*/
void rvClientModel::UpdateModel( void ) {
	UpdateModelTransform();

	idAnimator *animator = GetAnimator();
	if ( animator && animator->ModelHandle() ) {
		// set the callback to update the joints
		renderEntity->SetCallback(rvClientModel::ModelCallback);
	}
}

/*
================
rvClientModel::UpdateVisuals
================
*/
void rvClientModel::UpdateVisuals( void ) {
	UpdateModel();
	UpdateSound();
}

/*
================
rvClientModel::SetSkin
================
*/
void rvClientModel::SetSkin( const idDeclSkin *skin ) {
	renderEntity->SetCustomSkin(skin);
	UpdateVisuals();
}

/*
===============================================================================

rvAnimatedClientEntity

===============================================================================
*/

CLASS_DECLARATION( rvClientModel, rvAnimatedClientEntity )
END_CLASS

/*
================
rvAnimatedClientEntity::rvAnimatedClientEntity
================
*/
rvAnimatedClientEntity::rvAnimatedClientEntity ( void ) {
}

/*
================
rvAnimatedClientEntity::~rvAnimatedClientEntity
================
*/
rvAnimatedClientEntity::~rvAnimatedClientEntity ( void ) {
}

/*
================
rvAnimatedClientEntity::Spawn
================
*/
void rvAnimatedClientEntity::Spawn( void ) {
	SetModel( spawnArgs.GetString( "model" ) );
}
/*
================
rvAnimatedClientEntity::Think
================
*/
void rvAnimatedClientEntity::Think ( void ) {
	UpdateAnimation();

	rvClientEntity::Think();
}

/*
================
rvAnimatedClientEntity::UpdateAnimation
================
*/
void rvAnimatedClientEntity::UpdateAnimation( void ) {
	// is the model an MD5?
	if ( !animator.ModelHandle() ) {
		// no, so nothing to do
		return;
	}

	// call any frame commands that have happened in the past frame
	animator.ServiceAnims( gameLocal.previousTime, gameLocal.time );

	// if the model is animating then we have to update it
	if ( !animator.FrameHasChanged( gameLocal.time ) ) {
		// still fine the way it was
		return;
	}

	// get the latest frame bounds
	idBounds bounds;
	animator.GetBounds( gameLocal.time, bounds );
	if ( bounds.IsCleared() ) {
		gameLocal.DPrintf( "rvAnimatedClientEntity %s %d: inside out bounds - %d\n", GetClassname(), entityNumber, gameLocal.time );
	}
	renderEntity->SetBounds(bounds);

	// update the renderEntity
	UpdateVisuals();
	Present();

	// the animation is updated
	animator.ClearForceUpdate();
}

/*
================
rvAnimatedClientEntity::SetModel
================
*/
void rvAnimatedClientEntity::SetModel( const char *modelname ) {
	FreeEntityDef();

	idRenderModel *model = animator.SetModel( modelname );
	if ( !model ) {
		rvClientModel::SetModel( modelname );
		return;
	}
	renderEntity->SetRenderModel(model);

	if ( !renderEntity->GetCustomSkin() ) {
		renderEntity->SetCustomSkin(animator.ModelDef()->GetDefaultSkin());
	}

	// set the callback to update the joints
	renderEntity->SetCallback(rvClientModel::ModelCallback);

	idJointMat* joints;
	idBounds bounds;
	int numJoints;
	animator.GetJoints( &numJoints, &joints );
	animator.GetBounds( gameLocal.time, bounds );

	renderEntity->SetNumJoints(numJoints);
	renderEntity->SetJoints(joints);
	renderEntity->SetBounds(bounds);

	//UpdateVisuals();
	Present();
}
