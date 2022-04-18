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
	auto app = GE_ALLOC(WindowApplication)("LitModels", 1920, 1080);


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

	// GL - CCW winding
	auto model = GE_ALLOC(Model)(std::string() + GE_ASSET_PATH + "models/armor.gltf", 
		glTF2Loader::LoadingFlags::GE_LF_TRANSFORM_POS | glTF2Loader::LoadingFlags::GE_LF_TEXTURED | glTF2Loader::LoadingFlags::GE_LF_LIT);

	auto colorTexture = GE_ALLOC(Texture2D)(std::string() + GE_ASSET_PATH + "models/colormap_rgba.ktx2");

	//TODO - add check support for all enabled features (physical device)

	//TODO - enable anisotropic filtering - via physical device enabled features
	auto modelNode = GE_ALLOC(GeometryNode)("Armor");
	modelNode->SetGeometry(model);

	auto visualEffect = GE_ALLOC(Lit2DTextureVisualEffect)(colorTexture);

	// add the impacted node
	visualEffect->SetTargetNode(modelNode);
	modelNode->GetComponent<VisualComponent>()->SetVisualEffect(visualEffect);

	/////////////////////////////

	// camera- TODO - to be moved as cull camera as part of the scene graph
	app->GetGraphicsSystem()->GetMainCamera()->SetPosition(glm::vec3(0.0f, 0.0f, 15.0f));
	app->GetGraphicsSystem()->GetMainCamera()->SetFOV(60.0f);
	app->GetGraphicsSystem()->GetMainCamera()->SetZNear(0.1f);
	app->GetGraphicsSystem()->GetMainCamera()->SetZFar(100.0f);
	app->GetGraphicsSystem()->GetMainCamera()->EnableConstraints();

	////////////////////////////////////
	auto scene = GE_ALLOC(GroupNode);
	scene->AttachNode(lightNode);
	scene->AttachNode(modelNode);

	app->GetGraphicsSystem()->SetScene(scene);

	auto endTime = std::chrono::system_clock::now();
	std::chrono::duration<float32_t> diffTime = endTime - startTime;
	LOG_INFO("Startup loading time: %f", diffTime.count());

	app->Run();

	GE_FREE(app);
	//The rest of heap memory is cleared automatically

	return 0;
}