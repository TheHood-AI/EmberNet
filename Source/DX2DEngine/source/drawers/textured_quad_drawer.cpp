#include "stdafx.h"

#include "drawers/textured_quad_drawer.h"
#include "d3d/direct_3d.h"
#include "sprite/textured_quad.h"
#include "texture/texture_manager.h"
#include "sprite/sprite.h"
#include "texture/texture.h"
#include "shaders/shader_normal.h"
#include "shaders/customshader.h"
#include "engine.h"


using namespace Tga2D;

CTexturedQuadDrawer::CTexturedQuadDrawer(CDirectEngine* anEngine)
	:myEngine(anEngine)
{
}


CTexturedQuadDrawer::~CTexturedQuadDrawer(void)
{
	SAFE_RELEASE(myObjectBuffer);
	SAFE_RELEASE(myLayout);
	SAFE_RELEASE(myVertexBuffer);
	delete myShaders[EShader_Normal];
	delete myShaders[EShader_FullScreen];
}

void CTexturedQuadDrawer::Init()
{
	D3D11_BUFFER_DESC objectBufferDesc;
	objectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objectBufferDesc.ByteWidth = sizeof(ObjectBufferType);
	objectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	objectBufferDesc.MiscFlags = 0;
	objectBufferDesc.StructureByteStride = 0;

	HRESULT result;
	result = myEngine->GetDevice()->CreateBuffer(&objectBufferDesc, NULL, &myObjectBuffer);
	if(FAILED(result))
	{
		ERROR_PRINT("%s", "Object Buffer error");
		return;
	}
	InitShaders();
	CreateBuffer();
}

void CTexturedQuadDrawer::CreateBuffer()
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;       
	vertexBufferDesc.ByteWidth = sizeof(SVertex) * 6;        
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


void Tga2D::CTexturedQuadDrawer::Draw(CRenderObjectSprite* aObject)
{
	CTexturedQuad* quad = static_cast<CTexturedQuad*>(aObject);
	if (quad->myCustomShader)
	{
		quad->myCustomShader->Render(aObject, myObjectBuffer, myVertexBuffer);
		return;
	}

	if (quad)
	{
		myShaders[quad->GetShaderType()]->Render(aObject, myObjectBuffer, myVertexBuffer);
	}

}


bool Tga2D::CTexturedQuadDrawer::InitShaders()
{
	myShaders[EShader_Normal] = new CShaderNormal(CEngine::GetInstance());
	myShaders[EShader_Normal]->Init();

	myShaders[EShader_FullScreen] = new CShaderNormal(CEngine::GetInstance());
	myShaders[EShader_FullScreen]->Init("shaders/sprite_shader_ps.fx", "shaders/common_sprite_fullscreen.fx", "PShader", "VShaderFull");

	return true;
}

