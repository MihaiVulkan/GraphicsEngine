#include <GraphicsEngine.hpp>
#include <vector>
#include <chrono>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

#define RT_WIDTH 512
#define RT_HEIGHT 512

int main()
{
	auto startTime = std::chrono::system_clock::now();

	//TODO All objects should be dynamically allocated and cached in a Catalog/ResourceManager for memory management
	auto app = GE_ALLOC(WindowApplication)("Offscreen - Mirror", 1920, 1080);


	auto scenePass = GE_ALLOC(CompositeScenePass);
	// custom clear color for the default color framebuffer
	scenePass->Data().clearColor = { 0.0f, 0.0f, 0.2f, 1.0f };

	auto standardPass = GE_ALLOC(StandardScenePass);

	auto offscreenPass = GE_ALLOC(OffscreenScenePass);
	auto colorRT = GE_ALLOC(RenderTarget)(RenderTarget::TargetType::GE_TT_COLOR, RenderTarget::TargetOutput::GE_TO_RENDER_TEXTURE, RT_WIDTH, RT_HEIGHT);
	auto depthRT = GE_ALLOC(RenderTarget)(RenderTarget::TargetType::GE_TT_DEPTH_STENCIL, RenderTarget::TargetOutput::GE_TO_TEXTURE, RT_WIDTH, RT_HEIGHT);
	offscreenPass->AddRenderTarget(colorRT);
	offscreenPass->AddRenderTarget(depthRT);

	// simulate the mirror effect by scaling the geometry by -1 on OY axis and changing cull mode to front face
	ScenePass::VisualEffect effect;
	effect.isEnabled = true; // enable the effect
	CullFaceState cfs;
	cfs.SetCullMode(CullFaceState::CullMode::GE_CM_FRONT);
	effect.cullFaceState = cfs;
	effect.transform = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 1.0f));
	offscreenPass->SetVisualEffect(effect);
	//

	scenePass->AddPass(standardPass);
	scenePass->AddPass(offscreenPass);

	app->GetGraphicsSystem()->SetMainScenePass(scenePass);

	/////////////////////////////

	//NOTE! We use KTX2 textures which use the r(right) d(dowwn) UV space
	// according to: http://github.khronos.org/KTX-Specification/
	// GL - r(right), u(up)
	// Vulkan - r(right), d(down)

	// light setup - light dir in world space, inverted on Y compared to OpenGL as Vulkan has a different coordinate system
	// in VUlkan to get -1.0 dir on OY I actually need to specify +1.0 (-1 * -1.0f)
	auto dirLight = GE_ALLOC(DirectionalLight)(glm::vec3(0.0f, +1.0f, 0.65f), glm::vec3(1.0f));
	auto lightNode = GE_ALLOC(LightNode)("Light");
	lightNode->SetLight(dirLight);

	/// SHADERS ///// 
	auto litVS = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/litmodel.vert");
	auto litFS = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/litmodel.frag");

	// geometry setup

	////////// DRAGON MODEL ////////////
	auto dragon = GE_ALLOC(Model)(std::string() + GE_ASSET_PATH + "models/chinesedragon.gltf", glTF2Loader::FileLoadingFlags::PreTransformVertices | glTF2Loader::FileLoadingFlags::PreMultiplyVertexColors);

	auto dragonNode = GE_ALLOC(GeometryNode)("Dragon");
	dragonNode->SetAllowedPasses({ ScenePass::PassType::GE_PT_STANDARD, ScenePass::PassType::GE_PT_OFFSCREEN });

	dragonNode->SetIsLit(true); // lit node !

	dragonNode->SetGeometry(dragon);
	dragonNode->GetComponent<VisualComponent>()->AddShader(litVS);
	dragonNode->GetComponent<VisualComponent>()->AddShader(litFS);

	//// respected vertex shader order of elements in UBO
	dragonNode->GetComponent<VisualComponent>()->GetUniformBuffer(ScenePass::PassType::GE_PT_STANDARD, Shader::ShaderStage::GE_SS_VERTEX)->AddUniform(GLSLShaderTypes::UniformType::GE_UT_CAMERA_POS);
	dragonNode->GetComponent<VisualComponent>()->GetUniformBuffer(ScenePass::PassType::GE_PT_OFFSCREEN, Shader::ShaderStage::GE_SS_VERTEX)->AddUniform(GLSLShaderTypes::UniformType::GE_UT_CAMERA_POS);

	dragonNode->SetModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, +0.7f, 0.0f)));

	////////////// SUZANNE MODEL /////////
	auto suzanne = GE_ALLOC(Model)(std::string() + GE_ASSET_PATH + "models/suzanne.gltf", glTF2Loader::FileLoadingFlags::PreTransformVertices | glTF2Loader::FileLoadingFlags::PreMultiplyVertexColors);

	auto suzanneNode = GE_ALLOC(GeometryNode)("Suzanne");
	suzanneNode->SetAllowedPasses({ ScenePass::PassType::GE_PT_STANDARD, ScenePass::PassType::GE_PT_OFFSCREEN });

	suzanneNode->SetIsLit(true); // lit node !

	suzanneNode->SetGeometry(suzanne);
	suzanneNode->GetComponent<VisualComponent>()->AddShader(litVS);
	suzanneNode->GetComponent<VisualComponent>()->AddShader(litFS);

	//// respected vertex shader order of elements in UBO
	suzanneNode->GetComponent<VisualComponent>()->GetUniformBuffer(ScenePass::PassType::GE_PT_STANDARD, Shader::ShaderStage::GE_SS_VERTEX)->AddUniform(GLSLShaderTypes::UniformType::GE_UT_CAMERA_POS);

	suzanneNode->SetModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(+2.0f, +1.0f, 2.5f)));

	////////// PLANE MODEL - MIRROR ////////////

	// Model
	auto mirror = GE_ALLOC(Model)(std::string() + GE_ASSET_PATH + "models/plane.gltf", glTF2Loader::FileLoadingFlags::PreTransformVertices);
	
	// Shaders
	auto mirrorVS = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/mirror.vert");
	auto mirrorFS = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/mirror.frag");

	// Textures
	auto normalTexture = GE_ALLOC(Texture2D)(std::string() + GE_ASSET_PATH + "textures/stonefloor01_normal_rgba.ktx2");

	auto mirrorNode = GE_ALLOC(GeometryNode)("Mirror");
	mirrorNode->SetIsLit(true); // lit node !

	mirrorNode->SetGeometry(mirror);
	mirrorNode->GetComponent<VisualComponent>()->AddShader(mirrorVS);
	mirrorNode->GetComponent<VisualComponent>()->AddShader(mirrorFS);

	// NOTE! Texture order in here respects the sampler order in the fragment shader!
	// sample the color from colorRT
	mirrorNode->GetComponent<VisualComponent>()->AddTexture(colorRT->GetTexture(), Shader::ShaderStage::GE_SS_FRAGMENT);
	// sample normals for the wobble effect over the mirror
	mirrorNode->GetComponent<VisualComponent>()->AddTexture(normalTexture, Shader::ShaderStage::GE_SS_FRAGMENT);

	// respected vertex shader order of elements in UBO
	mirrorNode->GetComponent<VisualComponent>()->GetUniformBuffer(ScenePass::PassType::GE_PT_STANDARD, Shader::ShaderStage::GE_SS_VERTEX)->AddUniform(GLSLShaderTypes::UniformType::GE_UT_CAMERA_POS);

	//NOTE! mirrorNode has the StandardPass allowed by default 

	/////////////////////////////

	// camera- TODO - to be moved as cull camera as part of the scene graph
	app->GetGraphicsSystem()->GetMainCamera()->SetPosition(glm::vec3(0.0f, 3.0f, 15.0f));
	app->GetGraphicsSystem()->GetMainCamera()->SetFOV(60.0f);
	app->GetGraphicsSystem()->GetMainCamera()->SetZNear(0.1f);
	app->GetGraphicsSystem()->GetMainCamera()->SetZFar(1000.0f);
	app->GetGraphicsSystem()->GetMainCamera()->EnableConstraints();

	////////////////////////////////////
	auto scene = GE_ALLOC(GroupNode);
	scene->AttachNode(lightNode);
	scene->AttachNode(dragonNode);
	scene->AttachNode(suzanneNode);
	scene->AttachNode(mirrorNode);

	app->GetGraphicsSystem()->SetScene(scene);

	auto endTime = std::chrono::system_clock::now();
	std::chrono::duration<float32_t> diffTime = endTime - startTime;
	LOG_INFO("Startup loading time: %f", diffTime.count());

	app->Run();

	GE_FREE(app);
	//The rest of heap memory is cleared automatically

	return 0;
}