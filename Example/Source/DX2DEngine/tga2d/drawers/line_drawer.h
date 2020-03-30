#pragma once
#include <tga2d/render/render_common.h>
#include <tga2d/shaders/Shader.h>

namespace Tga2D
{
    struct LineBufferType
    {
        VECTOR2F myPosition;
    };

    class CRenderObjectLine;
    class CDirectEngine;
    class CLineDrawer : public CShader
    {
    public:
        CLineDrawer(CEngine* anEngine);
        ~CLineDrawer();
        bool Init() override;
        void Draw(CRenderObjectLine* aObject);
		void Draw(CLineMultiPrimitive* aObject);
    private:
        CLineDrawer &operator =( const CLineDrawer &anOther ) = delete;
        bool CreateInputLayout(ID3D10Blob* aVS) override;
        bool InitShaders();
        void CreateBuffer();

        void SetShaderParameters(CRenderObjectLine* aObject);
        void UpdateVertexes(CRenderObjectLine* aObject);
        ID3D11Buffer *myVertexBuffer;    // global
        SSimpleVertex myVerticies[2];
        CDirectEngine& myEngine;
    };
}