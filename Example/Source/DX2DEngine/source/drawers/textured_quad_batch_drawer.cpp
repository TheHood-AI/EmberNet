#include "stdafx.h"

#include "drawers/textured_quad_batch_drawer.h"
#include "sprite/textured_quad_batch.h"
#include "d3d/direct_3d.h"
#include "sprite/sprite_batch.h"
#include "sprite/sprite.h"
#include "texture/texture_manager.h"
#include "shaders/shader_normal_instanced.h"
#include "shaders/customshader.h"
#include "engine.h"

using namespace Tga2D;

CTexturedQuadBatchDrawer::CTexturedQuadBatchDrawer(CDirectEngine* anEngine)
: myEngine(anEngine)
, myIsLoaded(false)
{
}


CTexturedQuadBatchDrawer::~CTexturedQuadBatchDrawer()
{
	SAFE_RELEASE(myInstanceBuffer);
	SAFE_RELEASE(myVertexBuffer);

	delete myNormalShaderInstanced;
}

void Tga2D::CTexturedQuadBatchDrawer::Init()
{
	D3D11_BUFFER_DESC objectBufferDesc;
	objectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objectBufferDesc.ByteWidth = sizeof(ObjectBufferInstanceType) * 1025;
	objectBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	objectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	objectBufferDesc.MiscFlags = 0;
	objectBufferDesc.StructureByteStride = 0;

	HRESULT result;
	result = myEngine->GetDevice()->CreateBuffer(&objectBufferDesc, nullptr, &myInstanceBuffer);
	if(FAILED(result))
	{
		ERROR_PRINT("%s", "Object Buffer error");
		return;
	}
	myIsLoaded = InitShaders();
	CreateBuffer();
}

bool Tga2D::CTexturedQuadBatchDrawer::InitShaders()
{
	myNormalShaderInstanced = new CShaderNormalInstanced(CEngine::GetInstance());
	if (!myNormalShaderInstanced->Init())
	{
		return false;
	}

	return true;
}



void CTexturedQuadBatchDrawer::CreateBuffer()
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	float startSize = 1.0f;
	float theZ = 0.0f;
	myVerticies[0].X = -0;
	myVerticies[0].Y = -startSize;
	myVerticies[0].Z = theZ;
	myVerticies[0].texX = 0.0f;
	myVerticies[0].texY = 1.0f;
	myVerticies[0].myVertexIndex = 0;

	myVerticies[1].X = -0;
	myVerticies[1].Y = -0;
	myVerticies[1].Z = theZ;
	myVerticies[1].texX = 0.0f;
	myVerticies[1].texY = 0.0f;
	myVerticies[1].myVertexIndex = 1;

	myVerticies[2].X = startSize;
	myVerticies[2].Y = -startSize;
	myVerticies[2].Z = theZ;
	myVerticies[2].texX = 1.0f;
	myVerticies[2].texY = 1.0f;
	myVerticies[2].myVertexIndex = 2;

	myVerticies[3].X =  startSize;
	myVerticies[3].Y = -0;
	myVerticies[3].Z = theZ;
	myVerticies[3].texX = 1.0f;
	myVerticies[3].texY = 0.0f;
	myVerticies[3].myVertexIndex = 3;

	myVerticies[4].X = startSize;
	myVerticies[4].Y = -startSize;
	myVerticies[4].Z = theZ;
	myVerticies[4].texX = 1.0f;
	myVerticies[4].texY = 1.0f;
	myVerticies[4].myVertexIndex = 4;

	myVerticies[5].X = -0;
	myVerticies[5].Y = -0;
	myVerticies[5].Z = theZ;
	myVerticies[5].texX = 0.0f;
	myVerticies[5].texY = 0.0f;
	myVerticies[5].myVertexIndex = 5;

	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;       
	vertexBufferDesc.ByteWidth = sizeof(SVertexInstanced) * 6;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = myVerticies;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	HRESULT hr = myEngine->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
	if (FAILED(hr))
	{
		ERROR_PRINT("%s", "Buffer error");
		return;
	}
}


void Tga2D::CTexturedQuadBatchDrawer::Draw(CTexturedQuadBatch* aObject)
{
	if (!myIsLoaded)
	{
		return;
	}

	if (aObject->myCustomShader)
	{
		aObject->myCustomShader->Render(aObject, myInstanceBuffer, myVertexBuffer);
		return;
	}

	myNormalShaderInstanced->Render(aObject, myInstanceBuffer, myVertexBuffer);

}

