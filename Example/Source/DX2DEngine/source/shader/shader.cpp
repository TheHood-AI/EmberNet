#include "stdafx.h"

#include "shaders/shader.h"
#include "engine.h"
#include "d3d/direct_3d.h"
#include "light/light.h"
#include "light/light_manager.h"
#include "shaders/shader_common.h"
#include "filewatcher/file_watcher.h"
#include "texture/texture_manager.h"
#include "engine_defines.h"

Tga2D::CShader::CShader(CEngine* aEngine)
	:myEngine(aEngine)
	, myDirect3dEngine(&aEngine->GetDirect3D())
	, myLightBuffer(0)
	, myCommonBuffer(0)
	, myIsListeningOnFileChange(false)
	, myRandomSeed(rand() % 100)
{
	myIsReadyToRender = false;
}

Tga2D::CShader::~CShader()
{
	SAFE_RELEASE(myVertexShader);
	SAFE_RELEASE(myPixelShader);
	SAFE_RELEASE(myLayout);
	SAFE_RELEASE(myLightBuffer);
	SAFE_RELEASE(myCommonBuffer);
	
}

bool Tga2D::CShader::CreateShaders(const char* aVertex, const char* aPixel, callback_layout aLayout, std::string aPsMain, std::string aVsMain)
{
	myPsMain = aPsMain;
	myVsMain = aVsMain;
	myIsReadyToRender = false;
	myVertexShaderFile = aVertex;
	myPixelShaderFile = aPixel;

	if (!myIsListeningOnFileChange)
	{
		myEngine->GetFileWatcher()->WatchFileChange(aVertex, std::bind(&Tga2D::CShader::OnShaderFileModified, this, std::placeholders::_1));
		myEngine->GetFileWatcher()->WatchFileChange(aPixel, std::bind(&Tga2D::CShader::OnShaderFileModified, this, std::placeholders::_1));
	
		
		myIsListeningOnFileChange = true;
	}

	ID3D10Blob *VS = nullptr;
	ID3D10Blob *PS = nullptr;
	if (!myEngine->GetDirect3D().CompileShader(aVertex, aVsMain.c_str(), "vs_4_0", VS))
	{
		ERROR_PRINT("%s%s", "Shader error in file: ", aVertex);
		return false;
	}
	if (!myEngine->GetDirect3D().CompileShader(aPixel, aPsMain.c_str(), "ps_4_0", PS))
	{
		ERROR_PRINT("%s%s", "Shader error in file: ", aPixel);
		return false;
	}
	myEngine->GetDirect3D().GetDevice()->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &myVertexShader);
	myEngine->GetDirect3D().GetDevice()->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &myPixelShader);

	if (myLayout)
	{
		myLayout->Release();
		myLayout = nullptr;
	}

	if (aLayout)
	{
		aLayout(VS);
	}
	else if (!CreateInputLayout(VS))
	{
		// LAYOUT
		D3D11_INPUT_ELEMENT_DESC polygonLayout[7];

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

		polygonLayout[2].SemanticName = "TEXCOORD";
		polygonLayout[2].SemanticIndex = 1;
		polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
		polygonLayout[2].InputSlot = 1;
		polygonLayout[2].AlignedByteOffset = 0;
		polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		polygonLayout[2].InstanceDataStepRate = 1;

		polygonLayout[3].SemanticName = "TEXCOORD";
		polygonLayout[3].SemanticIndex = 2;
		polygonLayout[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[3].InputSlot = 1;
		polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		polygonLayout[3].InstanceDataStepRate = 1;

		polygonLayout[4].SemanticName = "TEXCOORD";
		polygonLayout[4].SemanticIndex = 3;
		polygonLayout[4].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[4].InputSlot = 1;
		polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		polygonLayout[4].InstanceDataStepRate = 1;

		polygonLayout[5].SemanticName = "TEXCOORD";
		polygonLayout[5].SemanticIndex = 4;
		polygonLayout[5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[5].InputSlot = 1;
		polygonLayout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[5].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		polygonLayout[5].InstanceDataStepRate = 1;

		polygonLayout[6].SemanticName = "TEXCOORD";
		polygonLayout[6].SemanticIndex = 5;
		polygonLayout[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[6].InputSlot = 1;
		polygonLayout[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[6].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		polygonLayout[6].InstanceDataStepRate = 1;


		unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

		// Create the vertex input layout.
		HRESULT result = myEngine->GetDirect3D().GetDevice()->CreateInputLayout(polygonLayout, numElements, VS->GetBufferPointer(), VS->GetBufferSize(), &myLayout);
		if (FAILED(result))
		{
			ERROR_PRINT("%s", "Layout error");
			return false;
		}
	}

	VS->Release();
	VS = 0;

	PS->Release();
	PS = 0;

	HRESULT result = S_OK;
	if (myCommonBuffer)
	{
		myCommonBuffer->Release();
		myCommonBuffer = nullptr;
	}

	D3D11_BUFFER_DESC commonBufferDesc;
	commonBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	commonBufferDesc.ByteWidth = sizeof(CommonBufferType);
	commonBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	commonBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	commonBufferDesc.MiscFlags = 0;
	commonBufferDesc.StructureByteStride = 0;

	result = myEngine->GetDirect3D().GetDevice()->CreateBuffer(&commonBufferDesc, NULL, &myCommonBuffer);

	// LIGHTS
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	if (myLightBuffer)
	{
		myLightBuffer->Release();
		myLightBuffer = nullptr;
	}
	LightBufferType lightType;
	D3D11_SUBRESOURCE_DATA lightData;
	lightData.pSysMem = &lightType;
	lightData.SysMemPitch = 0;
	lightData.SysMemSlicePitch = 0;

	result = myEngine->GetDirect3D().GetDevice()->CreateBuffer(&lightBufferDesc, &lightData, &myLightBuffer);
	
	myIsReadyToRender = true;
	return true;
}

void Tga2D::CShader::OnShaderFileModified(std::string aFile)
{
	INFO_PRINT("%s%s%s", "Shader: ", aFile.c_str(), " modified! Trying to reload...");

	ID3D10Blob *VS = nullptr;
	ID3D10Blob *PS = nullptr;
	if (!myEngine->GetDirect3D().CompileShader(myVertexShaderFile.c_str(), myVsMain.c_str(), "vs_4_0", VS))
	{
		ERROR_PRINT("%s%s", "Shader modified failed! error in file: ", myVertexShaderFile.c_str());
		return;
	}
	if (!myEngine->GetDirect3D().CompileShader(myPixelShaderFile.c_str(), myPsMain.c_str(), "ps_4_0", PS))
	{
		ERROR_PRINT("%s%s", "Shader modified failed! error in file: ", myPixelShaderFile.c_str());
		return;
	}

	VS->Release();
	PS->Release();

	if (myPixelShader)
	{
		myPixelShader->Release();
		myPixelShader = nullptr;
		
	}
	if (myVertexShader)
	{
		myVertexShader->Release();
		myVertexShader = nullptr;
	}

	CreateShaders(myVertexShaderFile.c_str(), myPixelShaderFile.c_str(), nullptr, myPsMain, myVsMain);
	INFO_PRINT("%s%s%s", "Shader: ", aFile.c_str(), " reloaded successfully");
}

bool Tga2D::CShader::Render(CRenderObject* aObject)
{
	if (!myVertexShader || !myPixelShader || !myIsReadyToRender || !myEngine)
	{
		return false;
	}

	CDirectEngine& engine = myEngine->GetDirect3D();
	engine.SetSamplerState(aObject->mySamplerFilter, aObject->mySamplerAddressMode);
	engine.SetBlendState(aObject->myBlendState);
	engine.GetContext()->VSSetShader(myVertexShader, NULL, 0);
	engine.GetContext()->PSSetShader(myPixelShader, NULL, 0);
	engine.GetContext()->IASetInputLayout(myLayout);
	engine.GetContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11ShaderResourceView* textures[1];
	textures[0] = myEngine->GetTextureManager().GetNoiseTexture();
	engine.GetContext()->PSSetShaderResources(0, 1, textures);
	DoOneFrameUpdates(aObject);

	return true;
}

void Tga2D::CShader::DoOneFrameUpdates(CRenderObject* /*aObject*/)
{
	CDirectEngine& directxengine = myEngine->GetDirect3D();

	if (!myCommonBuffer)
	{
		return;
	}
	
	D3D11_MAPPED_SUBRESOURCE mappedResourceCommon;
	CommonBufferType* dataPtrCommon;
	HRESULT result = directxengine.GetContext()->Map(myCommonBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResourceCommon);
	if (FAILED(result))
	{
		INFO_PRINT("Error in rendering!");
		return;
	}

	dataPtrCommon = (CommonBufferType*)mappedResourceCommon.pData;
	dataPtrCommon->myResolution = Vector4f(static_cast<float>(myEngine->GetRenderSize().x), static_cast<float>(myEngine->GetRenderSize().y), 0, 0);
	dataPtrCommon->myTimings.x = myEngine->myTotalTime;
	dataPtrCommon->myTimings.y = myEngine->myDeltaTime;
	dataPtrCommon->myRandomValues.x = static_cast<float>(myRandomSeed);
	dataPtrCommon->myRandomValues.y = (static_cast<float>(((rand() % 2000) - 1000)) / 1000.0f);


	directxengine.GetContext()->Unmap(myCommonBuffer, 0);
	directxengine.GetContext()->VSSetConstantBuffers(EConstantBufferSlot_Common, 1, &myCommonBuffer);
	directxengine.GetContext()->PSSetConstantBuffers(EConstantBufferSlot_Common, 1, &myCommonBuffer);

	// LIGHTS
#ifdef USE_LIGHTS
	if (myLightBuffer)
	{

		D3D11_MAPPED_SUBRESOURCE mappedResourceLight;
		LightBufferType* dataPtrLights;

		result = directxengine.GetContext()->Map(myLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResourceLight);
		if (FAILED(result))
		{
			INFO_PRINT("Error in rendering! Could not map lights");
			return;
		}

		std::vector<const CLight*> lights = myEngine->GetLightManager().myLightsToRender;
		dataPtrLights = (LightBufferType*)mappedResourceLight.pData;
		dataPtrLights->myNumberOfLights = static_cast<unsigned int>(lights.size());

		for (unsigned int i = 0; i < NUMBER_OF_LIGHTS_ALLOWED; i++)
		{
			dataPtrLights->myLights[i].myLightColors.Set(0, 0, 0, 0);
			dataPtrLights->myLights[i].myLightPositions.Set(0, 0, 0, 0);
		}

		for (unsigned int i = 0; i < lights.size(); i++)
		{
			dataPtrLights->myLights[i].myLightColors = lights[i]->myColor;
			dataPtrLights->myLights[i].myLightPositions.Set(((lights[i]->myPosition.x)) * myEngine->GetWindowSize().x, ((lights[i]->myPosition.y) ) * myEngine->GetWindowSize().y, lights[i]->myIntensity, lights[i]->myFallOff);
		}
		dataPtrLights->myAmbience = myEngine->GetLightManager().GetAmbientLight();

		directxengine.GetContext()->Unmap(myLightBuffer, 0);

		
	}

#endif
	directxengine.GetContext()->PSSetConstantBuffers(EConstantBufferSlot_Light, 1, &myLightBuffer);
	directxengine.GetContext()->VSSetConstantBuffers(EConstantBufferSlot_Light, 1, &myLightBuffer);
}