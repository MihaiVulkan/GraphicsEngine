#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/OpenGLRenderer.hpp"

#include "Foundation/Platform/Platform.hpp"

#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"

#include "Graphics/Rendering/RenderQueue.hpp"


#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/SceneGraph/LightNode.hpp"
#include "Graphics/Lights/DirectionalLight.hpp"
#include "Graphics/Cameras/Camera.hpp"

#include "Graphics/Components/VisualComponent.hpp"
#include "Graphics/Components/MaterialComponent.hpp"

#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"


#include "Graphics/Rendering/Resources/Texture.hpp"
#include "Graphics/Rendering/Resources/RenderTarget.hpp"
#include "Graphics/Rendering/Resources/Material.hpp"
#include "Graphics/Rendering/Resources/Model.hpp"
#include "Graphics/Rendering/Resources/IndexBuffer.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"

#include "Graphics/ShaderTools/GLSL/GLSLShaderParser.hpp"


#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLHelpers.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLTextureObject.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLFramebuffer.hpp"

// Common
#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLCommon.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLUtils.hpp"

// Resources
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLVertexFormat.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLVertexBuffer.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLIndexBuffer.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLUniformBuffer.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLShader.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLTexture.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLMaterial.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLModel.hpp"

#include "Foundation/MemoryManagement/MemoryOperations.hpp"

//#define PIPELINE_STATS

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

OpenGLRenderer::OpenGLRenderer()
	: Renderer()
	, mpWindow(nullptr)
	, mCurrentBufferIdx(0)
{}

OpenGLRenderer::OpenGLRenderer(Platform::Window* pWindow)
	: Renderer(pWindow)
	, mpWindow(pWindow)
	, mCurrentBufferIdx(0)

{
	Init(pWindow);
}

OpenGLRenderer::~OpenGLRenderer()
{
	Terminate();
}

void OpenGLRenderer::Init(Platform::Window* pWindow)
{
	assert(mpWindow != nullptr);

	// NOTE! adjust view size as the surfa capabilities differ from the native window size
	//auto surfCapabilities = mpDevice->GetSurfaceCapabilities();

	//mWindowWidth = surfCapabilities.currentExtent.width;
	//mWindowHeight = surfCapabilities.currentExtent.height;

	// already done in Renderer base class
	//mpWindow->GetWindowSize(&mWindowWidth, &mWindowHeight);

	Prepare();
}

void OpenGLRenderer::Terminate()
{
	Renderer::Terminate();

#ifdef PIPELINE_STATS
	for (auto& it : mPipelineStatsMap)
	{
		auto& psBuff = it.second;

		glDeleteQueries(psBuff.queryIds.size(), psBuff.queryIds.data());

		psBuff.queryIds.clear();
		psBuff.queryNames.clear();
	}
	mPipelineStatsMap.clear();
#endif

	for (auto& it : mVisualPassMap)
	{
		auto& rpBuff = it.second;

		GE_FREE(rpBuff.pFrameBuffer);
	}
	mVisualPassMap.clear();

}


void OpenGLRenderer::Prepare()
{
	assert(mpWindow != nullptr);

	mpWindow->GLContextMakeCurrent();

	OpenGLHelpers::ListRendererInfo();
	OpenGLHelpers::ListSupportedExtensions();
	OpenGLHelpers::ListCapabilities();

	// NOTE! For GL state setup see OpenGLVisualPass::SetupPipeline()

	mIsPrepared = true;
}

void OpenGLRenderer::OnWindowResize(uint32_t width, uint32_t height)
{
	if (false == mIsPrepared)
		return;

	mIsPrepared = false;

	if (width > 0)
		mWindowWidth = width;

	if (height > 0)
		mWindowHeight = height;

	assert(mpWindow != nullptr);

	mpWindow->GLContextMakeCurrent();

	glViewport(0, 0, mWindowWidth, mWindowHeight);

	// TODO - update projection matrix !

	//TODO - re-init framebuffers !!!

	mIsPrepared = true;
}

void OpenGLRenderer::RenderFrame(RenderQueue* pRenderQueue)
{
	if (false == mIsPrepared)
		return;

	assert(pRenderQueue != nullptr);

	BeginFrame();

	DrawSceneToBackBuffer();

	EndFrame();
}

void OpenGLRenderer::UpdateFrame(Camera* pCamera, float32_t crrTime)
{
	if (false == mIsPrepared)
		return;

	assert(mpRenderQueue != nullptr);

	assert(pCamera != nullptr);
	mpCamera = pCamera;

	UpdateNodes(pCamera, crrTime);
}

void OpenGLRenderer::BeginFrame()
{
	//TODO
}

void OpenGLRenderer::EndFrame()
{
	// TODO
}

void OpenGLRenderer::SubmitFrame()
{
	if (false == mIsPrepared)
		return;

	assert(mpWindow != nullptr);

	// GL context is tightly coupled with the window
	// hence the window instance is used to swap the back and front buffers!
	mpWindow->GLSwapBuffers();
}

void OpenGLRenderer::ComputeGraphicsResources(RenderQueue* pRenderQueue)
{
	if (false == mIsPrepared)
		return;

	assert(pRenderQueue != nullptr);

	mpRenderQueue = pRenderQueue;

	// TODO - compute resources for all renderable types
	auto& renderableList = mpRenderQueue->GetRenderables(RenderQueue::RenderableType::GE_RT_OPAQUE);

	mpRenderQueue->ForEach(renderableList,
		[&, this](const RenderQueue::Renderable* pRenderable)
		{
			assert(pRenderable != nullptr);

			auto* pGeoNode = pRenderable->pGeometryNode;
			assert(pGeoNode != nullptr);

			auto* pVisComp = pGeoNode->GetComponent<VisualComponent>();
			assert(pVisComp != nullptr);
			auto* pVisEffect = pVisComp->GetVisualEffect();
			assert(pVisEffect != nullptr);

			// we didn't call this earlier as we needed to have
			// the node info first
			pVisEffect->Init();

			// TODO - Compute mVisualPasses list of passes in their proper order
			const auto& passMap = pVisEffect->GetPasses();
			for (auto& it : passMap)
			{
				auto& passVector = it.second;
				for (auto* pPass : passVector)
				{
					AddVisualPass(pPass);
				}
			}

			pVisEffect->InitPasses(this);
		}
	);

	SetupPipelineStats();
}

void OpenGLRenderer::UpdateUniformBuffers(VisualPass* pVisualPass, GeometryNode* pGeoNode, Camera* pCamera, float32_t crrTime)
{
	// NOTE! When passing uniforms with shaders the usual way is to use API like glUniform*() to pass ints, floats, matrices.
	// In our case for GL we use a similar approach as we did for the Vulkan renderer
	// we map the uniform buffer data into GPU's address space: RAM -> VRAM
	// this is both fast and avoids lots of glUniform*() calls.

	assert(pVisualPass != nullptr);
	assert(pGeoNode != nullptr);
	assert(pCamera != nullptr);

	auto shaders = pVisualPass->GetShaders();
	for (auto iter = shaders.begin(); iter != shaders.end(); ++iter)
	{
		auto shaderStage = iter->first;
		auto* pUniformBuffer = pVisualPass->GetUniformBuffer(shaderStage);

		if (pUniformBuffer)
		{
			const auto& uniforms = pUniformBuffer->GetUniforms();

			for (const auto& uni : uniforms)
			{
				const auto& uniformType = uni.first;

				switch (uniformType)
				{
				case GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4:
				{
					pUniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4,
						pCamera->GetProjectionViewMatrix() * pVisualPass->GetTransform() * pGeoNode->GetModelMatrix());
				} break;
				case GLSLShaderTypes::UniformType::GE_UT_PV_CUBEMAP_MATRIX4:
				{
					glm::mat4 PV = pCamera->GetProjectionViewMatrix();

					// we remove the translation transform to allow the cubemap to never change position in world space
					PV[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

					pUniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4, PV * pGeoNode->GetModelMatrix());
				} break;
				case GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX4:
				{
					pUniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX4, pVisualPass->GetTransform() * pGeoNode->GetModelMatrix());
				} break;
				case GLSLShaderTypes::UniformType::GE_UT_NORMAL_MATRIX4:
				{
					pUniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_NORMAL_MATRIX4, pVisualPass->GetTransform() * pGeoNode->GetNormalMatrix());
				} break;
				case GLSLShaderTypes::UniformType::GE_UT_CAMERA_POS:
				{
					pUniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_CAMERA_POS, glm::vec4(pCamera->GetPosition(), 0.0f));
				} break;
				case GLSLShaderTypes::UniformType::GE_UT_PROJECTION_MATRIX4:
				case GLSLShaderTypes::UniformType::GE_UT_VIEW_MATRIX4:
				case GLSLShaderTypes::UniformType::GE_UT_COLOR_VEC4:
				case GLSLShaderTypes::UniformType::GE_UT_LIGHT_DIR:
				case GLSLShaderTypes::UniformType::GE_UT_LIGHT_COLOR:
				case GLSLShaderTypes::UniformType::GE_UT_CRR_TIME:
					// TODO
					break;
				case GLSLShaderTypes::UniformType::GE_UT_COUNT:
				default:
					//LOG_ERROR("Invalid uniform type! Failed to update the uniform!");
					break;
				}
			}

			Bind(pUniformBuffer);
		}
	}
}

void OpenGLRenderer::BindLight(VisualPass* pVisualPass, const LightNode* pLightNode, GeometryNode* pGeoNode)
{
	assert(pVisualPass != nullptr);
	assert(pLightNode != nullptr);
	assert(pGeoNode != nullptr);

	if (false == pGeoNode->IsLit())
	{
		LOG_INFO("No light to bind for this node as it is unlit!");
		return;
	}

	auto* pLight = pLightNode->GetLight();
	assert(pLight != nullptr);

	switch (pVisualPass->GetPassType())
	{
	case VisualPass::PassType::GE_PT_STANDARD:
	case VisualPass::PassType::GE_PT_OFFSCREEN:
	{
		auto* pVertUBO = pVisualPass->GetUniformBuffer(Shader::ShaderStage::GE_SS_VERTEX);
		auto* pFragUBO = pVisualPass->GetUniformBuffer(Shader::ShaderStage::GE_SS_FRAGMENT);

		switch (pLight->GetLightType())
		{
		case Light::LightType::GE_LT_DIRECTIONAL:
		{
			if (pFragUBO)
			{
				pFragUBO->SetUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_DIR, glm::vec4(pLight->GetDirection(), 0.0f));
				pFragUBO->SetUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_COLOR, glm::vec4(pLight->GetColor(), 1.0f));
			}
		}
		break;
		case Light::LightType::GE_LT_POINT:
		{
			if (pGeoNode->IsPassAllowed(VisualPass::PassType::GE_PT_SHADOWS))
			{
				if (pVertUBO)
					pVertUBO->SetUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_PVM_MATRIX4, pLight->GetLightPVM());
			}

			if (pFragUBO)
			{
				pFragUBO->SetUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_POS, glm::vec4(pLight->GetPosition(), 1.0f));
				pFragUBO->SetUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_COLOR, glm::vec4(pLight->GetColor(), 1.0f));
			}
		} break;
		case Light::LightType::GE_LT_SPOT:
			//TODO
			break;
		case Light::LightType::GE_PT_COUNT:
		default:
			LOG_ERROR("Invalid light type!");
		}
	} break;
	case VisualPass::PassType::GE_PT_SHADOWS:
	{
		auto* pUBO = pVisualPass->GetUniformBuffer(Shader::ShaderStage::GE_SS_VERTEX);

		switch (pLight->GetLightType())
		{
		case Light::LightType::GE_LT_POINT:
		{
			if (pUBO)
				pUBO->SetUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_PVM_MATRIX4, pLight->GetLightPVM());
		} break;
		case Light::LightType::GE_LT_DIRECTIONAL:
		case Light::LightType::GE_LT_SPOT:
			//TODO
			break;
		case Light::LightType::GE_PT_COUNT:
		default:
			LOG_ERROR("Invalid light type!");
		}
	} break;
	}
}

void OpenGLRenderer::DrawSceneToBackBuffer()
{
	assert(mpRenderQueue != nullptr);

	// THE ACTUAL RENDERING IS DONE HERE !!!!

	DrawNodes(0);
}

void OpenGLRenderer::DrawNodes(uint32_t currentBufferIdx)
{
	for (auto& it : mVisualPassMap)
	{
		auto& passType = it.first;
		auto& passData = it.second;

		//
		BeginQuery(passType, currentBufferIdx);
		//

		BeginRenderPass(passData, currentBufferIdx);

		for (auto* pPass : passData.passes)
		{
			if (pPass)
			{
				pPass->RenderNode(currentBufferIdx);
			}
		}

		EndRenderPass(passData, currentBufferIdx);

		//
		EndQuery(passType, currentBufferIdx);
		//
	}
}


void OpenGLRenderer::UpdateNodes(Camera* pCamera, float32_t crrTime)
{
	assert(pCamera != nullptr);

	for (auto& it : mVisualPassMap)
	{
		auto& passType = it.first;
		auto& passData = it.second;

		for (auto* pPass : passData.passes)
		{
			if (pPass)
			{
				pPass->UpdateNode(pCamera, crrTime);
			}
		}
	}

	GetQueryResults();
}

void OpenGLRenderer::DrawNode(VisualPass* pVisualPass, GeometryNode* pGeoNode, uint32_t currentBufferIdx)
{
	assert(pVisualPass != nullptr);
	assert(pGeoNode != nullptr);

	//	UpdateDynamicStates(pVisualPass, currentBufferIdx);

		// debug case
	if (pVisualPass->GetIsDebug())
	{
		DrawDirect(3, 0);
		return;
	}

	// draw geometric primitives
	auto* pGeometry = pGeoNode->GetGeometry();
	assert(pGeometry != nullptr);

	// bind vertex buffer
	auto* pVertexBuffer = pGeometry->GetVertexBuffer();
	assert(pVertexBuffer != nullptr);

	//// bind index buffer (if available)
	bool isIndexedDrawing = pGeometry->IsIndexed();
	auto* pIndexBuffer = pGeometry->GetIndexBuffer();
	if (nullptr == pIndexBuffer)
	{
		isIndexedDrawing = false; // failsafe
	}

	uint32_t count = 0;
	if (isIndexedDrawing)
	{
		if (pIndexBuffer)
			count = pIndexBuffer->GetIndexCount();
	}
	else
	{
		count = pVertexBuffer->GetVertexCount();
	}

	auto* pVertexFormat = pGeometry->GetVertexFormat();
	assert(pVertexFormat != nullptr);

	uint32_t instanceCount = (pVertexFormat->GetVertexInputRate() == VertexFormat::VertexInputRate::GE_VIR_VERTEX ? 1 : 0); //TODO

	if (isIndexedDrawing)
	{
		// in case of model loading
		if (pGeometry->IsModel())
		{
			Model* pModel = dynamic_cast<Model*>(pGeometry);
			if (pModel)
			{
				auto* gadrModel = Get(pModel);
				assert(gadrModel != nullptr);

				gadrModel->Draw([this, &pIndexBuffer](uint32_t indexCount, uint32_t firstIndex)
					{
						DrawDirect(indexCount, firstIndex, pIndexBuffer);
					});
			}
		}
		else
		{
			DrawDirect(count, 0, pIndexBuffer);
		}
	}
	else
	{
		DrawDirect(count, 0);
	}
}

void OpenGLRenderer::UpdateNode(VisualPass* pVisualPass, GeometryNode* pGeoNode, Camera* pCamera, float32_t crrTime)
{
	assert(pVisualPass != nullptr);
	assert(pGeoNode != nullptr);
	assert(pCamera != nullptr);

	UpdateUniformBuffers(pVisualPass, pGeoNode, pCamera, crrTime);

	//TODO - other stuff to update
}

void OpenGLRenderer::DrawDirect(uint32_t count, uint32_t first, IndexBuffer* pIndexBuffer)
{
	if (pIndexBuffer)
	{
		GLenum glIndexType = OpenGLUtils::IndexTypeToOpenGLIndexType(pIndexBuffer->GetIndexType());
		uint32_t sizeofBytes = OpenGLUtils::IndexTypeToSizeofBytes(pIndexBuffer->GetIndexType());

		glDrawElements(GL_TRIANGLES, count, glIndexType, (void*)(first * sizeofBytes));
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, first, count);
	}
}

//void OpenGLRenderer::DrawIndirect(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t currentBufferIdx, bool_t isIndexedDrawing)
//{
//	// TODO - indirect draw
//}

void OpenGLRenderer::BeginRenderPass(const VisualPassData& visualPassData, uint32_t currentBufferIdx)
{
	auto passType = visualPassData.passes[0]->GetPassType();
	if ((passType == VisualPass::PassType::GE_PT_OFFSCREEN || passType == VisualPass::PassType::GE_PT_SHADOWS) &&
		visualPassData.pFrameBuffer)
	{
		visualPassData.pFrameBuffer->Bind();
	}


	auto& dataRef = visualPassData.passBeginData;

	// TODO - for now the render area size is the size of: screen/framebuffer/render target
	glViewport(0, 0, dataRef.width, dataRef.height);


	if (visualPassData.passes[0]->GetPassType() == VisualPass::PassType::GE_PT_SHADOWS)
	{
		glDepthRangef(0.0f, dataRef.depth);

		glClearDepthf(dataRef.depth);

		glClear(GL_DEPTH_BUFFER_BIT);
	}
	else // standard or offscreen
	{
		//TODO - allow custom clear color
		if (visualPassData.passes[0]->GetPassType() == VisualPass::PassType::GE_PT_STANDARD)
		{
			glClearColor(0.5, 0.5, 0.5, 1.0);
		}
		else
		{
			glClearColor(dataRef.clearColor.r, dataRef.clearColor.g, dataRef.clearColor.b, dataRef.clearColor.a);
		}

		glDepthRangef(0.0f, dataRef.depth);

		glClearDepthf(dataRef.depth);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

}

void OpenGLRenderer::EndRenderPass(const VisualPassData& visualPassData, uint32_t currentBufferIdx)
{
	auto passType = visualPassData.passes[0]->GetPassType();
	if ((passType == VisualPass::PassType::GE_PT_OFFSCREEN || passType == VisualPass::PassType::GE_PT_SHADOWS) &&
		visualPassData.pFrameBuffer)
	{
		visualPassData.pFrameBuffer->UnBind();
	}
}

void OpenGLRenderer::SetupFrameBuffers(VisualPass* pVisualPass, OpenGLFrameBuffer*& pFrameBufferOut, VisualPassBeginData& visualPassBeginDataOut)
{
	assert(pVisualPass != nullptr);

	//NOTE! Currently Depth attachment is supported by default

	switch (pVisualPass->GetPassType())
	{
	case VisualPass::PassType::GE_PT_STANDARD:
	{
		// NOTE! For the Standard Render pass there is no explicit framebuffer setup
		// as OpenGL has a default farmebuffer called Backbuffer to render to !!!

		assert(GetWindowWidth() > 0);
		assert(GetWindowHeight() > 0);

		visualPassBeginDataOut.width = GetWindowWidth();
		visualPassBeginDataOut.height = GetWindowHeight();
		visualPassBeginDataOut.depth = 1.0f;
		visualPassBeginDataOut.stencil = 0;
		//TODO - clear color

		// No framebuffer setup - using Backbuffer

	} break;
	case VisualPass::PassType::GE_PT_OFFSCREEN:
	{
		auto& renderTargets = pVisualPass->GetRenderTargets();

		assert(renderTargets.size() == ATT_COUNT);

		auto colorIt = renderTargets.find(RenderTarget::TargetType::GE_TT_COLOR);
		assert(colorIt != renderTargets.end());
		auto* pColorRT = Get(colorIt->second);
		assert(pColorRT != nullptr);

		auto depthIt = renderTargets.find(RenderTarget::TargetType::GE_TT_DEPTH_STENCIL);
		assert(depthIt != renderTargets.end());
		auto* pDepthRT = Get(depthIt->second);
		assert(pDepthRT != nullptr);

		// prepare color + depth RTs as attachments for the framebuffer
		assert(pColorRT->GetGLTextureObject() != nullptr);
		assert(pDepthRT->GetGLTextureObject() != nullptr);

		// get RT width and height for the framebuffer size
		const auto& extent3D = pColorRT->GetGLTextureObject()->GetData().extent;

		assert(extent3D.width > 0);
		assert(extent3D.height > 0);

		visualPassBeginDataOut.width = extent3D.width;
		visualPassBeginDataOut.height = extent3D.height;
		visualPassBeginDataOut.depth = 1.0f;
		visualPassBeginDataOut.stencil = 0;
		//TODO - clear color
		
		pFrameBufferOut = GE_ALLOC(OpenGLFrameBuffer);
		assert(pFrameBufferOut != nullptr);

		pFrameBufferOut->Add2DTextureAttachment(GL_COLOR_ATTACHMENT0, pColorRT->GetGLTextureObject()->GetHandle());
		pFrameBufferOut->Add2DTextureAttachment(GL_DEPTH_ATTACHMENT, pDepthRT->GetGLTextureObject()->GetHandle());

		pFrameBufferOut->CheckCompleteness();
	} break;
	case VisualPass::PassType::GE_PT_SHADOWS:
	{
		auto& renderTargets = pVisualPass->GetRenderTargets();

		assert(renderTargets.size() == 1);

		auto depthIt = renderTargets.find(RenderTarget::TargetType::GE_TT_DEPTH);
		assert(depthIt != renderTargets.end());
		auto* pDepthRT = Get(depthIt->second);
		assert(pDepthRT != nullptr);

		// prepare color + depth RTs as attachments for the framebuffer
		assert(pDepthRT->GetGLTextureObject() != nullptr);

		// get RT width and height for the framebuffer size
		const auto& extent3D = pDepthRT->GetGLTextureObject()->GetData().extent;

		assert(extent3D.width > 0);
		assert(extent3D.height > 0);

		visualPassBeginDataOut.width = extent3D.width;
		visualPassBeginDataOut.height = extent3D.height;
		visualPassBeginDataOut.depth = 1.0f;
		visualPassBeginDataOut.stencil = 0;
		//TODO - clear color

		pFrameBufferOut = GE_ALLOC(OpenGLFrameBuffer);
		assert(pFrameBufferOut != nullptr);

		pFrameBufferOut->Add2DTextureAttachment(GL_DEPTH_ATTACHMENT, pDepthRT->GetGLTextureObject()->GetHandle());

		pFrameBufferOut->CheckCompleteness();
	} break;
	case VisualPass::PassType::GE_PT_COUNT:
	default:
		LOG_ERROR("Invalid visual pass type!");
		return;
	}
}

void OpenGLRenderer::AddVisualPass(VisualPass* pVisualPass)
{
	assert(pVisualPass != nullptr);

	auto* pGeoNode = pVisualPass->GetNode();
	assert(pGeoNode != nullptr);

	if (pGeoNode->IsPassAllowed(pVisualPass->GetPassType()))
	{
		auto it = mVisualPassMap.find(pVisualPass->GetPassType());
		if (it != mVisualPassMap.end())
		{
			bool foundNode = false;
			auto& passData = it->second;
			for (auto* pPass : passData.passes)
			{
				if (pPass && pPass->GetNode() == pGeoNode)
				{
					foundNode = true;
					break;
				}
			}

			if (false == foundNode)
			{
				passData.passes.push_back(pVisualPass);
			}
		}
		else
		{
			OpenGLFrameBuffer* pFrameBuffer = nullptr;
			VisualPassBeginData passBeginData;

			SetupFrameBuffers(pVisualPass, pFrameBuffer, passBeginData);

			VisualPassData passData;
			passData.passBeginData = passBeginData;
			passData.pFrameBuffer = pFrameBuffer;

			passData.passes.push_back(pVisualPass);

			mVisualPassMap.emplace(pVisualPass->GetPassType(), passData);
		}
	}
};

void OpenGLRenderer::SetupPipelineStats()
{
#ifdef PIPELINE_STATS
	if (!mPipelineStatsMap.empty())
		return;

	assert(false == mVisualPassMap.empty());

	// OpenGL 4.6
	PipelineStatsData pipelineStatsData;
	pipelineStatsData.queryTargets = { 
									 	GL_VERTICES_SUBMITTED,
									 	GL_PRIMITIVES_SUBMITTED,
									 	GL_VERTEX_SHADER_INVOCATIONS,
									 	GL_TESS_CONTROL_SHADER_PATCHES,
									 	GL_TESS_EVALUATION_SHADER_INVOCATIONS,
									 	GL_GEOMETRY_SHADER_INVOCATIONS,
									 	GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED,
									 	GL_COMPUTE_SHADER_INVOCATIONS,
									 	GL_CLIPPING_INPUT_PRIMITIVES,
									 	GL_CLIPPING_OUTPUT_PRIMITIVES,
									 	GL_FRAGMENT_SHADER_INVOCATIONS
									 };

	pipelineStatsData.queryNames = {
			"Input assembly vertex count        ",
			"Input assembly primitives count    ",
			"Vertex shader invocations          ",
			"Tesselation control patches shader invocations          ",
			"Tesselation evaluation shader invocations          ",
			"Geometry shader invocations          ",
			"Geometry shader primitives output          ",
			"Compute shader invocations",
			"Clipping stage primitives processed",
			"Clipping stage primtives output    ",
			"Fragment shader invocations        "
	};

	assert(pipelineStatsData.queryTargets.size() == pipelineStatsData.queryNames.size());

	pipelineStatsData.queryIds.resize(pipelineStatsData.queryNames.size());
	pipelineStatsData.queryTargets.resize(pipelineStatsData.queryNames.size());

	for (auto& it : mVisualPassMap)
	{
		auto& passType = it.first;

		for (size_t idx = 0; idx < pipelineStatsData.queryTargets.size(); idx ++)
		{
			GLuint queryId = 0;
			glCreateQueries(pipelineStatsData.queryTargets[idx], 1, &queryId);

			pipelineStatsData.queryIds[idx] = queryId;
		}

		mPipelineStatsMap.emplace(passType, pipelineStatsData);
	}


#endif
}

// Retrieves the results of the pipeline statistics query submitted to the command buffer
void OpenGLRenderer::GetQueryResults()
{
#ifdef PIPELINE_STATS
	assert(false == mPipelineStatsMap.empty());

	for (auto& it : mPipelineStatsMap)
	{
		auto& passType = it.first;
		auto& queryData = it.second;

		std::string passTypeStr = (passType == VisualPass::PassType::GE_PT_SHADOWS ? "SHADOW" : (passType == VisualPass::PassType::GE_PT_OFFSCREEN ? "OFFSCREEN" : "STANDARD"));
		LOG_INFO("//////////////// Render pass: %s ///////////////////////", passTypeStr.c_str());

		for (size_t idx = 0; idx < queryData.queryIds.size(); ++ idx)
		{
			const auto& queryId = queryData.queryIds[idx];
			const auto& queryName = queryData.queryNames[idx];
			
			uint16_t counter = 1000; /* counter to avoid a possible infinite loop */
			GLint queryReady = 0;
			do
			{
				glGetQueryObjectiv(queryId, GL_QUERY_RESULT_AVAILABLE, &queryReady);
			} while (! queryReady && counter--);

			if (queryReady)
			{
				GLint stat = 0;
				glGetQueryObjectiv(queryId, GL_QUERY_RESULT, &stat);
				
				LOG_INFO("Pipeline Stat %s : %lu", queryName.c_str(), stat);
			}
			else
			{
				LOG_INFO(" Result not ready ... rendering anyway");
			}
		}
	}
#endif
}

void OpenGLRenderer::ResetQuery(VisualPass::PassType passType, uint32_t currentBufferIdx)
{
#ifdef PIPELINE_STATS
	//
#endif
}

void OpenGLRenderer::BeginQuery(VisualPass::PassType passType, uint32_t currentBufferIdx)
{
#ifdef PIPELINE_STATS
	// before every use and in between uses the query pool must be reset
	//ResetQuery(passType, currentBufferIdx);

	assert(false == mPipelineStatsMap.empty());

	auto it = mPipelineStatsMap.find(passType);
	if (it != mPipelineStatsMap.end())
	{
		auto& passType = it->first;
		auto& queryData = it->second;

		for (size_t idx = 0; idx < queryData.queryIds.size(); ++idx)
		{
			const auto& queryTarget = queryData.queryTargets[idx];
			const auto& queryId = queryData.queryIds[idx];

			glBeginQuery(queryTarget, queryId);
		}
	}
#endif
}

void OpenGLRenderer::EndQuery(VisualPass::PassType passType, uint32_t currentBufferIdx)
{
#ifdef PIPELINE_STATS
	assert(false == mPipelineStatsMap.empty());

	auto it = mPipelineStatsMap.find(passType);
	if (it != mPipelineStatsMap.end())
	{
		auto& passType = it->first;
		auto& queryData = it->second;

		for (size_t idx = 0; idx < queryData.queryIds.size(); ++idx)
		{
			const auto& queryTarget = queryData.queryTargets[idx];

			glEndQuery(queryTarget);
		}
	}
#endif
}
#endif // OPENGL_RENDERER