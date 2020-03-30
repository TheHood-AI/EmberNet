#pragma once

#include "shader.h"
#include "shader_common.h"

namespace Tga2D
{
	struct ObjectBufferType
	{
		Vector4f myRotationAndSize;
		VECTOR2F myPosition;
		VECTOR2F myUnUsed2;
		Vector4f myColor;
		Vector4f myPivot;
		Vector4f myUV;
	};


	class CRenderObjectSprite;
	class CTexturedQuad;
	class CShaderNormal : public CShader
	{
	public:
		CShaderNormal(CEngine* aEngine);
		~CShaderNormal();

		bool Init() override;
		bool Init(std::string aPixelShaderFile, std::string aVertexShaderFile, std::string aPixelMainFunction, std::string aVertexMainFunction);
		void Render(CRenderObjectSprite* aObject, ID3D11Buffer* aObjectBuffer, ID3D11Buffer* aVertexBuffer);
		bool CreateInputLayout(ID3D10Blob* aVS) override;
	private:
		void SetShaderParameters(CRenderObjectSprite* aObject, ID3D11Buffer* aObjectBuffer, ID3D11Buffer* aVertexBuffer);
		void UpdateVertexes(CTexturedQuad* aQuad, ID3D11Buffer* aVertexBuffer);
	};

}