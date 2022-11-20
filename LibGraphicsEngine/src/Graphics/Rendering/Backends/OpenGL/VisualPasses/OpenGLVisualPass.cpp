#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/VisualPasses/OpenGLVisualPass.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLCommon.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLUtils.hpp"
#include "Graphics/Rendering/Backends/OpenGL/OpenGLRenderer.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLBuffer.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLVertexArrayObject.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLShaderProgram.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLShaderObject.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLVertexBuffer.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLVertexFormat.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLShader.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLTexture.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLUniformBuffer.hpp"
#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"
#include "Graphics/Rendering/Resources/RenderTarget.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderParser.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

using namespace std::placeholders;  // for _1, _2, _3...

int GADVisualPass::msGlobalTexUnit = -1; // at first usage it will be incremeneted by 1 and become = 0 - first valid tex unit

GADVisualPass::GADVisualPass()
	: mpOpenGLRenderer(nullptr)
	, mpOpenGLVertexArrayObject(nullptr)
	, mpOpenGLShaderProgram(nullptr)
	, mpVisualPass(nullptr)
	, mIsPresentPass(false)
{}

GADVisualPass::GADVisualPass(Renderer* pRenderer, VisualPass* pVisualPass)
	: GADVisualPass()
{
	Init(pRenderer, pVisualPass);
}

GADVisualPass::~GADVisualPass()
{
	GE_FREE(mpOpenGLVertexArrayObject);
	GE_FREE(mpOpenGLShaderProgram);

	mpOpenGLRenderer = nullptr;
}

void GADVisualPass::Init(Renderer* pRenderer, VisualPass* pVisualPass)
{
	assert(pRenderer != nullptr);
	assert(pVisualPass != nullptr);

	mpOpenGLRenderer = dynamic_cast<OpenGLRenderer*>(pRenderer);
	assert(mpOpenGLRenderer != nullptr);

	mpOpenGLVertexArrayObject = GE_ALLOC(OpenGLVertexArrayObject);
	assert(mpOpenGLVertexArrayObject != nullptr);

	mpOpenGLShaderProgram = GE_ALLOC(OpenGLShaderProgram);
	assert(mpOpenGLShaderProgram != nullptr);

	mpVisualPass = pVisualPass;

	mIsPresentPass = (mpVisualPass->GetPassType() == VisualPass::PassType::GE_PT_STANDARD);
	
	SetupPipeline();


	if (mpVisualPass->HasTextures())
	{
		// detect how many texture units to use 
		// depending on the number of textures to be applied at the same time
		mStartTextUnit = msGlobalTexUnit + 1;

		auto& textureMap = mpVisualPass->GetTextures();

		const auto& shaders = mpVisualPass->GetShaders();
		for (auto iter = shaders.begin(); iter != shaders.end(); ++iter)
		{
			auto shaderStage = iter->first;
			auto* pShader = iter->second;
			assert(pShader != nullptr);

			auto it = textureMap.find(pShader->GetShaderStage());
			if (it != textureMap.end())
			{
				auto& textureArray = it->second;

				msGlobalTexUnit += textureArray.size();
			}
		}
	}
}

void GADVisualPass::SetupPipeline()
{
	assert(mpOpenGLRenderer != nullptr);
	assert(mpVisualPass != nullptr);

	////  Shaders state
	SetupShaderStage();

	//// Vertex input state
	SetupVertexInputState();

	//// Input assembly state describes how primitives are assembled
	//// This pipeline will assemble vertex data as a triangle lists
	SetupPrimitiveAssemblyState();

	//// Viewport state sets the number of viewports and scissor used in this pipeline
	//// Note: This is actually overriden by the dynamic states (see below)
//	SetupViewportScissorState();

	// Enable dynamic states
	// Most states are baked into the pipeline, but there are still a few dynamic states that can be changed within a command buffer
	// To be able to change these we need do specify which dynamic states will be changed using this pipeline. Their actual states are set later on in the command buffer.
	// For this example we will set the viewport and scissor using dynamic states
//	SetupDynamicState();

}

void GADVisualPass::SetupShaderStage()
{
	assert(mpOpenGLRenderer != nullptr);
	assert(mpVisualPass != nullptr);
	assert(mpOpenGLShaderProgram != nullptr);

	// setup Shaders
	const auto& shaders = mpVisualPass->GetShaders();

	size_t i = 0;
	for (auto iter = shaders.begin(); iter != shaders.end(); ++iter)
	{
		auto shaderStage = iter->first;
		auto* pShader = iter->second;
		assert(pShader != nullptr);

		auto GADRShader = mpOpenGLRenderer->Get(pShader);
		assert(GADRShader != nullptr);

		auto refGLShader = GADRShader->GetGLShaderObject();
		assert(refGLShader != nullptr);

		if (refGLShader->Compile())
		{
			mpOpenGLShaderProgram->AttachShader(refGLShader->GetHandle());
		}
		else
		{
			LOG_ERROR("Shader %s failed compilation! Abort!", pShader->GetSourcePath().c_str());
			return;
		}
	}

	if (mpOpenGLShaderProgram->Link())
	{
		// cleanup
		for (auto iter = shaders.begin(); iter != shaders.end(); ++iter)
		{
			auto* pShader = iter->second;
			assert(pShader != nullptr);

			auto GADRShader = mpOpenGLRenderer->Get(pShader);
			assert(GADRShader != nullptr);

			auto refGLShader = GADRShader->GetGLShaderObject();
			assert(refGLShader != nullptr);

			mpOpenGLShaderProgram->DetachShader(refGLShader->GetHandle());
		}
	}
}

void GADVisualPass::SetupVertexInputState()
{
	// NOTE! We need a geometry independent vertex format !!!
	// https://vulkan.lunarg.com/doc/view/1.2.148.0/mac/chunked_spec/chap22.html#fxvertex-example

	assert(mpOpenGLRenderer != nullptr);
	assert(mpVisualPass != nullptr);
	assert(mpOpenGLVertexArrayObject != nullptr);

	//if (mpVisualPass->GetIsDebug())
	//{
	//	//
	//}

	auto* pGeoNode = mpVisualPass->GetNode();
	assert(pGeoNode != nullptr);

	auto* pGeometry = pGeoNode->GetGeometry();
	assert(pGeometry != nullptr);

	auto* pVertexFormat = pGeometry->GetVertexFormat();
	assert(pVertexFormat != nullptr);

	GADRVertexFormat* gadrVertexFormat = mpOpenGLRenderer->Get(pVertexFormat);
	assert(gadrVertexFormat != nullptr);

	Shader* pVertexShader = mpVisualPass->GetShader(Shader::ShaderStage::GE_SS_VERTEX);
	assert(pVertexShader != nullptr);

	GLSLShaderParser* pGlslParser = pVertexShader->GetGLSLParser();
	assert(pGlslParser != nullptr);

	auto shaderAttribs = pGlslParser->GetVertexAttributes();

	// update attribute locations based on the current used vertex shader
	auto inputAttribs = gadrVertexFormat->GetInputAttributes();

	// the shader can use a smaller number of attributes compared to input geometry data
	assert(shaderAttribs.size() <= inputAttribs.size());


	//
	mpOpenGLVertexArrayObject->Bind();

	// bind vertex buffer
	auto* pVertexBuffer = pGeometry->GetVertexBuffer();
	assert(pVertexBuffer != nullptr);

	mpOpenGLRenderer->Bind(pVertexBuffer, 0);

	// vertex input setup - tied to Vertex buffer bind
	auto inputAttIter = inputAttribs.begin();
	for (auto shaderAttIter = shaderAttribs.begin(); shaderAttIter != shaderAttribs.end(); ++shaderAttIter, ++inputAttIter)
	{
		auto& refAtt = inputAttIter->second;
		auto refInput = shaderAttIter->second.pInput;

		//TODO - see if to use VertexData struct with all attributes packed or separately
		glVertexAttribPointer(refInput->location, refAtt.size, refAtt.type, GL_FALSE, refAtt.stride, (void*)refAtt.offset);
		glEnableVertexAttribArray(refInput->location);
	}

	// bind index buffer (if available)
	bool isIndexedDrawing = pGeometry->IsIndexed();
	auto* pIndexBuffer = pGeometry->GetIndexBuffer();
	if (nullptr == pIndexBuffer)
	{
		isIndexedDrawing = false; // failsafe
	}

	if (isIndexedDrawing)
	{
		mpOpenGLRenderer->Bind(pIndexBuffer, 0);
	}

	mpOpenGLVertexArrayObject->UnBind();

	mpOpenGLRenderer->UnBind(pVertexBuffer);
	if (isIndexedDrawing)
	{
		mpOpenGLRenderer->UnBind(pIndexBuffer);
	}
}

void GADVisualPass::SetupPrimitiveAssemblyState()
{
	assert(mpVisualPass != nullptr);

	// Input assembly state describes how primitives are assembled
	// This pipeline will assemble vertex data as a triangle lists

	// During rendering we use GL_TRIANGLES drawing mode!
}

void GADVisualPass::SetupViewportScissorState()
{
	assert(mpOpenGLRenderer != nullptr);
	assert(mpVisualPass != nullptr);

	// Viewport
	glViewport(0, 0, mpOpenGLRenderer->GetWindowWidth(), mpOpenGLRenderer->GetWindowHeight());

	// NOTE! no need to set scisors at this point!
}

void GADVisualPass::SetupRasterizationState()
{
	assert(mpVisualPass != nullptr);

	// Rasterization state

	GLenum glPolygonMode_ = OpenGLUtils::PrimitivePolygonModeToOpenGLPolygonMode(mpVisualPass->GetPolygonMode());

	GLenum glFaceWinding = OpenGLUtils::PrimitiveFaceWindingToOpenGLFaceWinding(mpVisualPass->GetFaceWinding());

	GLenum glCullMode = OpenGLUtils::FaceCullModeToOpenGLFaceCullMode(mpVisualPass->GetCullFaceState().GetCullMode());

	if (mpVisualPass->GetIsDebug())
	{
		glCullMode = GL_NONE;
	}

	// Cull Face
	glCullFace(glCullMode);
	glEnable(GL_CULL_FACE);

	// Facing
	glFrontFace(glFaceWinding);

	// Point Size - TODO
	//glPointSize(1.0f);
	//glEnable(GL_PROGRAM_POINT_SIZE);

	// Polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, glPolygonMode_);

	if (mpVisualPass->GetPassType() == VisualPass::PassType::GE_PT_SHADOWS)
	{
	//	GLenum glPolygonOffset_ = OpenGLUtils::PrimitivePolygonModeToOpenGLPolygonOffset(mpVisualPass->GetPolygonMode());

	//	glPolygonOffset(slopeScaleDepthBias, depthBias);
	//	glEnable(glPolygonOffset_);
	}

}

void GADVisualPass::SetupMultisampleState()
{
	//TODO

	// Multi sampling state
	

	// Anti-Aliasing Sample Mode
	//glEnable(GL_MULTISAMPLE); //anti-aliasing

//	glDisable(GL_MULTISAMPLE);
}

void GADVisualPass::SetupDepthStencilState()
{
	assert(mpVisualPass != nullptr);

	// Depth and stencil state containing depth and stencil compare and test operations
	// We only use depth tests and want depth tests and writes to be enabled and compare with less or equal
	auto& depthStencilState = mpVisualPass->GetDepthStencilState();

	auto depthEnabled = depthStencilState.GetIsDepthEnabled();
	auto depthWritable = depthStencilState.GetIsDepthWritable();
	auto glDepthCompareOp = OpenGLUtils::CompareOpToOpenGLCompareOp(depthStencilState.GetDepthCompareOp());

	if (depthEnabled)
	{
		glDepthFunc(glDepthCompareOp);

		glDepthMask(depthWritable);
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	auto stencilEnabled = depthStencilState.GetIsStencilEnabled();
	if (stencilEnabled)
	{
		GLenum glFailOp = OpenGLUtils::StencilOpToOpenGLStencilOp(depthStencilState.GetStencilFailOp());
		GLenum glPassOp = OpenGLUtils::StencilOpToOpenGLStencilOp(depthStencilState.GetStencilPassDepthPassOp());
		GLenum glDepthFailOp = OpenGLUtils::StencilOpToOpenGLStencilOp(depthStencilState.GetStencilPassDepthFailOp());
		GLenum glCompareOp = OpenGLUtils::CompareOpToOpenGLCompareOp(depthStencilState.GetStencilCompareOp());
	//	stencilOpState.compareMask = depthStencilState.GetStencilCompareMask();
	//	stencilOpState.writeMask = depthStencilState.GetStencilWriteMask();
	//	stencilOpState.reference = depthStencilState.GetStencilReference();

		// Stencil
		glStencilOp(glFailOp, glDepthFailOp, glPassOp);
		glStencilFunc(glCompareOp, 0, 0xFF);
		glStencilMask(0x1);
		glEnable(GL_STENCIL_TEST);
	}
	else
	{
		glDisable(GL_STENCIL_TEST);
	}
}

void GADVisualPass::SetupColorBlendState()
{
	assert(mpVisualPass != nullptr);

	if (mpVisualPass->GetPassType() == VisualPass::PassType::GE_PT_SHADOWS)
	{
		glDisable(GL_BLEND);

		return;
	}

	auto& colorBlendState = mpVisualPass->GetColorBlendState();

	auto blendEnabled = colorBlendState.GetIsBlendEnabled();

	if (blendEnabled)
	{
		// NOTE! We need one blend attachment state per color attachment (even if blending is not used)
		auto glSrcColorBlendFactor = OpenGLUtils::BlendFactorToOpenGLBlendFactor(colorBlendState.GetSrcColorBlendFactor());
		auto glDstColorBlendFactor = OpenGLUtils::BlendFactorToOpenGLBlendFactor(colorBlendState.GetDstColorBlendFactor());
		auto glColorBlendOp = OpenGLUtils::BlendOpToOpenGLBlendOp(colorBlendState.GetColorBlendOp());
		auto glSrcAlphaBlendFactor = OpenGLUtils::BlendFactorToOpenGLBlendFactor(colorBlendState.GetSrcAlphaBlendFactor());
		auto glDstAlphaBlendFactor = OpenGLUtils::BlendFactorToOpenGLBlendFactor(colorBlendState.GetDstAlphaBlendFactor());
		auto glAlphaBlendOp = OpenGLUtils::BlendOpToOpenGLBlendOp(colorBlendState.GetAlphaBlendOp());

		auto constantColor = colorBlendState.GetConstantColor();

		glBlendFuncSeparate(glSrcColorBlendFactor, glDstColorBlendFactor, glSrcAlphaBlendFactor, glDstAlphaBlendFactor);
		glBlendEquationSeparate(glColorBlendOp, glAlphaBlendOp);

		// NOTE! If constant color is used
		glBlendColor(constantColor.r, constantColor.g, constantColor.b, constantColor.a);

		glEnable(GL_BLEND);
	}
	else
	{
		glDisable(GL_BLEND);
	}
}

void GADVisualPass::SetupDynamicState()
{
	assert(mpVisualPass != nullptr);

	// No equivalent for OpenGL
}

void GADVisualPass::BindUniforms()
{
	assert(mpVisualPass != nullptr);
	assert(mpOpenGLRenderer != nullptr);

	const auto& shaders = mpVisualPass->GetShaders();

	size_t i = 0;
	for (auto iter = shaders.begin(); iter != shaders.end(); ++iter)
	{
		auto shaderStage = iter->first;
		auto* pShader = iter->second;
		assert(pShader != nullptr);

		auto* pGlslParser = pShader->GetGLSLParser();
		assert(pGlslParser != nullptr);

		// bind current shader ubo binding point to the UBO GL buffer
		if (pGlslParser->GetUniformBlock().IsValid())
		{
			auto* pUniformBuffer = mpVisualPass->GetUniformBuffer(shaderStage);
			if (pUniformBuffer)
			{
				auto* pGADRUniformBuffer = mpOpenGLRenderer->Get(pUniformBuffer);
				auto& uniformBlock = pGlslParser->GetUniformBlock();
				if (pGADRUniformBuffer && uniformBlock.IsValid())
				{
					auto* pGLBuffer = pGADRUniformBuffer->GetGLBuffer();
					if (pGLBuffer)
					{
						pGLBuffer->Bind();

						glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlock.binding, pGLBuffer->GetHandle());

						pGLBuffer->UnBind();
					}
				}
			}
		}

		// texture sampler(s) are present
		if (pGlslParser->GetUniforms().empty() == false && mpVisualPass->HasTextures())
		{
			auto& uniforms = pGlslParser->GetUniforms();

			auto& textureMap = mpVisualPass->GetTextures();
			auto it = textureMap.find(pShader->GetShaderStage());
			if (it != textureMap.end())
			{
				auto& textureArray = it->second;
				assert(textureArray.size() == uniforms.size()); //TODO - improve this

				uint8_t texUnit = mStartTextUnit;

				auto texIter = textureArray.begin();
				auto unifIter = uniforms.begin();
				for (; texIter != textureArray.end(); ++texIter, ++unifIter)
				{
					auto* pTexture = *texIter;

					if (pTexture->GetUsageType() == Texture::UsageType::GE_UT_RENDER)
					{
						auto* gadrTexture = mpOpenGLRenderer->Get(pTexture);
						assert(gadrTexture != nullptr);

						gadrTexture->Bind(texUnit);

						GLint loc = glGetUniformLocation(mpOpenGLShaderProgram->GetHandle(), unifIter->first.c_str());
						glUniform1i(loc, texUnit);
					}
					else if (pTexture->GetUsageType() == Texture::UsageType::GE_UT_RENDER_TARGET)
					{
						auto* pVisualEffect = mpVisualPass->GetVisualEffect();
						assert(pVisualEffect != nullptr);

						const auto& passMap = pVisualEffect->GetPasses();

						bool_t foundRT = false;
						VisualEffect::PassMap::const_iterator cPassIt;
						VisualPass::RenderTargetMap::const_iterator cRTIt;
						for (cPassIt = passMap.begin(); cPassIt != passMap.end(); ++cPassIt)
						{
							auto& passVector = cPassIt->second;
							for (auto* pPass : passVector)
							{
								if (pPass)
								{
									const auto& renderTargets = pPass->GetRenderTargets();
									for (cRTIt = renderTargets.begin(); cRTIt != renderTargets.end(); ++cRTIt)
									{
										auto* pRT = cRTIt->second;
										assert(pRT != nullptr);

										if (pRT->GetTexture() == pTexture)
										{
											foundRT = true;
											break;
										}
									}
								}
							}

							if (foundRT)
								break;
						}

						if (foundRT)
						{
							auto* pRT = cRTIt->second;

							auto* gadrTexture = mpOpenGLRenderer->Get(pRT);
							assert(gadrTexture != nullptr);

							gadrTexture->Bind(texUnit);

							GLint loc = glGetUniformLocation(mpOpenGLShaderProgram->GetHandle(), unifIter->first.c_str());
							glUniform1i(loc, texUnit);
						}
					}
				
					texUnit++;
				}
			}
		}
	}
}

void GADVisualPass::RenderNode(uint32_t currentBufferIdx)
{
	assert(mpVisualPass != nullptr);
	assert(mpOpenGLRenderer != nullptr);
	assert(mpOpenGLShaderProgram != nullptr);
	assert(mpOpenGLVertexArrayObject != nullptr);

	auto* pGeoNode = mpVisualPass->GetNode();
	assert(pGeoNode != nullptr);

	// setup global states - as opengl is a global state machine
	// for these states to have effect only for this node we need to activate them
	// just before rendering this node !!!

	//// Rasterization state
	SetupRasterizationState();

	// Multi sampling state
	// This example does not make use fo multi sampling (for anti-aliasing), the state must still be set and passed to the pipeline
	SetupMultisampleState();

	//// Depth and stencil state containing depth and stencil compare and test operations
	//// We only use depth tests and want depth tests and writes to be enabled and compare with less or equal
	SetupDepthStencilState();

	// Color blend state describes how blend factors are calculated (if used)
	// We need one blend attachment state per color attachment (even if blending is not used)
	SetupColorBlendState();

	///////////////////////////////////////


	// at this point all processed nodes are allowed for this visual pass !
	mpOpenGLShaderProgram->Bind();

	BindUniforms();

	mpOpenGLVertexArrayObject->Bind();

	mpOpenGLRenderer->DrawNode(mpVisualPass, pGeoNode, currentBufferIdx);

	mpOpenGLVertexArrayObject->UnBind();

//	mpOpenGLShaderProgram->UnBind();
}

void GADVisualPass::UpdateNode(Camera* pCamera, float32_t crrTime)
{
	assert(mpVisualPass != nullptr);
	assert(mpOpenGLRenderer != nullptr);

	auto* pGeoNode = mpVisualPass->GetNode();
	assert(pGeoNode != nullptr);

	// at this point all processed nodes are allowed for this visual pass !
	mpOpenGLRenderer->UpdateNode(mpVisualPass, pGeoNode, pCamera, crrTime);
}
#endif // OPENGL_RENDERER