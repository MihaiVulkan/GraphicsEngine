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

	/////////////////////////////

	//NOTE! We use KTX2 textures which use the r(right) d(dowwn) UV space
	// according to: http://github.khronos.org/KTX-Specification/
	// GL - r(right), u(up)
	// Vulkan - r(right), d(down)

	// light setup - light dir in world space, inverted on Y compared to OpenGL as Vulkan has a different coordinate system
	// in VUlkan to get -1.0 dir on OY I actually need to specify +1.0 (-1 * -1.0f)
	auto dirLight = GE_ALLOC(DirectionalLight)(glm::vec3(0.0f, +1.0f, 0.65f), Color3f(1.0f));
	auto lightNode = GE_ALLOC(LightNode)("Light");
	lightNode->SetLight(dirLight);

	// geometry setup

	////////// DRAGON MODEL ////////////
	auto dragon = GE_ALLOC(Model)(std::string() + GE_ASSET_PATH + "models/chinesedragon.gltf",
		glTF2Loader::LoadingFlags::GE_LF_TRANSFORM_POS | glTF2Loader::LoadingFlags::GE_LF_COLORED | glTF2Loader::LoadingFlags::GE_LF_LIT);

	auto dragonNode = GE_ALLOC(GeometryNode)("Dragon");
	dragonNode->SetGeometry(dragon);
	dragonNode->SetModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, +0.7f, 0.0f)));

	auto colorEffect_1 = GE_ALLOC(LitColorAttributeVisualEffect);

	// add the impacted node
	colorEffect_1->SetTargetNode(dragonNode);
	dragonNode->GetComponent<VisualComponent>()->SetVisualEffect(colorEffect_1);

	////////////// SUZANNE MODEL /////////
	auto suzanne = GE_ALLOC(Model)(std::string() + GE_ASSET_PATH + "models/suzanne.gltf",
		glTF2Loader::LoadingFlags::GE_LF_TRANSFORM_POS | glTF2Loader::LoadingFlags::GE_LF_LIT);

	auto suzanneNode = GE_ALLOC(GeometryNode)("Suzanne");
	suzanneNode->SetGeometry(suzanne);
	suzanneNode->SetModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(+2.0f, +1.0f, 2.5f)));

	auto colorEffect_2 = GE_ALLOC(LitColorVisualEffect)({ 0.0f, 1.0f, 1.0f, 1.0f });

	// add the impacted node
	colorEffect_2->SetTargetNode(suzanneNode);
	suzanneNode->GetComponent<VisualComponent>()->SetVisualEffect(colorEffect_2);


	////////// PLANE MODEL - MIRROR ////////////

	// Model
	auto mirror = GE_ALLOC(Model)(std::string() + GE_ASSET_PATH + "models/plane.gltf",
		glTF2Loader::LoadingFlags::GE_LF_TRANSFORM_POS | glTF2Loader::LoadingFlags::GE_LF_TEXTURED | glTF2Loader::LoadingFlags::GE_LF_LIT);

	auto mirrorNode = GE_ALLOC(GeometryNode)("Mirror");
	mirrorNode->SetIsLit(true); // lit node !
	mirrorNode->SetGeometry(mirror);

	auto mirrorVisualEffect = GE_ALLOC(LitMirrorTextureVisualEffect)(0.03f);

	// add the impacted node
	mirrorVisualEffect->SetTargetNode(mirrorNode);
	mirrorVisualEffect->AddDependencyNode(dragonNode);
	mirrorVisualEffect->AddDependencyNode(suzanneNode);
	mirrorNode->GetComponent<VisualComponent>()->SetVisualEffect(mirrorVisualEffect);


	/////////////////////////////

	// camera- TODO - to be moved as cull camera as part of the scene graph
	app->GetGraphicsSystem()->GetMainCamera()->SetPosition(glm::vec3(0.0f, 3.0f, 15.0f));
	app->GetGraphicsSystem()->GetMainCamera()->SetFOV(60.0f);
	app->GetGraphicsSystem()->GetMainCamera()->SetZNear(0.1f);
	app->GetGraphicsSystem()->GetMainCamera()->SetZFar(100.0f);
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