#pragma once
#include "render/render_common.h"
#include "render/render_object.h"

namespace Tga2D
{


	class CShaderNormal;
	class CDirectEngine;
	class CTexturedQuad;
	class CTexturedQuadDrawer
	{
	public:
		CTexturedQuadDrawer(CDirectEngine* anEngine);
		~CTexturedQuadDrawer(void);
		void Init();
		void Draw(CRenderObjectSprite* aObject);
	private:
		bool InitShaders();
		void CreateBuffer();

		void SetShaderParameters(CRenderObjectSprite* aObject);
		void UpdateVertexes(CTexturedQuad* aQuad);
		ID3D11Buffer *myVertexBuffer;    // global
		ID3D11Buffer* myObjectBuffer;
		SVertex myVerticies[6];
		CDirectEngine* myEngine;
		ID3D11InputLayout *myLayout;            // the pointer to the input layout

		//CShaderNormal* myNormalShader;
		CShaderNormal* myShaders[EShader_MAX];
	};

}