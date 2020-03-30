#include "stdafx.h"
#include "drawers/line_drawer.h"
#include "d3d/direct_3d.h"
#include "render/render_object.h"
#include "shaders/Shader.h"
#include "engine.h"
#include "primitives/line_primitive.h"

using namespace Tga2D;
CLineDrawer::CLineDrawer(CEngine* anEngine)
	:CShader(anEngine)
	,myEngine(anEngine->GetDirect3D())
{
}


CLineDrawer::~CLineDrawer()
{
	SAFE_RELEASE(myVertexBuffer);
}

bool CLineDrawer::Init()
{
	CShader::Init();
	InitShaders();
	CreateBuffer();
	return true;
}


void CLineDrawer::CreateBuffer()
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	float theZ = 0.0f;
	myVerticies[0].X = -0.5f;
	myVerticies[0].Y = 0.5f;
	myVerticies[0].Z = theZ;

	myVerticies[1].X = -0.5f;
	myVerticies[1].Y = -0.5f;
	myVerticies[1].Z = theZ;

	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;       
	vertexBufferDesc.ByteWidth = sizeof(SSimpleVertex) * 2000;        
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = myVerticies;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HRESULT hr = myEngine.GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &myVertexBuffer);
	if (FAILED(hr))
	{
		ERROR_PRINT("%s", "Buffer error");
		return;
	}

}

void Tga2D::CLineDrawer::Draw(CLineMultiPrimitive* aObject)
{
	if (!myVertexBuffer)
	{
		return;
	}
	Render(aObject);
	myEngine.GetContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

	D3D11_MAPPED_SUBRESOURCE mappedVertexResource;
	SSimpleVertex* dataVertexPtr;
	HRESULT result = myEngine.GetContext()->Map(myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexResource);
	if (FAILED(result))
	{
		INFO_PRINT("Error in rendering!");
		return;
	}

	dataVertexPtr = (SSimpleVertex*)mappedVertexResource.pData;
	int theCount = 0;
	for (unsigned int i = 0; i < aObject->myCount; i++)
	{

		VECTOR2F correctedPositionFrom = VECTOR2F((aObject->myFromPosition[i].x * 2.0f) - 1.0f, -(aObject->myFromPosition[i].y * 2) + 1);
		static const float ratioX = (float)CEngine::GetInstance()->GetWindowSize().x / (float)CEngine::GetInstance()->GetWindowSize().y;

		VECTOR2F correctedPositionTo = VECTOR2F((aObject->myToPosition[i].x * 2.0f) - 1.0f, -(aObject->myToPosition[i].y * 2) + 1);

		dataVertexPtr[theCount].X = correctedPositionFrom.x;
		dataVertexPtr[theCount].Y = correctedPositionFrom.y;
		dataVertexPtr[theCount].Z = 0;

		dataVertexPtr[theCount + 1].X = correctedPositionTo.x;
		dataVertexPtr[theCount + 1].Y = correctedPositionTo.y;
		dataVertexPtr[theCount + 1].Z = 0;

		dataVertexPtr[theCount].myColorA = aObject->myColor[i].myA;
		dataVertexPtr[theCount].myColorR = aObject->myColor[i].myR;
		dataVertexPtr[theCount].myColorG = aObject->myColor[i].myG;
		dataVertexPtr[theCount].myColorB = aObject->myColor[i].myB;

		dataVertexPtr[theCount+1].myColorA = aObject->myColor[i].myA;
		dataVertexPtr[theCount+1].myColorR = aObject->myColor[i].myR;
		dataVertexPtr[theCount+1].myColorG = aObject->myColor[i].myG;
		dataVertexPtr[theCount+1].myColorB = aObject->myColor[i].myB;
		theCount += 2;
	}

	
	myEngine.GetContext()->Unmap(myVertexBuffer, 0);


	unsigned int strides = sizeof(SSimpleVertex);
	unsigned int offsets = 0;
	myEngine.GetContext()->IASetVertexBuffers(0, 1, &myVertexBuffer, &strides, &offsets);

	myEngine.GetContext()->Draw(aObject->myCount * 2, 0);
}

bool Tga2D::CLineDrawer::InitShaders()
{
	CreateShaders("shaders/lineshader.fx", "shaders/lineshader.fx");

	return true;
}

bool CLineDrawer::CreateInputLayout(ID3D10Blob* aVS)
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
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;


	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	HRESULT result = myEngine.GetDevice()->CreateInputLayout(polygonLayout, numElements, aVS->GetBufferPointer(), aVS->GetBufferSize(), &myLayout);
	if (FAILED(result))
	{
		ERROR_PRINT("%s", "Layout error");
		return false;
	}
	return true;
}

void CLineDrawer::SetShaderParameters(CRenderObjectLine* aObject)
{
	UpdateVertexes(aObject);


	unsigned int strides = sizeof(SSimpleVertex);
	unsigned int offsets = 0;
	myEngine.GetContext()->IASetVertexBuffers(0, 1, &myVertexBuffer, &strides, &offsets);
}

void CLineDrawer::UpdateVertexes(CRenderObjectLine* aObject)
{
	D3D11_MAPPED_SUBRESOURCE mappedVertexResource;
	SSimpleVertex* dataVertexPtr;
	HRESULT result = myEngine.GetContext()->Map(myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexResource);
	if(FAILED(result))
	{
		INFO_PRINT("Error in rendering!");
		return;
	}

	VECTOR2F correctedPositionFrom = VECTOR2F( (aObject->myFromPosition.x * 2.0f) -1.0f, -(aObject->myFromPosition.y *2) +1) ;
	static const float ratioX = (float)CEngine::GetInstance()->GetWindowSize().x / (float)CEngine::GetInstance()->GetWindowSize().y;
	//correctedPositionFrom.x *= ratioX;

	VECTOR2F correctedPositionTo = VECTOR2F((aObject->myToPosition.x * 2.0f) - 1.0f, -(aObject->myToPosition.y * 2) + 1);
	//correctedPositionTo.x *= ratioX;

	//VECTOR2F correctedPosition = DX2D::VECTOR2F((aObject->myPosition.x * 2.0f) - 1.0f, (aObject->myPosition.y * 2.0f));

	dataVertexPtr = (SSimpleVertex*)mappedVertexResource.pData;

	dataVertexPtr[0].X = correctedPositionFrom.x;
	dataVertexPtr[0].Y = correctedPositionFrom.y;
	dataVertexPtr[0].Z = 0;

	dataVertexPtr[1].X = correctedPositionTo.x;
	dataVertexPtr[1].Y = correctedPositionTo.y;
	dataVertexPtr[1].Z = 0;

	dataVertexPtr[0].myColorA = aObject->myColor.w;
	dataVertexPtr[0].myColorR = aObject->myColor.x;
	dataVertexPtr[0].myColorG = aObject->myColor.y;
	dataVertexPtr[0].myColorB = aObject->myColor.z;

	dataVertexPtr[1].myColorA = aObject->myColor.w;
	dataVertexPtr[1].myColorR = aObject->myColor.x;
	dataVertexPtr[1].myColorG = aObject->myColor.y;
	dataVertexPtr[1].myColorB = aObject->myColor.z;



	myEngine.GetContext()->Unmap(myVertexBuffer, 0);
}


void CLineDrawer::Draw(CRenderObjectLine* aObject)
{
	Render(aObject);
	myEngine.GetContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

	SetShaderParameters(aObject);
	myEngine.GetContext()->Draw(2, 0);
}