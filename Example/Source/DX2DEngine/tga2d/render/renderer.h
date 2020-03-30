#pragma once
#include "render_common.h"

struct ID3D11ShaderResourceView;
namespace Tga2D
{
	class CRenderObject;
	class CTexturedQuad;
	class CLight;
	class CDirectEngine;
	class CRenderer
	{
	public:
		CRenderer();
		~CRenderer();
		bool DoRender();
		void AddToRender(CRenderObject* aObject);
		void AddToRender(CTexturedQuad* aObject);
		void Update();
	private:

		struct SOptimTip
		{
			SOptimTip()
			{
				myCount = 0;
				myHasWarned = false;
			}
			unsigned int myCount;
			bool myHasWarned;
			std::string myPath;
		};
		
		typedef std::map<ID3D11ShaderResourceView*, SOptimTip> OptimisationMap;
		OptimisationMap myRenderedSpritesOfSameType;
		CDirectEngine* myDXEngine;
		
	};
}