#include <GraphicsEngine.hpp>
#include <vector>
#include <chrono>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

#define VK_Y -1
#define GL_Y 1

#define FLIPY VK_Y // Vulkan Renderer setup, TODO - for the other renderers

int main()
{
	auto startTime = std::chrono::system_clock::now();

	//TODO All objects should be dynamically allocated and cached in a Catalog/ResourceManager for memory management
	auto app = GE_ALLOC(WindowApplication)("Textured Cubes", 1920, 1080);


	//NOTE! We use KTX2 textures which use the r(right) d(dowwn) UV space
	// according to: http://github.khronos.org/KTX-Specification/
	// GL - r(right), u(up)
	// Vulkan - r(right), d(down)

	// Default face winding
	// GL - CCW
	// Vulkan - CW

	///////////////////////////////////////////  SETUP Room - skybox
	// Our room is actually a skybox. It uses a KT2 texture. KTX2 Cubemap faces are stored in the order: +X, -X, +Y, -Y, +Z, -Z.

	std::vector<float32_t> vertexDataRoom
	//NOTE! When drawing a textured cube the optimized data approch of only using 8 vertices and 36 indices will not work as
	// there is not enough data for the uv coordinates, so the texture will be renderer as broken.
	// For this reason we use the 24 vertices approach with no indices.
	// object space position, uv
	{
		// right face (+X)
		  1.0f, -1.0f * FLIPY, 1.0f, //12
		  1.0f, 1.0f * FLIPY, 1.0f, //13
		  1.0f, 1.0f * FLIPY, -1.0f, //14
		  1.0f, -1.0f * FLIPY, -1.0f, //15
		// left face (-X)
		  -1.0f, -1.0f * FLIPY, -1.0f, //8
		  -1.0f, 1.0f * FLIPY, -1.0f, //9
		  -1.0f, 1.0f * FLIPY, 1.0f, //10
		  -1.0f, -1.0f * FLIPY, 1.0f, //11
		// top face (+Y)
		  -1.0f, 1.0f * FLIPY, 1.0f, //16
		  -1.0f, 1.0f * FLIPY, -1.0f, //17
		  1.0f, 1.0f * FLIPY, -1.0f, //18
		  1.0f, 1.0f * FLIPY, 1.0f, //19
		// bottom face (-Y)
		  -1.0f, -1.0f * FLIPY, -1.0f, //20
		  -1.0f, -1.0f * FLIPY, 1.0f, //21
		  1.0f, -1.0f * FLIPY, 1.0f, //22
		  1.0f, -1.0f * FLIPY, -1.0f, //23
		// front face (+Z)
		 -1.0f, -1.0f * FLIPY, 1.0f, //0
		 -1.0f, 1.0f * FLIPY, 1.0f, //1
		  1.0f, 1.0f * FLIPY, 1.0f, //2
		  1.0f, -1.0f * FLIPY, 1.0f, //3
		// back face (-Z)
		  1.0f, -1.0f * FLIPY, -1.0f, //4
		  1.0f, 1.0f * FLIPY, -1.0f, //5
		  -1.0f, 1.0f * FLIPY, -1.0f, //6
		  -1.0f, -1.0f * FLIPY, -1.0f, //7
	};

	// face widing - counter clock-wise
	std::vector<uint32_t> indexDataRoom
	{
	#if FLIPY == VK_Y
		// right face (+X)
		13, 14, 12, 14, 15, 12,
		// left face (-X)
		9, 10, 8, 10, 11, 8,
		// top face (+Y)
		21, 22, 20, 22, 23, 20,
		// bottom face (-Y)
		17, 18, 16, 18, 19, 16,
		// front face (+Z)
		1, 2, 0, 2, 3, 0,
		// back face (-Z)
		5, 6, 4, 6, 7, 4
	#else
		// right face (+X)
		12, 15, 13, 15, 14, 13,
		// left face (-X)
		8, 11, 9, 11, 10, 9,
		// top face (+Y)
		16, 19, 17, 19, 18, 17,
		// bottom face (-Y)
		20, 23, 21, 23, 22, 21,
		// front face (+Z)
		0, 3, 1, 3, 2, 1,
		// back face (-Z)
		4, 7, 5, 7, 6, 5
	#endif
	};

	auto vfRoom = GE_ALLOC(VertexFormat)(3, 0, 0, 0, 0); //P3
	auto vbRoom = GE_ALLOC(VertexBuffer)(vfRoom, Buffer::BufferUsage::GE_BU_STATIC, vertexDataRoom.data(), vertexDataRoom.size() * sizeof(float32_t));
	auto ibRoom = GE_ALLOC(IndexBuffer)(IndexBuffer::BufferUsage::GE_BU_STATIC, IndexBuffer::IndexType::GE_IT_UINT32, indexDataRoom.data(), indexDataRoom.size() * sizeof(uint32_t));

	vertexDataRoom.clear();
	indexDataRoom.clear();

	auto room = GE_ALLOC(GeometricPrimitive);

	room->SetVertexBuffer(vbRoom);
	room->SetIndexBuffer(ibRoom);

	auto textureRoom = GE_ALLOC(TextureCubeMap)(std::string() + GE_ASSET_PATH + "textures/cubemap_yokohama_rgba.ktx2");

	auto roomNode = GE_ALLOC(GeometryNode)("Textured room");
	roomNode->SetGeometry(room);
	roomNode->SetModelMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(100.0f)));

	auto cubemapVisualEffect = GE_ALLOC(UnlitCubemapTextureVisualEffect)(textureRoom);

	// add the impacted node
	cubemapVisualEffect->SetTargetNode(roomNode);
	roomNode->GetComponent<VisualComponent>()->SetVisualEffect(cubemapVisualEffect);

	/////////////////////////////////////////// SETUP Single Texture Cube
	// geometry setup
	std::vector<float32_t> vertexDataCube
	//NOTE! When drawing a textured cube the optimized data approch of only using 8 vertices and 36 indices will not work as
	// there is not enough data for the uv coordinates, so the texture will be renderer as broken.
	// For this reason we use the 24 vertices approach with no indices.
	// object space position, uv
	{
		// front face
		 -1.0f, -1.0f * FLIPY, 1.0f,   0.0f, 0.0f, //0
		 -1.0f, 1.0f * FLIPY, 1.0f,   0.0f, 1.0f, //1
		  1.0f, 1.0f * FLIPY, 1.0f,   1.0f, 1.0f, //2
		  1.0f, -1.0f * FLIPY, 1.0f,   1.0f, 0.0f, //3
		// back face
		  1.0f, -1.0f * FLIPY, -1.0f,   0.0f, 0.0f, //4
		  1.0f, 1.0f * FLIPY, -1.0f,   0.0f, 1.0f, //5
		  -1.0f, 1.0f * FLIPY, -1.0f,   1.0f, 1.0f, //6
		  -1.0f, -1.0f * FLIPY, -1.0f,   1.0f, 0.0f, //7
		// left face
		  -1.0f, -1.0f * FLIPY, -1.0f,   0.0f, 0.0f, //8
		  -1.0f, 1.0f * FLIPY, -1.0f,   0.0f, 1.0f, //9
		  -1.0f, 1.0f * FLIPY, 1.0f,   1.0f, 1.0f, //10
		  -1.0f, -1.0f * FLIPY, 1.0f,   1.0f, 0.0f, //11
		// right face
		  1.0f, -1.0f * FLIPY, 1.0f,   0.0f, 0.0f, //12
		  1.0f, 1.0f * FLIPY, 1.0f,   0.0f, 1.0f, //13
		  1.0f, 1.0f * FLIPY, -1.0f,   1.0f, 1.0f, //14
		  1.0f, -1.0f * FLIPY, -1.0f,   1.0f, 0.0f, //15
		// top face (top face on flipY becomes bottom face)
		  -1.0f, 1.0f * FLIPY, 1.0f,   0.0f, 0.0f, //16
		  -1.0f, 1.0f * FLIPY, -1.0f,   0.0f, 1.0f, //17
		  1.0f, 1.0f * FLIPY, -1.0f,   1.0f, 1.0f, //18
		  1.0f, 1.0f * FLIPY, 1.0f,   1.0f, 0.0f, //19
		// bottom face (top face on flipY becomes top face)
		  -1.0f, -1.0f * FLIPY, -1.0f,   0.0f, 0.0f, //20
		  -1.0f, -1.0f * FLIPY, 1.0f,   0.0f, 1.0f, //21
		  1.0f, -1.0f * FLIPY, 1.0f,   1.0f, 1.0f, //22
		  1.0f, -1.0f * FLIPY, -1.0f,   1.0f, 0.0f, //23



		 //-1.0f, 1.0f, 1.0f, //0
		 //1.0f, 1.0, 1.0f, //1
		 //-1.0f, 1.0f, -1.0f, //2
		 //1.0f, 1.0, -1.0f, //3

		 //-1.0f, -1.0f, 1.0f, //0
		 //1.0f, -1.0, 1.0f, //1
		 //-1.0f, -1.0f, -1.0f, //2
		 //1.0f, -1.0, -1.0f, //3

	};

	// Winding default:
	// GL - CCW
	// Vulkan - CW

	// face widing - counter clock-wise
	std::vector<uint32_t> indexDataCube
	{
	#if FLIPY == VK_Y
		// front face
		1, 2, 0, 2, 3, 0,
		// back face
		5, 6, 4, 6, 7, 4,
		// left face
		9, 10, 8, 10, 11, 8,
		// right face
		13, 14, 12, 14, 15, 12,
		// top face
		21, 22, 20, 22, 23, 20,
		// bottom face
		17, 18, 16, 18, 19, 16
	#else
		// front face
		0, 3, 1, 3, 2, 1,
		// back face
		4, 7, 5, 7, 6, 5,
		// left face
		8, 11, 9, 11, 10, 9,
		// right face
		12, 15, 13, 15, 14, 13,
		// top face
		16, 19, 17, 19, 18, 17,
		// bottom face
		20, 23, 21, 23, 22, 21
	#endif

		// GL - CCW
		// front face
		//0, 4, 5, 0, 5, 1,
		//// back face
		//3, 7, 6, 3, 6, 2,
		//// left face
		//2, 6, 4, 2, 4, 0,
		//// right face
		//1, 5, 7, 1, 7, 3,
		//// top face
		//2, 0, 1, 2, 1, 3,
		//// bottom face
		//7, 5, 4, 7, 4, 6
	};


	auto vfCube = GE_ALLOC(VertexFormat)(3, 0, 0, 0, 2); //P3 UV2
	auto vbCube = GE_ALLOC(VertexBuffer)(vfCube, Buffer::BufferUsage::GE_BU_STATIC, vertexDataCube.data(), vertexDataCube.size() * sizeof(float32_t));
	auto ibCube = GE_ALLOC(IndexBuffer)(Buffer::BufferUsage::GE_BU_STATIC, IndexBuffer::IndexType::GE_IT_UINT32, indexDataCube.data(), indexDataCube.size() * sizeof(uint32_t));

	auto cube = GE_ALLOC(GeometricPrimitive);

	cube->SetVertexBuffer(vbCube);
	cube->SetIndexBuffer(ibCube);

	auto textureCube = GE_ALLOC(Texture2D)(std::string() + GE_ASSET_PATH + "textures/vulkan_11_rgba.ktx2");

	//TODO - add check support for all enabled features (physical device)

	//TODO - enable anisotropic filtering - via physical device enabled features
	auto cubeNode = GE_ALLOC(GeometryNode)("Textured cube");
	cubeNode->SetGeometry(cube);

	auto tex2DVisualEffect = GE_ALLOC(Unlit2DTextureVisualEffect)(textureCube);

	// add the impacted node
	tex2DVisualEffect->SetTargetNode(cubeNode);
	cubeNode->GetComponent<VisualComponent>()->SetVisualEffect(tex2DVisualEffect);

	/////////////////////////////////////////// SETUP Multi Texture (Array) Cube

	auto cubeTextureArray = GE_ALLOC(GeometricPrimitive);

	cubeTextureArray->SetVertexBuffer(vbCube);
	cubeTextureArray->SetIndexBuffer(ibCube);

	vertexDataCube.clear();
	indexDataCube.clear();

	auto textureArrayCube = GE_ALLOC(Texture2DArray)(std::string() + GE_ASSET_PATH + "textures/texturearray_rgba.ktx2");

	//TODO - add check support for all enabled features (physical device)

	//TODO - enable anisotropic filtering - via physical device enabled features
	auto cubeTextureArrayNode = GE_ALLOC(GeometryNode)("Multi-Textured cube");
	cubeTextureArrayNode->SetGeometry(cubeTextureArray);
	cubeTextureArrayNode->SetModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, 0.0f)));

	// add the impacted node
	auto tex2DArrayVisualEffect = GE_ALLOC(Unlit2DTextureArrayVisualEffect)(textureArrayCube);

	// add the impacted node
	tex2DArrayVisualEffect->SetTargetNode(cubeTextureArrayNode);
	cubeTextureArrayNode->GetComponent<VisualComponent>()->SetVisualEffect(tex2DArrayVisualEffect);

	/////////////////////////////

	// camera- TODO - to be moved as cull camera as part of the scene graph
	app->GetGraphicsSystem()->GetMainCamera()->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
	app->GetGraphicsSystem()->GetMainCamera()->SetFOV(60.0f);
	app->GetGraphicsSystem()->GetMainCamera()->SetZNear(0.1f);
	app->GetGraphicsSystem()->GetMainCamera()->SetZFar(1000.0f);
	app->GetGraphicsSystem()->GetMainCamera()->EnableConstraints();

	////////////////////////////////////
	auto scene = GE_ALLOC(GroupNode);
	scene->AttachNode(roomNode);
	scene->AttachNode(cubeNode);
	scene->AttachNode(cubeTextureArrayNode);

	app->GetGraphicsSystem()->SetScene(scene);

	auto endTime = std::chrono::system_clock::now();
	std::chrono::duration<float32_t> diffTime = endTime - startTime;
	LOG_INFO("Startup loading time: %f", diffTime.count());

	app->Run();

	GE_FREE(app);
	//The rest of heap memory is cleared automatically

	return 0;
}