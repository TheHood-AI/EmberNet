#pragma once
#include <array>
#include "render/render_common.h"
#include <thread>
struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Buffer;
struct ID3D11InputLayout;
struct ID3D10Blob;
struct ID3D11SamplerState;
struct ID3D11Texture2D; 
struct ID3D11DepthStencilState;
struct ID3D11DepthStencilView;
struct ID3D11RasterizerState;
struct ID3D11DeviceChild;
struct ID3D11BlendState;
struct ID3D11Debug;
struct IDXGIAdapter;
struct D3D11_VIEWPORT;
namespace Tga2D
{ 
	class CEngine;
	class CTexturedQuad;
	class CRendertarget;
	class CRenderObject;
	class CRenderObjectSprite;
	class CRenderObjectLine;
	class CTexturedQuadDrawer;
	class CLineDrawer;
	class CTexturedQuadBatch;
	class CTexturedQuadBatchDrawer;
	class CCustomShapeDrawer;
	class CRenderObjectCustom;
	class CLineMultiPrimitive;
	class CDirectEngine
	{
	public:
		CDirectEngine(void);
		~CDirectEngine(void);
		bool Init(const CEngine& aEngine, VECTOR2UI aWindowSize, SEngineCreateParameters& someParameters);
		bool IsInitiated();
		void PostRenderFrame();
		void RenderFrame();
		void SetClearColor(CColor aColor);
		void SetWorldMatrixPosition(VECTOR2F aCorrectedPosition, VECTOR2F aPosition);
		void SetWorldMatrixZoom(float aZoom);
		ID3D11DeviceContext* GetContext() const {return myDeviceContext;}
		ID3D11Device* GetDevice() const {return myDevice;}
	
		void Draw(CRenderObject* aObject);
		void DoDraw(CRenderObjectSprite* aObject);
		void DoDraw(CRenderObjectLine* aObject);
		void DoDraw(CTexturedQuadBatch* aBatch);
		void DoDraw(CRenderObjectCustom* aCustomObject);
		void DoDraw(CLineMultiPrimitive* aObject);
		bool CompileShader(const char* aShader, const char* aMainFunction, const char* aTarget, ID3D10Blob*& aCodeBlob);
		VECTOR2UI myWindowSize;
		VECTOR2UI myTargetSize;

		void SetResolution(VECTOR2UI aResolution);
		void SetFullScreen(bool aFullScreen);
		void SetViewPort(float aTopLeftX, float aTopLeftY, float aWidth, float aHeight, float aMinDepth = 0.0f, float aMaxDepth = 1.0f);
		
		void SetBlendState(EBlendState aState);
		void SetSamplerState(ESamplerFilter aFilter, ESamplerAddressMode aAddressMode);

		Matrix44 myCorrectedWorldWatrix;
		Matrix44 myWorldWatrix;
		Matrix44 myViewWatrix;
		Matrix44 myProjWatrix;

        int GetObjectRenderCount();


		void SetRenderTarget(CSprite* aSpriteTarget);

        void Clear( const CColor &aColor );
		bool IsOnSameThreadAsEngine(std::thread::id aOtherThread) const { return aOtherThread == myThreadID; }
	private:
		bool CreateBlendStates();
		bool CreateSamplers();

		bool File_exist(const char *fileName);
		bool CollectAdapters(VECTOR2UI aWindowSize, VECTOR2I& aNumDenumerator, IDXGIAdapter*& outAdapter);
		void SetShaderParametersForSprite(CRenderObjectSprite* aObject);
		void UpdateVertexes(CTexturedQuad* aQuad);
		void SetDebugObjectName(_In_ ID3D11DeviceChild* resource, _In_z_ std::string aName);

		

		IDXGISwapChain *mySwapchain;
		ID3D11Device *myDevice;
		ID3D11DeviceContext *myDeviceContext;
		CRendertarget* myRendertarget;
		ID3D11RenderTargetView *myBackbuffer;
		std::thread::id myThreadID;

		std::array<std::array<ID3D11SamplerState*, ESamplerAddressMode::ESamplerAddressMode_Count>, ESamplerFilter::ESamplerFilter_Count> mySamplerStates;
		ID3D11Texture2D* myDepthStencilBuffer;
		ID3D11DepthStencilState* myDepthStencilState;
		ID3D11DepthStencilView* myDepthStencilView;
		ID3D11RasterizerState* myRasterState;
		std::array<ID3D11BlendState*, EBlendState::EBlendState_Count> myBlendStates;
		ID3D11Debug *myD3dDebug;
			
		int myRenderedCount;

		CTexturedQuadDrawer* myTexturedQuadDrawer;
		CCustomShapeDrawer* myCustomShapeDrawer;
		CLineDrawer* myLineDrawer;
		CTexturedQuadBatchDrawer* myTexturedQuadBatchDrawer;
		CColor myClearColor;
		int myVideoCardMemory;
		bool myEnableVSync;
		bool myIsInitiated;
		D3D11_VIEWPORT* mySavedViewport;
		bool myHasSavedViewPort;
	};
}