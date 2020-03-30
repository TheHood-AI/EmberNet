#pragma once
#include <tga2d/math/matrix44.h>
#include <tga2d/render/render_object.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <engine_defines.h>


namespace Tga2D
{
    struct MatrixBufferType
    {
        Matrix44 world;
        Matrix44 view;
        Matrix44 projection;
    };

    struct CommonBufferType
    {
        Vector4f myResolution;
        Vector4f myTimings;
        Vector4f myRandomValues;
    };


    struct SLightInstance
    {
        Vector4f myLightColors;
        Vector4f myLightPositions;
    };

    struct LightBufferType
    {
		LightBufferType()
		{
			myAmbience = 1.0f;
			myNumberOfLights = 0;
			unused2 = 0;
			unused3 = 0;
		}
        SLightInstance myLights[NUMBER_OF_LIGHTS_ALLOWED];
        unsigned int myNumberOfLights;
        float myAmbience;
        unsigned int unused2;
        unsigned int unused3;
    };

    class CDirectEngine;
    class CEngine;
    class CShader
    {
    public:
        CShader(CEngine* aEngine);
        virtual ~CShader();
        virtual bool Init(){ return false; }

		typedef std::function<void(ID3D10Blob* aBlob)> callback_layout;
		bool CreateShaders(const char* aVertex, const char* aPixel, callback_layout aLayout = nullptr, std::string aPsMain = "PShader", std::string aVsMain = "VShader");
        bool Render(CRenderObject* aObject);
       
		ID3D11InputLayout *myLayout;            // the pointer to the input layout
    protected:
		virtual bool CreateInputLayout(ID3D10Blob* aVS) { aVS; return false; }
        void OnShaderFileModified(std::string aFile);
        void DoOneFrameUpdates(CRenderObject* aObject);
        ID3D11VertexShader *myVertexShader;    // the vertex shader
        ID3D11PixelShader *myPixelShader;     // the pixel shader
       
       
        CDirectEngine* myDirect3dEngine;
        CEngine* myEngine;
        /*ID3D11Buffer* myMatrixBuffer;*/
        ID3D11Buffer* myLightBuffer;
        ID3D11Buffer* myCommonBuffer;

		bool myIsReadyToRender;
		bool myIsListeningOnFileChange;
		int myRandomSeed;

        std::string myVertexShaderFile;
        std::string myPixelShaderFile;
		std::string myPsMain;
		std::string myVsMain;
       
    };
}