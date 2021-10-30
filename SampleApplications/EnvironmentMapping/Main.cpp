#include <GraphicsEngine.hpp>
#include <vector>
#include <chrono>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

int main()
{
	auto startTime = std::chrono::system_clock::now();

	//TODO All objects should be dynamically allocated and cached in a Catalog/ResourceManager for memory management
	auto app = GE_ALLOC(WindowApplication)("EnvironmentMapping", 1920, 1080);


	//NOTE! We use KTX2 textures which use the r(right) d(dowwn) UV space
	// according to: http://github.khronos.org/KTX-Specification/
	// GL - r(right), u(up)
	// Vulkan - r(right), d(down)

	// light setup - light dir in world space, inverted on Y compared to OpenGL as Vulkan has a different coordinate system
	// in VUlkan to get -1.0 dir on OY I actually need to specify +1.0 (-1 * -1.0f)
	auto dirLight = GE_ALLOC(DirectionalLight)(glm::vec3(0.0f, +1.0f, 0.65f), glm::vec3(1.0f));
	auto lightNode = GE_ALLOC(LightNode);
	lightNode->SetLight(dirLight);


	// geometry setup

	///////// SKYBOX ////////////

	auto skybox = GE_ALLOC(Model)(std::string() + GE_ASSET_PATH + "models/cube.gltf", glTF2Loader::FileLoadingFlags::PreTransformVertices);

	auto vsSkybox = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/skybox.vert");
	auto fsSkybox = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/skybox.frag"); 

	auto skyboxTexture = GE_ALLOC(TextureCubeMap)(std::string() + GE_ASSET_PATH + "textures/cubemap.ktx2");

	//TODO - add check support for all enabled features (physical device)

	//TODO - enable anisotropic filtering - via physical device enabled features
	auto skyboxNode = GE_ALLOC(GeometryNode);

	skyboxNode->SetGeometry(skybox);
	skyboxNode->GetComponent<VisualComponent>()->AddShader(vsSkybox);
	skyboxNode->GetComponent<VisualComponent>()->AddShader(fsSkybox);
	skyboxNode->GetComponent<VisualComponent>()->AddTexture(skyboxTexture, Shader::ShaderStage::GE_SS_FRAGMENT);

	// respected vertex shader order of elements in UBO
//	skyboxNode->GetComponent<VisualComponent>()->GetUniformBuffer(Shader::ShaderStage::GE_SS_VERTEX)->AddUniform(GLSLShaderTypes::UniformType::GE_UT_PV_CUBEMAP_MATRIX4); 	// special case for cubemaps
	skyboxNode->GetComponent<VisualComponent>()->GetUniformBuffer(ScenePass::PassType::GE_PT_STANDARD, Shader::ShaderStage::GE_SS_VERTEX)->AddUniform(GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX4);
	skyboxNode->SetModelMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(20.0f)));

	skyboxNode->GetComponent<VisualComponent>()->GetCullFaceState().SetCullMode(CullFaceState::CullMode::GE_CM_FRONT);

	////////// VENUS MODEL - ENVIRONMENT REFLECTION ////////////

	auto venus = GE_ALLOC(Model)(std::string() + GE_ASSET_PATH + "models/venus.gltf", glTF2Loader::FileLoadingFlags::PreTransformVertices);

	auto vsLit = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/litmodel.vert");
	auto fsReflect = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/reflect.frag");

	auto venusReflectNode = GE_ALLOC(GeometryNode);
	venusReflectNode->SetIsLit(true); // lit node !

	venusReflectNode->SetGeometry(venus);
	venusReflectNode->GetComponent<VisualComponent>()->AddShader(vsLit);
	venusReflectNode->GetComponent<VisualComponent>()->AddShader(fsReflect);
	venusReflectNode->GetComponent<VisualComponent>()->AddTexture(skyboxTexture, Shader::ShaderStage::GE_SS_FRAGMENT);

	// respected vertex shader order of elements in UBO
	venusReflectNode->GetComponent<VisualComponent>()->GetUniformBuffer(ScenePass::PassType::GE_PT_STANDARD, Shader::ShaderStage::GE_SS_VERTEX)->AddUniform(GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX4);
	venusReflectNode->GetComponent<VisualComponent>()->GetUniformBuffer(ScenePass::PassType::GE_PT_STANDARD, Shader::ShaderStage::GE_SS_VERTEX)->AddUniform(GLSLShaderTypes::UniformType::GE_UT_NORMAL_MATRIX4);
	venusReflectNode->GetComponent<VisualComponent>()->GetUniformBuffer(ScenePass::PassType::GE_PT_STANDARD, Shader::ShaderStage::GE_SS_VERTEX)->AddUniform(GLSLShaderTypes::UniformType::GE_UT_CAMERA_POS);
	venusReflectNode->SetModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f)));

	////////// VENUS MODEL - ENVIRONMENT REFRACTION ////////////

	auto fsRefract = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/refract.frag");

	auto venusRefractNode = GE_ALLOC(GeometryNode);
	venusRefractNode->SetIsLit(true); // lit node !

	venusRefractNode->SetGeometry(venus);
	venusRefractNode->GetComponent<VisualComponent>()->AddShader(vsLit);
	venusRefractNode->GetComponent<VisualComponent>()->AddShader(fsRefract);
	venusRefractNode->GetComponent<VisualComponent>()->AddTexture(skyboxTexture, Shader::ShaderStage::GE_SS_FRAGMENT);

	// respected vertex shader order of elements in UBO
	venusRefractNode->GetComponent<VisualComponent>()->GetUniformBuffer(ScenePass::PassType::GE_PT_STANDARD, Shader::ShaderStage::GE_SS_VERTEX)->AddUniform(GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX4);
	venusRefractNode->GetComponent<VisualComponent>()->GetUniformBuffer(ScenePass::PassType::GE_PT_STANDARD, Shader::ShaderStage::GE_SS_VERTEX)->AddUniform(GLSLShaderTypes::UniformType::GE_UT_NORMAL_MATRIX4);
	venusRefractNode->GetComponent<VisualComponent>()->GetUniformBuffer(ScenePass::PassType::GE_PT_STANDARD, Shader::ShaderStage::GE_SS_VERTEX)->AddUniform(GLSLShaderTypes::UniformType::GE_UT_CAMERA_POS);
	venusRefractNode->SetModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(+2.0f, 0.0f, 0.0f)));


	/////////////////////////////

	// custom clear color for the default color framebuffer
	app->GetGraphicsSystem()->GetMainScenePass()->Data().clearColor = { 0.0f, 0.0f, 0.2f, 1.0f };

	// camera- TODO - to be moved as cull camera as part of the scene graph
	app->GetGraphicsSystem()->GetMainCamera()->SetPosition(glm::vec3(0.0f, 0.0f, 15.0f));
	app->GetGraphicsSystem()->GetMainCamera()->SetFOV(60.0f);
	app->GetGraphicsSystem()->GetMainCamera()->SetZNear(0.1f);
	app->GetGraphicsSystem()->GetMainCamera()->SetZFar(1000.0f);
	app->GetGraphicsSystem()->GetMainCamera()->EnableConstraints();

	////////////////////////////////////
	auto scene = GE_ALLOC(GroupNode);
	scene->AttachNode(lightNode);
	scene->AttachNode(skyboxNode);
	scene->AttachNode(venusReflectNode);
	scene->AttachNode(venusRefractNode);

	app->GetGraphicsSystem()->SetScene(scene);

	auto endTime = std::chrono::system_clock::now();
	std::chrono::duration<float32_t> diffTime = endTime - startTime;
	LOG_INFO("Startup loading time: %f", diffTime.count());

	app->Run();

	GE_FREE(app);
	//The rest of heap memory is cleared automatically

	return 0;
}