// draw_occlusion.cpp
//

#include "engine_precompiled.h"
#include "tr_local.h"

enum rvnmOcclusionQueryState
{
	OCCLUSION_QUERY_STATE_HIDDEN,
	OCCLUSION_QUERY_STATE_VISIBLE,
	OCCLUSION_QUERY_STATE_WAITING
};

class rvmOcclusionQuery {
public:
	rvmOcclusionQuery()
	{
		glGenQueries(1, &id);
		queryState = OCCLUSION_QUERY_STATE_HIDDEN;
		queryStartTime = -1;
	}

	~rvmOcclusionQuery() {
		queryStartTime = -1;
		glDeleteQueries(1, &id);
	}

	void RunQuery(viewLight_s *light)
	{
		this->light = light;

		if (queryState != OCCLUSION_QUERY_STATE_WAITING)
		{
			queryState = OCCLUSION_QUERY_STATE_WAITING;

			queryStartTime = Sys_Milliseconds();
			queryTimeOutTime = queryStartTime + SEC2MS(r_occlusionQueryTimeOut.GetInteger());

			glBeginQuery(GL_ANY_SAMPLES_PASSED, id);
			RB_DrawElementsImmediate(light->frustumTris);
			glEndQuery(GL_ANY_SAMPLES_PASSED);
		}
	}

	viewLight_s* GetViewLight() { return light; }

	bool IsVisible() {
		GLuint passed = INT_MAX;
		
		passed = 0;
		glGetQueryObjectuiv(id, GL_QUERY_RESULT, &passed);
		if (passed) {
			return true;
		}

		return false;
	}

	int GetQueryStartTime(void) {
		return queryStartTime;
	}
	
	bool IsQueryStale(void) {
		int currentTime = Sys_Milliseconds();
		return queryTimeOutTime < currentTime;
	}

private:
	int queryStartTime;
	int queryTimeOutTime;
	GLuint id;
	viewLight_s* light;
	rvnmOcclusionQueryState queryState;
};


void RB_Draw_LightOcclusion(void) {
	viewLight_t		*vLight;
	idList<rvmOcclusionQuery*> occlusionQueries;

	if (backEnd.viewDef->viewLights == NULL)
		return;

	renderProgManager.BindShader_Depth();

	RB_SetMVP(backEnd.viewDef->worldSpace.mvp);

	renderProgManager.CommitUniforms();

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	for (vLight = backEnd.viewDef->viewLights; vLight; vLight = vLight->next) {
		if (!vLight->localInteractions && !vLight->globalInteractions
			&& !vLight->translucentInteractions) {
			continue;
		}

		if (!backEnd.viewDef->isEditor) {
			if(vLight->lightDef->currentOcclusionQuery) {
				// Only reap the occlusion query if it isn't stale.
				if(!vLight->lightDef->currentOcclusionQuery->IsQueryStale())
				{
					if (vLight->lightDef->currentOcclusionQuery->IsVisible()) {
						vLight->visibleFrame = tr.frameCount;

						backEnd.perfMetrics.gpuCulledLights++;

						if (!vLight->lightDef->parms.noShadows)
							backEnd.perfMetrics.shadowedLightCount++;
					}
				}

				delete vLight->lightDef->currentOcclusionQuery;
				vLight->lightDef->currentOcclusionQuery = NULL;
			}
		}

		backEnd.perfMetrics.frustumCulledLights++;

		rvmOcclusionQuery*query = new rvmOcclusionQuery();
		query->RunQuery(vLight);
		if (!backEnd.viewDef->isEditor) {
			vLight->lightDef->currentOcclusionQuery = query;
		}
		else {
			occlusionQueries.Append(query);
		}
	}

	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// Test the occlusion queries(if in editor, reap immediately). 
	if (backEnd.viewDef->isEditor)
	{
		for (int i = 0; i < occlusionQueries.Num(); i++) {
			if (occlusionQueries[i]->IsVisible()) {
				occlusionQueries[i]->GetViewLight()->visibleFrame = tr.frameCount;

				backEnd.perfMetrics.gpuCulledLights++;

				if (!occlusionQueries[i]->GetViewLight()->lightDef->parms.noShadows)
					backEnd.perfMetrics.shadowedLightCount++;
			}

			delete occlusionQueries[i];
		}
	}
	occlusionQueries.Clear();


	renderProgManager.Unbind();

	
}