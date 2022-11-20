#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLMaterial.hpp"
#include "Graphics/Rendering/Resources/Material.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;



GADRMaterial::GADRMaterial()
	: mpMaterial(nullptr)
{}

GADRMaterial::GADRMaterial(Renderer* pRenderer, Material* pMaterial)
	: mpMaterial(pMaterial)
{
	Create();
}

GADRMaterial::~GADRMaterial()
{
	Destroy();
}

void GADRMaterial::Create()
{
	assert(mpMaterial != nullptr);
}

void GADRMaterial::Destroy()
{
	if (mpMaterial)
	{
		mpMaterial = nullptr;
	}
}

void GADRMaterial::Bind(uint32_t currentBufferIdx)
{
	//
}
#endif // OPENGL_RENDERER