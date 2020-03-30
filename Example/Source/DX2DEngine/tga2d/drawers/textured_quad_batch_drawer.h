#pragma once

#include "render/render_common.h"
#include "render/render_object.h"
#include "shaders/shader_common.h"

namespace Tga2D
{

	class CShaderNormalInstanced;
	class CTexturedQuadBatch;
	class CSprite;
	class CTexturedQuadBatchDrawer
	{
	public:
		CTexturedQuadBatchDrawer(CDirectEngine* anEngine);
		~CTexturedQuadBatchDrawer();
		void Init();
		void Draw(CTexturedQuadBatch* aBatch);
	private:
		bool InitShaders();
		void CreateBuffer();

		ID3D11Buffer *myVertexBuffer;    // global
		ID3D11Buffer* myInstanceBuffer;
		SVertexInstanced myVerticies[6];
		CDirectEngine* myEngine;

		ID3D11ShaderResourceView* myLastResourceUsed;

		std::vector<ObjectBufferInstanceType> myBufferedObjects;


		CShaderNormalInstanced* myNormalShaderInstanced;
		bool myIsLoaded;
	};
}
