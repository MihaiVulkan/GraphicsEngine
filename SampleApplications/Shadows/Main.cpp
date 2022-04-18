#include <GraphicsEngine.hpp>
#include <vector>
#include <chrono>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

#define SHADOW_MAP_WIDTH 2048
#define SHADOW_MAP_HEIGHT 2048


int main()
{
	auto startTime = std::chrono::system_clock::now();

	//TODO All objects should be dynamically allocated and cached in a Catalog/ResourceManager for memory management
	auto app = GE_ALLOC(WindowApplication)("Projected Shadow mapping", 1920, 1080);


	//NOTE! We use KTX2 textures which use the r(right) d(dowwn) UV space
	// according to: http://github.khronos.org/KTX-Specification/
	// GL - r(right), u(up)
	// Vulkan - r(right), d(down)

	// light setup - light pos in world space, inverted on Y compared to OpenGL as Vulkan has a different coordinate system
	// in Vulkan to get -1.0 dir on OY I actually need to specify +1.0 (-1 * -1.0f)
	auto pointLight = GE_ALLOC(PointLight)(glm::vec3(10.0f, +30.0f, 25.0f), Color3f(1.0f));
	auto lightNode = GE_ALLOC(LightNode)("Light");
	lightNode->SetLight(pointLight);

	// geometry setup

	////////// SEVERAL MODELS ////////////
	auto models = GE_ALLOC(Model)(std::string() + GE_ASSET_PATH + "models/vulkanscene_shadow.gltf",
		glTF2Loader::LoadingFlags::GE_LF_TRANSFORM_POS | glTF2Loader::LoadingFlags::GE_LF_COLORED | glTF2Loader::LoadingFlags::GE_LF_LIT);

	auto modelsNode = GE_ALLOC(GeometryNode)("VulkanScene_shadow");
	modelsNode->SetGeometry(models);

	auto shadowEffect_1 = GE_ALLOC(LitShadowColorAttributeVisualEffect);

	// add the impacted node
	shadowEffect_1->SetTargetNode(modelsNode);
	modelsNode->GetComponent<VisualComponent>()->SetVisualEffect(shadowEffect_1);

	//////////// Torusknot ///////////

	auto torusknot = GE_ALLOC(Model)(std::string() + GE_ASSET_PATH + "models/torusknot.gltf",
		glTF2Loader::LoadingFlags::GE_LF_TRANSFORM_POS | glTF2Loader::LoadingFlags::GE_LF_COLORED | glTF2Loader::LoadingFlags::GE_LF_LIT);

	auto torusknotNode = GE_ALLOC(GeometryNode)("Torusknot");
	torusknotNode->SetGeometry(torusknot);
	torusknotNode->SetModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(+4.5f, +1.0f, +4.5f)));

	auto shadowEffect_2 = GE_ALLOC(LitShadowColorAttributeVisualEffect);

	// add the impacted node
	shadowEffect_2->SetTargetNode(torusknotNode);
	torusknotNode->GetComponent<VisualComponent>()->SetVisualEffect(shadowEffect_2);

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
	scene->AttachNode(modelsNode);
	scene->AttachNode(torusknotNode);

	app->GetGraphicsSystem()->SetScene(scene);

	auto endTime = std::chrono::system_clock::now();
	std::chrono::duration<float32_t> diffTime = endTime - startTime;
	LOG_INFO("Startup loading time: %f", diffTime.count());

	app->Run();

	GE_FREE(app);
	//The rest of heap memory is cleared automatically

	return 0;
}