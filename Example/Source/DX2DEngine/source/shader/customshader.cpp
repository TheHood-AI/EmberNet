#include "stdafx.h"
#include "shaders/shader_normal.h"
#include "shaders/shader_normal_instanced.h"
#include "shaders/customshader.h"
#include "shaders/shader_common.h"
#include "texture/texture.h"
#include "sprite/textured_quad_batch.h"
#include "sprite/textured_quad.h"

#include <tga2d/render/render_object.h>
#include <d3dcommon.h>
#include <d3d11.h>

Tga2D::CCustomShader::CCustomShader()
{
	myShader = nullptr;
	myCurrentDataIndex = -1;
	myIsPostInited = false;
	myBufferIndex = EShaderDataBufferIndex_1;
	myCurrentTextureIndex = 0;
	myCustomBuffer = nullptr;
}

Tga2D::CCustomShader::~CCustomShader()
{
	SAFE_DELETE(myShader);
}

bool Tga2D::CCustomShader::Init(const char* aVertex, const char* aPixel)
{
	if (!PostInitInternal(aVertex, aPixel))
	{
		return false;
	}

	D3D11_BUFFER_DESC commonBufferDesc;
	commonBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	commonBufferDesc.ByteWidth = sizeof(Tga2D::Vector4f) * 64;
	commonBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	commonBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	commonBufferDesc.MiscFlags = 0;
	commonBufferDesc.StructureByteStride = 0;

	HRESULT result = CEngine::GetInstance()->GetDirect3D().GetDevice()->CreateBuffer(&commonBufferDesc, NULL, &myCustomBuffer);
	if (result != S_OK)
	{
		ERROR_PRINT("DX2D::CCustomShader::PostInit - Size missmatch between CPU and GPU(shader)");
		return false;
	}

	myIsPostInited = true;
	return true;
}

void Tga2D::CCustomShader::SetDataBufferIndex(EShaderDataBufferIndex aBufferRegisterIndex)
{
	myBufferIndex = (unsigned char)aBufferRegisterIndex;
}

bool Tga2D::CCustomShader::PostInitInternal(const char* aVertex, const char* aPixel)
{
	myShader = new CShaderNormal(CEngine::GetInstance());
	if (!myShader->CreateShaders(aVertex, aPixel))
	{
		delete myShader;
		myShader = nullptr;
		return false;
	}
	return true;
}

bool Tga2D::CCustomShader::Render(CRenderObject* aObject, ID3D11Buffer* aObjectBuffer, ID3D11Buffer* aVertexBuffer)
{
	if (!myIsPostInited)
	{
		ERROR_PRINT("DX2D::CCustomShader::Render - Custom shader is not inited! Run PostInit() on this shader!");
		return false;
	}

	ID3D11DeviceContext* context = CEngine::GetInstance()->GetDirect3D().GetContext();

	if (myCustomBuffer)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResourceCommon;
		Tga2D::Vector4f* dataPtrCommon;
		HRESULT result = context->Map(myCustomBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResourceCommon);
		if (FAILED(result))
		{
			INFO_PRINT("Error in rendering!");
			return false;
		}

		dataPtrCommon = (Tga2D::Vector4f*)mappedResourceCommon.pData;
		for (int i = 0; i < myCurrentDataIndex + 1; i++)
		{
			dataPtrCommon[i].x = myCustomData[i].x;
			dataPtrCommon[i].y = myCustomData[i].y;
			dataPtrCommon[i].z = myCustomData[i].z;
			dataPtrCommon[i].w = myCustomData[i].w;
		}

		context->Unmap(myCustomBuffer, 0);
		context->VSSetConstantBuffers(myBufferIndex, 1, &myCustomBuffer);
		context->PSSetConstantBuffers(myBufferIndex, 1, &myCustomBuffer);
	}
	

	for (int i = 0; i < myCurrentTextureIndex; i++)
	{
		ID3D11ShaderResourceView* textures[1];
		textures[0] = myBoundTextures[i].myTexture->myResource;

		context->PSSetShaderResources(myBoundTextures[i].myIndex, 1, textures);
	}

	DoRender(aObject, aObjectBuffer, aVertexBuffer);



	
	return true;
}

void Tga2D::CCustomShader::DoRender(CRenderObject* aObject, ID3D11Buffer* aObjectBuffer, ID3D11Buffer* aVertexBuffer)
{
	myShader->Render(static_cast<CRenderObjectSprite*>(aObject), aObjectBuffer, aVertexBuffer);
}

void Tga2D::CCustomShader::SetShaderdataFloat4(Tga2D::Vector4f someData, EShaderDataID aID)
{
	if (aID > MAX_SHADER_DATA)
	{
		ERROR_PRINT("DX2D::CCustomShader::SetShaderdataFloat4() The id is bigger than allowed size");
		return;
	}
	myCustomData[aID] = someData;
	if (aID > myCurrentDataIndex)
	{
		myCurrentDataIndex = aID;
	}
}

void Tga2D::CCustomShader::SetTextureAtRegister(Tga2D::CTexture* aTexture, EShaderTextureSlot aRegisterIndex)
{
	myBoundTextures[aRegisterIndex-4] = SBoundTexture(aTexture, (unsigned char)aRegisterIndex);

	if (myCurrentTextureIndex <  (aRegisterIndex - 4) + 1)
	{
		myCurrentTextureIndex = static_cast<unsigned char>((aRegisterIndex - 4) + 1);
	}
}

void Tga2D::CCustomShader::SetTextureAtRegister(Tga2D::CSprite* aSpriteWithTexture, EShaderTextureSlot aRegisterIndex)
{
	SetTextureAtRegister(aSpriteWithTexture->GetTexturedQuad()->myTexture, aRegisterIndex);
}

//Batched
Tga2D::CCustomShaderBatched::CCustomShaderBatched()
{
	myBatchShader = nullptr;
}

Tga2D::CCustomShaderBatched::~CCustomShaderBatched()
{
	SAFE_DELETE(myBatchShader);
}

void Tga2D::CCustomShaderBatched::DoRender(CRenderObject* aObject, ID3D11Buffer* aObjectBuffer, ID3D11Buffer* aVertexBuffer)
{
	myBatchShader->Render(static_cast<CTexturedQuadBatch*>(aObject), aObjectBuffer, aVertexBuffer);
}

bool Tga2D::CCustomShaderBatched::PostInitInternal(const char* aVertex, const char* aPixel)
{
	myBatchShader = new CShaderNormalInstanced(CEngine::GetInstance());
	if (!myBatchShader->CreateShaders(aVertex, aPixel))
	{
		delete myBatchShader;
		myBatchShader = nullptr;
		return false;
	}
	return true;
}

