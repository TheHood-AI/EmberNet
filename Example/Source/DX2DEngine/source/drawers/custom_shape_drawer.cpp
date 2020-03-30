#include "stdafx.h"

#include "drawers/custom_shape_drawer.h"
#include "render/render_object.h"
#include "d3d/direct_3d.h"
#include "engine.h"
#include "light/light_manager.h"
#include "shaders/shader_common.h"
#include "primitives/custom_shape.h"
#include "texture/texture.h"
#include "engine_defines.h"

using namespace Tga2D;

CCustomShapeDrawer::CCustomShapeDrawer( CEngine* anEngine )
    :CShader( anEngine )
    , myEngine( anEngine->GetDirect3D() )
{
}

CCustomShapeDrawer::~CCustomShapeDrawer()
{
	SAFE_RELEASE(myVertexBuffer);
	SAFE_RELEASE(myObjectBuffer);
}

bool CCustomShapeDrawer::Init()
{
    myMaxPoints = MAX_POINTS_IN_CUSTOM_SHAPE;
    CShader::Init();

    D3D11_BUFFER_DESC objectBufferDesc;
    objectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    objectBufferDesc.ByteWidth = sizeof( ObjectBufferTypeCustomShape );
    objectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    objectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    objectBufferDesc.MiscFlags = 0;
    objectBufferDesc.StructureByteStride = 0;

    HRESULT result;
    result = myEngine.GetDevice()->CreateBuffer( &objectBufferDesc, NULL, &myObjectBuffer );
    if( FAILED( result ) )
    {
        ERROR_PRINT( "%s", "Object Buffer error" );
        return false;
    }

    InitShaders();
    CreateBuffer();
    return true;
}

void CCustomShapeDrawer::CreateBuffer()
{
    D3D11_BUFFER_DESC vertexBufferDesc;

    ZeroMemory( &vertexBufferDesc, sizeof( vertexBufferDesc ) );

    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof( SSimpleVertex ) * myMaxPoints;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    HRESULT hr = myEngine.GetDevice()->CreateBuffer( &vertexBufferDesc, nullptr, &myVertexBuffer );
    if( FAILED( hr ) )
    {
        ERROR_PRINT( "%s", "Buffer error" );
        return;
    }
}

bool CCustomShapeDrawer::InitShaders()
{
    CreateShaders( "shaders/custom_shape.fx", "shaders/custom_shape.fx" );

    return true;
}

bool CCustomShapeDrawer::CreateInputLayout( ID3D10Blob* aVS )
{
    // LAYOUT
    D3D11_INPUT_ELEMENT_DESC polygonLayout[3];

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

	polygonLayout[2].SemanticName = "TEXCOORD";
	polygonLayout[2].SemanticIndex = 1;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

    unsigned int numElements = sizeof( polygonLayout ) / sizeof( polygonLayout[0] );

    // Create the vertex input layout.
    HRESULT result = myEngine.GetDevice()->CreateInputLayout( polygonLayout, numElements, aVS->GetBufferPointer(), aVS->GetBufferSize(), &myLayout );
    if( FAILED( result ) )
    {
        ERROR_PRINT( "%s", "Layout error" );
        return false;
    }
    return true;
}

int CCustomShapeDrawer::SetShaderParameters( CRenderObjectCustom* aObject )
{
    if( !myObjectBuffer )
    {
        return 0;
    }
    int tris = UpdateVertexes( aObject );

    D3D11_MAPPED_SUBRESOURCE mappedObjectResource;

    HRESULT result = myDirect3dEngine->GetContext()->Map( myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedObjectResource );
    if( FAILED( result ) )
    {
        INFO_PRINT( "Error in rendering!" );
        return 0;
    }

    ObjectBufferTypeCustomShape* objectDataPtr;
    objectDataPtr = (ObjectBufferTypeCustomShape*)mappedObjectResource.pData;

    bool haveLights = CEngine::GetInstance()->GetLightManager().GetNumberOfLights() > 0;
    float lightType = haveLights ? 1.0f : 0.0f;

    objectDataPtr->myRotationAndSize = Vector4f(aObject->GetRotation(), aObject->GetSize().x, aObject->GetSize().y, lightType );
	objectDataPtr->myPosition.x = aObject->GetPosition().x;
	objectDataPtr->myPosition.y = aObject->GetPosition().y;

    myDirect3dEngine->GetContext()->Unmap( myObjectBuffer, 0 );

    unsigned int strides = sizeof( SSimpleVertex );
    unsigned int offsets = 0;
    myEngine.GetContext()->IASetVertexBuffers( 0, 1, &myVertexBuffer, &strides, &offsets );

	if (CCustomShape* shape = static_cast<CCustomShape*>(aObject))
	{
		myEngine.GetContext()->PSSetShaderResources(0, 1, &shape->GetTexture()->myResource);
	}

	
    myDirect3dEngine->GetContext()->VSSetConstantBuffers( EConstantBufferSlot_ObjectCustom, 1, &myObjectBuffer );
    return tris;
}

int CCustomShapeDrawer::UpdateVertexes( CRenderObjectCustom* aObject )
{
    D3D11_MAPPED_SUBRESOURCE mappedVertexResource;
    SSimpleVertex* dataVertexPtr;
    HRESULT result = myEngine.GetContext()->Map( myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexResource );
    if( FAILED( result ) )
    {
        INFO_PRINT( "Error in rendering!" );
        return 0;
    }

    dataVertexPtr = (SSimpleVertex*)mappedVertexResource.pData;

    int index = 0;
    for( SCustomPoint& point : aObject->myPoints )
    {
        if( index >= myMaxPoints )
        {
            INFO_PRINT( "%s%i%s", "Customshape:Render - Too many points rendered at one custom shape! We support: ", myMaxPoints, " skipping the rest, increase this nuber in engine_defines.h" );
            continue;
        }

		VECTOR2F offsetPos(Tga2D::CEngine::GetInstance()->GetWindowRatio(), -1.0f);
		VECTOR2F correctedPositionFrom = VECTOR2F((point.myPosition.x * 2.0f) - 1.0f, -(point.myPosition.y * 2.0f) + 1.0f);

        dataVertexPtr[index].X = correctedPositionFrom.x;
        dataVertexPtr[index].Y = correctedPositionFrom.y;
        dataVertexPtr[index].Z = 0;

        dataVertexPtr[index].myColorR = point.myColor.myR;
        dataVertexPtr[index].myColorG = point.myColor.myG;
        dataVertexPtr[index].myColorB = point.myColor.myB;
        dataVertexPtr[index].myColorA = point.myColor.myA;

		dataVertexPtr[index].myU = point.myUV.x;
		dataVertexPtr[index].myV = point.myUV.y;

        index++;
    }

    myEngine.GetContext()->Unmap( myVertexBuffer, 0 );
    return index;
}

void Tga2D::CCustomShapeDrawer::Draw( CRenderObjectCustom* aObject )
{
	if (!Render(aObject))
	{
		return;
	}
	myEngine.SetBlendState(aObject->myBlendState);
    myEngine.GetContext()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    int tris = SetShaderParameters( aObject );
    if( tris > 0 && tris % 3 == 0 )
    {
        myEngine.GetContext()->Draw( tris, 0 );
    }
}
