#include "stdafx.h"
#include <tga2d/animation/SpineAnimation_internal.h>
#include <tga2d/primitives/custom_shape.h>
#include <tga2d/texture/texture.h>
#include <tga2d/engine.h>
#include <tga2d/drawers/debug_drawer.h>
#include <engine_defines.h>

typedef unsigned char Uint8;

_SP_ARRAY_IMPLEMENT_TYPE(spColorArray, spColor)

using namespace Tga2D;
void _AtlasPage_createTexture(AtlasPage* self, const char* path, void* aData)
{
	CSpineAnimation_Internal* anim = static_cast<CSpineAnimation_Internal*>(aData);

	Tga2D::CCustomShape* shape = new Tga2D::CCustomShape();
	
	shape->SetTexture(path);
	
	shape->SetSamplerState(ESamplerFilter_Bilinear, ESamplerAddressMode_Clamp);
	if (self->uWrap == SP_ATLAS_REPEAT && self->vWrap == SP_ATLAS_REPEAT)  shape->SetSamplerState(ESamplerFilter_Bilinear, ESamplerAddressMode_Wrap);

	self->rendererObject = shape;
	VECTOR2UI size = shape->GetTexture()->myImageSize;
	self->width = size.x;
	self->height = size.y;
	anim->myShapes.push_back(shape);
}

void _AtlasPage_disposeTexture(AtlasPage* self) 
{
	delete self->rendererObject; // myShape
	self->rendererObject = nullptr;
}

char* _Util_readFile(const char* path, int* length) 
{
	return _spReadFile(path, length);
}


SkeletonData* readSkeletonJsonData(const char* filename, Atlas* atlas, float scale)
{
	SkeletonJson* json = SkeletonJson_create(atlas);
	json->scale = scale;
	SkeletonData* skeletonData = SkeletonJson_readSkeletonDataFile(json, filename);
	if (!skeletonData)
	{
		ERROR_PRINT("%s%s", "Animation error: " , json->error)
	}
	SkeletonJson_dispose(json);
	return skeletonData;
}


CSpineAnimation_Internal::CSpineAnimation_Internal(const char* aAtlasPath, const char* aBonePath) :
	timeScale(1),
	vertexEffect(0),
	myWorldVertices(0), 
	myClipper(0)
{
	
	//myShape = new Tga2D::CCustomShape();
	myAtlas = Atlas_createFromFile(aAtlasPath, 0, this);
	mySkeletonData = readSkeletonJsonData(aBonePath, myAtlas, 1.0f);
	if (!mySkeletonData)
	{
		ERROR_PRINT("%s", "Animation error, bone error, all connected?")
	}
	else
	{
		myStateData = AnimationStateData_create(mySkeletonData);
		Bone_setYDown(true);
		myWorldVertices = MALLOC(float, SPINE_MESH_VERTEX_COUNT_MAX);

		mySkeleton = Skeleton_create(mySkeletonData);

		tempUvs = spFloatArray_create(16);
		tempColors = spColorArray_create(16);

		ownsAnimationStateData = myStateData == 0;
		if (ownsAnimationStateData) myStateData = AnimationStateData_create(mySkeletonData);

		myAnimationState = AnimationState_create(myStateData);
		myClipper = spSkeletonClipping_create();

		myNumBones = mySkeletonData->bonesCount;
	}

	
}

CSpineAnimation_Internal::~CSpineAnimation_Internal() 
{
	FREE(myWorldVertices);
	if (ownsAnimationStateData) AnimationStateData_dispose(myAnimationState->data);
	if (myAnimationState)
	{
		AnimationState_dispose(myAnimationState);
	}
	if (mySkeleton)
	{
		Skeleton_dispose(mySkeleton);
		if (myClipper)
		{
			spSkeletonClipping_dispose(myClipper);
		}
		
		spFloatArray_dispose(tempUvs);
		spColorArray_dispose(tempColors);
	}
	
	
	if (myAtlas)
	{
		Atlas_dispose(myAtlas);
		myAtlas = nullptr;
	}

	if (myStateData)
	{
		AnimationStateData_dispose(myStateData);
	}

	spSkeletonData_dispose(mySkeletonData);
	myShapes.clear(); // Automatically deleted in _AtlasPage_disposeTexture

}

void CSpineAnimation_Internal::Update(float deltaTime) 
{
	Skeleton_update(mySkeleton, deltaTime);
	AnimationState_update(myAnimationState, deltaTime * timeScale);
	AnimationState_apply(myAnimationState, mySkeleton);
	Skeleton_updateWorldTransform(mySkeleton);
}

struct SVertexForSpine
{
	Tga2D::CColor color;
	VECTOR2F position;
	VECTOR2F texCoords;
};


void CSpineAnimation_Internal::draw(const VECTOR2F& aPosition, const VECTOR2F& aScale/*RenderTarget& target, RenderStates states*/) const
{
	if (!myAnimationState || !myClipper)
	{
		return;
	}

	for (Tga2D::CCustomShape* shape : myShapes)
	{
		shape->Reset();
	}
	

	if (vertexEffect != 0) vertexEffect->begin(vertexEffect, mySkeleton);
	unsigned short quadIndices[6] = { 0, 1, 2, 2, 3, 0 };

	
	Tga2D::CCustomShape* customShape = 0;
	
	std::vector< Tga2D::CCustomShape*> queuedShapes;
	SVertexForSpine vertex;
	VECTOR2UI winTargetSize = Tga2D::CEngine::GetInstance()->GetTargetSize();
	for (int slotIndex = 0; slotIndex < mySkeleton->slotsCount; ++slotIndex)
	{
		Slot* slot = mySkeleton->drawOrder[slotIndex];
		Attachment* attachment = slot->attachment;
		if (!attachment) continue;

		float* vertices = myWorldVertices; 
		int verticesCount = 0;
		float* uvs = 0;
		unsigned short* indices = 0;
		int indicesCount = 0;
		spColor* attachmentColor;

		if (attachment->type == ATTACHMENT_REGION) 
		{
			RegionAttachment* regionAttachment = (RegionAttachment*)attachment;
			spRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, vertices, 0, 2);
			verticesCount = 4;
			uvs = regionAttachment->uvs;
			indices = quadIndices;
			indicesCount = 6;
			customShape = (Tga2D::CCustomShape*)((AtlasRegion*)regionAttachment->rendererObject)->page->rendererObject;
			attachmentColor = &regionAttachment->color;

		}
		else if (attachment->type == ATTACHMENT_MESH) 
		{
			MeshAttachment* mesh = (MeshAttachment*)attachment;
			if (mesh->super.worldVerticesLength > SPINE_MESH_VERTEX_COUNT_MAX) continue;
			customShape = (Tga2D::CCustomShape*)((AtlasRegion*)mesh->rendererObject)->page->rendererObject;
			spVertexAttachment_computeWorldVertices(SUPER(mesh), slot, 0, mesh->super.worldVerticesLength, myWorldVertices, 0, 2);
			verticesCount = mesh->super.worldVerticesLength >> 1;
			uvs = mesh->uvs;
			indices = mesh->triangles;
			indicesCount = mesh->trianglesCount;
			attachmentColor = &mesh->color;
		}
		else if (attachment->type == SP_ATTACHMENT_CLIPPING) 
		{
			spClippingAttachment* clip = (spClippingAttachment*)slot->attachment;
			spSkeletonClipping_clipStart(myClipper, slot, clip);
			continue;
		}
		else continue;

		Uint8 r = static_cast<Uint8>(mySkeleton->color.r * slot->color.r * attachmentColor->r * 255);
		Uint8 g = static_cast<Uint8>(mySkeleton->color.g * slot->color.g * attachmentColor->g * 255);
		Uint8 b = static_cast<Uint8>(mySkeleton->color.b * slot->color.b * attachmentColor->b * 255);
		Uint8 a = static_cast<Uint8>(mySkeleton->color.a * slot->color.a * attachmentColor->a * 255);
		vertex.color.myR = r / 255.0f;
		vertex.color.myG = g / 255.0f;
		vertex.color.myB = b / 255.0f;
		vertex.color.myA = a / 255.0f;

		spColor light;
		light.r = r / 255.0f;
		light.g = g / 255.0f;
		light.b = b / 255.0f;
		light.a = a / 255.0f;

		EBlendState blend = EBlendState_Disabled;
		if (!usePremultipliedAlpha) 
		{
			switch (slot->data->blendMode) {
			case BLEND_MODE_NORMAL:
				blend = EBlendState_Alphablend;
				break;
			case BLEND_MODE_ADDITIVE:
				blend = EBlendState_Additiveblend;
				break;
			default:
				blend = EBlendState_Alphablend;
			}
		}
		else 
		{
			switch (slot->data->blendMode) 
			{
			case BLEND_MODE_NORMAL:
				blend = EBlendState_Alphablend;
				break;
			case BLEND_MODE_ADDITIVE:
				blend = EBlendState_Additiveblend;
				break;
			default:
				blend = EBlendState_Alphablend;
			}
		}

		if (customShape)
		{
			customShape->Reset();
			customShape->SetBlendState(blend);
		}


		//customShape->SetPosition(aPosition);
		//customShape->SetSize(aScale);

		if (spSkeletonClipping_isClipping(myClipper))
		{
			spSkeletonClipping_clipTriangles(myClipper, vertices, verticesCount << 1, indices, indicesCount, uvs, 2);
			vertices = myClipper->clippedVertices->items;
			verticesCount = myClipper->clippedVertices->size >> 1;
			uvs = myClipper->clippedUVs->items;
			indices = myClipper->clippedTriangles->items;
			indicesCount = myClipper->clippedTriangles->size;
		}
		customShape->SetSize({ 1, 1 });
		VECTOR2UI size = VECTOR2UI(1, 1);
		if (customShape)
		{
			size = customShape->GetTexture()->myImageSize;
		}
		if (vertexEffect != 0) 
		{
			for (int i = 0; i < verticesCount; i++)
			{
				spColor vertexColor = light;
				spColor dark;
				dark.r = dark.g = dark.b = dark.a = 0;
				int index = i << 1;
				float x = vertices[index];
				float y = vertices[index + 1];
				float u = uvs[index];
				float v = uvs[index + 1];
				vertexEffect->transform(vertexEffect, &x, &y, &u, &v, &vertexColor, &dark);
				vertices[index] = x;
				vertices[index + 1] = y;
			}
			
			if (customShape)
			{
				for (int i = 0; i < indicesCount; ++i)
				{
					int index = indices[i] << 1;
					vertex.position.x = vertices[index] / (float)winTargetSize.x;// +aPosition.x;
					vertex.position.y = vertices[index + 1] / (float)winTargetSize.y;// +aPosition.y;
					vertex.texCoords.x = uvs[index] * size.x;
					vertex.texCoords.y = uvs[index + 1] * size.y;
					spColor vertexColor = tempColors->items[index >> 1];
					vertex.color.myR = static_cast<Uint8>(vertexColor.r) * myColor.myR;
					vertex.color.myG = static_cast<Uint8>(vertexColor.g) * myColor.myB;
					vertex.color.myB = static_cast<Uint8>(vertexColor.b) * myColor.myB;
					vertex.color.myA = static_cast<Uint8>(vertexColor.a) * myColor.myA;
					customShape->AddPoint(vertex.position, vertex.color, vertex.texCoords);
				}
			}

		}
		else if(customShape)
		{
			vertex.color.myR *= myColor.myR;
			vertex.color.myG *= myColor.myB;
			vertex.color.myB *= myColor.myB;
			vertex.color.myA *= myColor.myA;
			for (int i = 0; i < indicesCount; ++i) 
			{
				int index = indices[i] << 1;
				vertex.position.x = vertices[index] / (float)winTargetSize.x*aScale.x +aPosition.x;
				vertex.position.y = vertices[index + 1] / (float)winTargetSize.y*aScale.y +aPosition.y;
		
				vertex.texCoords.x = (uvs[index] );
				vertex.texCoords.y = (uvs[index + 1] );

				customShape->AddPoint(vertex.position, vertex.color, vertex.texCoords);
			}
		}

		spSkeletonClipping_clipEnd(myClipper, slot);

		customShape->BuildShape();
		customShape->Render();		
	}



	spSkeletonClipping_clipEnd2(myClipper);

	if (vertexEffect != 0) vertexEffect->end(vertexEffect);
}

void Tga2D::CSpineAnimation_Internal::DrawDebug()
{
	if (!myAnimationState)
	{
		return;
	}
	VECTOR2UI winSize = Tga2D::CEngine::GetInstance()->GetWindowSize();
	spBone** bones = mySkeleton->bones;
	for (int i=0; i< mySkeleton->bonesCount; i++)
	{
		spBone* bone = bones[i];


		VECTOR2F bonePos = VECTOR2F( (bone->worldX / (float)winSize.x) , bone->worldY / (float)winSize.y);
		Tga2D::CEngine::GetInstance()->GetDebugDrawer().DrawCircle(bonePos, 0.01f, CColor(1, 1, 1, 1));

		for (int j = 0; j< bone->childrenCount; j++)
		{
			spBone* child = bone->children[j];

			VECTOR2F bonePosChild = VECTOR2F((child->worldX / (float)winSize.x) , child->worldY / (float)winSize.y);
			Tga2D::CEngine::GetInstance()->GetDebugDrawer().DrawArrow(bonePos, bonePosChild, CColor(1, 1, 1, 1), 1.0f);

		}
	}


}

void Tga2D::CSpineAnimation_Internal::DoMixAnimation(std::string aFrom, std::string aTo, float aDelay)
{
	if (!myStateData)
	{
		return;
	}
	AnimationStateData_setMixByName((spAnimationStateData*)myStateData, aFrom.c_str(), aTo.c_str(), aDelay);
}
void Tga2D::CSpineAnimation_Internal::AddAnimationToQueue(int aTrackIndex, std::string aName, bool aLoop, float aDelay)
{
	if (!myAnimationState)
	{
		return;
	}
	if (!AnimationState_addAnimationByName(myAnimationState, aTrackIndex, aName.c_str(), aLoop, aDelay))
	{
		ERROR_PRINT("%s%s%s", "Animation: ", aName.c_str(), " not found");
	}
}


void Tga2D::CSpineAnimation_Internal::ClearQueue()
{
	if (!myAnimationState)
	{
		return;
	}
	spAnimationState_clearTracks(myAnimationState);
}

void Tga2D::CSpineAnimation_Internal::SetCurrentAnimation(int aTrackIndex, std::string aName, bool aLoop)
{
	if (!myAnimationState)
	{
		return;
	}
	if (aName.empty())
	{
		spAnimationState_addEmptyAnimation(myAnimationState, aTrackIndex, 0.1f, 0);
		return;
	}
	
	if (!AnimationState_setAnimationByName(myAnimationState, aTrackIndex, aName.c_str(), aLoop))
	{
		ERROR_PRINT("%s%s%s", "Animation: ", aName.c_str(), " not found");
	}
}

const char* Tga2D::CSpineAnimation_Internal::GetCurrentAnimation(int aTrackIndex)
{
	if (!myAnimationState)
	{
		return "";
	}
	spTrackEntry* state = AnimationState_getCurrent(myAnimationState, aTrackIndex);
	const char* animationName = (state && state->animation) ? state->animation->name : nullptr;
	return animationName;
}