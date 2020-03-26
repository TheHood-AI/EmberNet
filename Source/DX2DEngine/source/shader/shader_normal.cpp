#include "stdafx.h"

#include "shaders/shader_normal.h"
#include "engine.h"
#include "d3d/direct_3d.h"
#include "render/render_object.h"
#include "sprite/textured_quad.h"
#include "texture/texture.h"
#include "light/light_manager.h"
#include "texture/texture_manager.h"

using namespace Tga2D;

CShaderNormal::CShaderNormal(CEngine* aEngine)
	:CShader(aEngine)
{
}


CShaderNormal::~CShaderNormal()
{
}

bool Tga2D::CShaderNormal::Init()
{
	return CShader::CreateShaders("shaders/sprite_shader_vs.fx", "shaders/sprite_shader_ps.fx", nullptr);
}

bool  Tga2D::CShaderNormal::Init(std::string aPixelShaderFile, std::string aVertexShaderFile, std::string aPixelMainFunction, std::string aVertexMainFunction)
{
	return CShader::CreateShaders(aVertexShaderFile.c_str(), aPixelShaderFile.c_str(), nullptr, aPixelMainFunction, aVertexMainFunction);
}

void Tga2D::CShaderNormal::Render(CRenderObjectSprite* aObject, ID3D11Buffer* aObjectBuffer, ID3D11Buffer* aVertexBuffer)
{
	if (!myIsReadyToRender)
	{
		return;
	}
	CShader::Render(aObject);
	SetShaderParameters(aObject, aObjectBuffer, aVertexBuffer);

	myDirect3dEngine->GetContext()->Draw(6, 0);

}

bool Tga2D::CShaderNormal::CreateInputLayout(ID3D10Blob* aVS)
{
	// LAYOUT
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;


	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	HRESULT result = myDirect3dEngine->GetDevice()->CreateInputLayout(polygonLayout, numElements, aVS->GetBufferPointer(), aVS->GetBufferSize(), &myLayout);
	if (FAILED(result))
	{
		ERROR_PRINT("%s", "Layout error");
	}
	return true;
}

void CShaderNormal::SetShaderParameters(CRenderObjectSprite* aObject, ID3D11Buffer* aObjectBuffer, ID3D11Buffer* aVertexBuffer)
{
	if (!aObjectBuffer || !aObject)
	{
		return;
	}

	D3D11_MAPPED_SUBRESOURCE mappedObjectResource;

	HRESULT result = myDirect3dEngine->GetContext()->Map(aObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedObjectResource);
	if (FAILED(result))
	{
		INFO_PRINT("Error in rendering!");
		return;
	}


	VECTOR2F correctedPosition = VECTOR2F((aObject->myPosition.x * 2.0f) - 1.0f, -(aObject->myPosition.y * 2.0f) + 1.0f);

	//correctedPosition.x -= ratioX;

	ObjectBufferType* objectDataPtr;
	objectDataPtr = (ObjectBufferType*)mappedObjectResource.pData;
	
	objectDataPtr->myPosition = correctedPosition;
	objectDataPtr->myColor = aObject->myColor.AsVec4();
	objectDataPtr->myPivot = Vector4f(aObject->myPivot.x, aObject->myPivot.y, 0, 0);
	objectDataPtr->myUV = Vector4f(aObject->myUV.x, aObject->myUV.y, aObject->myUVScale.x, aObject->myUVScale.y);

	

	
	//bool haveLights = CEngine::GetInstance()->GetLightManager().GetNumberOfLights() > 0;
	//float lightType = haveLights ? 1.0f : 0.0f;
	if (CTexturedQuad* texQuad = static_cast<CTexturedQuad*>(aObject))
	{
		UpdateVertexes(texQuad, aVertexBuffer);

		ID3D11ShaderResourceView* textures[1 + EShaderMap::MAP_MAX];
		textures[0] = texQuad->myTexture->myResource;
		for (unsigned short index = 0; index < EShaderMap::MAP_MAX; index++)
		{
			textures[1 + index] = CEngine::GetInstance()->GetTextureManager().GetDefaultNormalMapResource();
			if (texQuad->myMaps[index])
			{
				textures[1 + index] = texQuad->myMaps[index]->myResource;
			}
		}
		myDirect3dEngine->GetContext()->PSSetShaderResources(1, 1 + EShaderMap::MAP_MAX, textures);



	}

	// Multiply by two to take it from 0-1 space to -1 to 1
	objectDataPtr->myRotationAndSize = Vector4f(aObject->myRotation, (aObject->mySize.x * 2) * aObject->mySizeMultiplier.x, (aObject->mySize.y * 2) * aObject->mySizeMultiplier.y, 0.0f/*lightType*/);

	myDirect3dEngine->GetContext()->Unmap(aObjectBuffer, 0);

	const unsigned int strides = sizeof(SVertex);
	const unsigned int offsets = 0;
	myDirect3dEngine->GetContext()->IASetVertexBuffers(0, 1, &aVertexBuffer, &strides, &offsets);
	myDirect3dEngine->GetContext()->VSSetConstantBuffers(EConstantBufferSlot_Object, 1, &aObjectBuffer);
}

void CShaderNormal::UpdateVertexes(CTexturedQuad* aQuad, ID3D11Buffer* aVertexBuffer)
{
	D3D11_MAPPED_SUBRESOURCE mappedVertexResource;
	SVertex* dataVertexPtr;
	HRESULT result = myDirect3dEngine->GetContext()->Map(aVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexResource);
	if (FAILED(result))
	{
		INFO_PRINT("Error in rendering!");
		return;
	}
	dataVertexPtr = (SVertex*)mappedVertexResource.pData;

	const STextureRext& textureRect = aQuad->GetTextureRect();

	const float startSize = 1.0f;

	float theZ = 0.0f;
	dataVertexPtr[0].X = 0;
	dataVertexPtr[0].Y = -startSize;
	dataVertexPtr[0].Z = theZ;
	dataVertexPtr[0].texX = textureRect.myStartX;
	dataVertexPtr[0].texY = textureRect.myEndY;

	dataVertexPtr[1].X = 0;
	dataVertexPtr[1].Y = 0;
	dataVertexPtr[1].Z = theZ;
	dataVertexPtr[1].texX = textureRect.myStartX;
	dataVertexPtr[1].texY = textureRect.myStartY;

	dataVertexPtr[2].X = startSize;
	dataVertexPtr[2].Y = -startSize;
	dataVertexPtr[2].Z = theZ;
	dataVertexPtr[2].texX = textureRect.myEndX;
	dataVertexPtr[2].texY = textureRect.myEndY;

	dataVertexPtr[3].X = startSize;
	dataVertexPtr[3].Y = 0;
	dataVertexPtr[3].Z = theZ;
	dataVertexPtr[3].texX = textureRect.myEndX;
	dataVertexPtr[3].texY = textureRect.myStartY;

	dataVertexPtr[4].X = startSize;
	dataVertexPtr[4].Y = -startSize;
	dataVertexPtr[4].Z = theZ;
	dataVertexPtr[4].texX = textureRect.myEndX;
	dataVertexPtr[4].texY = textureRect.myEndY;

	dataVertexPtr[5].X = 0;
	dataVertexPtr[5].Y = 0;
	dataVertexPtr[5].Z = theZ;
	dataVertexPtr[5].texX = textureRect.myStartX;
	dataVertexPtr[5].texY = textureRect.myStartY;

	myDirect3dEngine->GetContext()->Unmap(aVertexBuffer, 0);
}