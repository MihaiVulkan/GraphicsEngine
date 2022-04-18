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
	auto lightNode = GE_ALLOC(LightNode)("Light");
	lightNode->SetLight(dirLight);


	// geometry setup

	///////// SKYBOX ////////////

	auto skybox = GE_ALLOC(Model)(std::string() + GE_ASSET_PATH + "models/cube.gltf",
		glTF2Loader::LoadingFlags::GE_LF_TRANSFORM_POS | glTF2Loader::LoadingFlags::GE_LF_TEXTURED);

	auto skyboxTexture = GE_ALLOC(TextureCubeMap)(std::string() + GE_ASSET_PATH + "textures/cubemap.ktx2");

	//TODO - add check support for all enabled features (physical device)

	//TODO - enable anisotropic filtering - via physical device enabled features
	auto skyboxNode = GE_ALLOC(GeometryNode)("Skybox");
	skyboxNode->SetGeometry(skybox);
	skyboxNode->SetModelMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(20.0f)));

	auto cubemapVisualEffect = GE_ALLOC(UnlitCubemapTextureVisualEffect)(skyboxTexture);

	// add the impacted node
	cubemapVisualEffect->SetTargetNode(skyboxNode);
	skyboxNode->GetComponent<VisualComponent>()->SetVisualEffect(cubemapVisualEffect);

	////////// VENUS MODEL - ENVIRONMENT REFLECTION ////////////

	auto venus = GE_ALLOC(Model)(std::string() + GE_ASSET_PATH + "models/venus.gltf",
		glTF2Loader::GE_LF_TRANSFORM_POS | glTF2Loader::GE_LF_TEXTURED | glTF2Loader::GE_LF_LIT);

	auto venusReflectNode = GE_ALLOC(GeometryNode)("Reflective model");
	venusReflectNode->SetGeometry(venus);
	venusReflectNode->SetModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f)));


	auto envMappingReflectVisualEffect = GE_ALLOC(LitEnvironmentMappingTextureVisualEffect)(skyboxTexture, 
		LitEnvironmentMappingTextureVisualEffect::MappingType::GE_MT_REFLECTIVE);

	// add the impacted node
	envMappingReflectVisualEffect->SetTargetNode(venusReflectNode);
	venusReflectNode->GetComponent<VisualComponent>()->SetVisualEffect(envMappingReflectVisualEffect);


	////////// VENUS MODEL - ENVIRONMENT REFRACTION ////////////

	auto venusRefractNode = GE_ALLOC(GeometryNode)("Refractive model");
	venusRefractNode->SetIsLit(true); // lit node !
	venusRefractNode->SetGeometry(venus);
	venusRefractNode->SetModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(+2.0f, 0.0f, 0.0f)));


	auto envMappingRefractVisualEffect = GE_ALLOC(LitEnvironmentMappingTextureVisualEffect)(skyboxTexture,
		LitEnvironmentMappingTextureVisualEffect::MappingType::GE_MT_REFRACTIVE);

	// add the impacted node
	envMappingRefractVisualEffect->SetTargetNode(venusRefractNode);
	venusRefractNode->GetComponent<VisualComponent>()->SetVisualEffect(envMappingRefractVisualEffect);

	/////////////////////////////

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