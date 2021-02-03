#include <GraphicsEngine.hpp>
#include <vector>
#include <chrono>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

#define FLIPY -1.0f // Vulkan Renderer setup, TODO - for the other renderers

int main()
{
	auto startTime = std::chrono::system_clock::now();

	//TODO All objects should be dynamically allocated and cached in a Catalog/ResourceManager for memory management
	WindowApplication app;
	
	// TODO - remove hardcoded window size
	app.Init("Textured Cubes", 1920, 1080);

	//NOTE! We use KTX2 textures which use the r(right) d(dowwn) UV space
	// according to: http://github.khronos.org/KTX-Specification/
	// GL - r(right), u(up)
	// Vulkan - r(right), d(down)

	// Default face winding
	// GL - CCW
	// VUlkan - CW

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
	#ifdef FLIPY
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

	VertexFormat vfRoom(3, 0, 0, 0, 0); //P3
	//VertexFormat::VF_P3_UV2;
	VertexBuffer vbRoom(&vfRoom, Buffer::BufferUsage::GE_BU_STATIC, VertexBuffer::VertexInputRate::GE_VIR_VERTEX, vertexDataRoom.data(), vertexDataRoom.size() * sizeof(float32_t));
	IndexBuffer ibRoom(IndexBuffer::BufferUsage::GE_BU_STATIC, IndexBuffer::IndexType::GE_IT_UINT32, indexDataRoom.data(), indexDataRoom.size() * sizeof(uint32_t));

	// GL - CW winding for cubemaps
	GeometricPrimitive room(GeometricPrimitive::PrimitiveTopology::GE_PT_TRIANGLE_LIST,
		GeometricPrimitive::FaceWinding::GE_FW_CLOCKWISE,
		GeometricPrimitive::PolygonMode::GE_PM_FILL);

	room.SetVertexBuffer(&vbRoom);
	room.SetIndexBuffer(&ibRoom);

	//TODO - improve asset paths
	Shader vsRoom(std::string() + GE_ASSET_PATH + "shaders/skybox.vert");
	Shader fsRoom(std::string() + GE_ASSET_PATH + "shaders/skybox.frag");

	TextureCubeMap textureRoom(std::string() + GE_ASSET_PATH + "textures/cubemap_yokohama_rgba.ktx2");

	GeometryNode roomNode;
	roomNode.AttachGeometry(&room);
	roomNode.GetComponent<VisualComponent>()->AddShader(&vsRoom);
	roomNode.GetComponent<VisualComponent>()->AddShader(&fsRoom);
	roomNode.GetComponent<VisualComponent>()->AddTexture(&textureRoom, Shader::ShaderStage::GE_SS_FRAGMENT);
	
	roomNode.SetModelMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(100.0f)));

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
	};

	// Winding default:
	// GL - CCW
	// Vulkan - CW

	// face widing - counter clock-wise
	std::vector<uint32_t> indexDataCube
	{
	#ifdef FLIPY
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
};


	VertexFormat vfCube(3, 0, 0, 0, 2); //P3 UV2
	VertexBuffer vbCube(&vfCube, Buffer::BufferUsage::GE_BU_STATIC, VertexBuffer::VertexInputRate::GE_VIR_VERTEX, vertexDataCube.data(), vertexDataCube.size() * sizeof(float32_t));
	IndexBuffer ibCube(Buffer::BufferUsage::GE_BU_STATIC, IndexBuffer::IndexType::GE_IT_UINT32, indexDataCube.data(), indexDataCube.size() * sizeof(uint32_t));

	// GL - CCW winding
	GeometricPrimitive cube(GeometricPrimitive::PrimitiveTopology::GE_PT_TRIANGLE_LIST,
		GeometricPrimitive::FaceWinding::GE_FW_COUNTER_CLOCKWISE,
		GeometricPrimitive::PolygonMode::GE_PM_FILL);

	cube.SetVertexBuffer(&vbCube);
	cube.SetIndexBuffer(&ibCube);

	//TODO - improve asset paths
	Shader vsCube(std::string() + GE_ASSET_PATH + "shaders/cube.vert");
	Shader fsCube(std::string() + GE_ASSET_PATH + "shaders/cube.frag");

	Texture2D textureCube(std::string() + GE_ASSET_PATH + "textures/vulkan_11_rgba.ktx2");

	//TODO - add check support for all enabled features (physical device)

	//TODO - enable anisotropic filtering - via physical device enabled features
	GeometryNode cubeNode;
	cubeNode.AttachGeometry(&cube);
	cubeNode.GetComponent<VisualComponent>()->AddShader(&vsCube);
	cubeNode.GetComponent<VisualComponent>()->AddShader(&fsCube);
	cubeNode.GetComponent<VisualComponent>()->AddTexture(&textureCube, Shader::ShaderStage::GE_SS_FRAGMENT);

	/////////////////////////////////////////// SETUP Multi Texture (Array) Cube

	// GL - CCW winding
	GeometricPrimitive cubeTextureArray(GeometricPrimitive::PrimitiveTopology::GE_PT_TRIANGLE_LIST,
		GeometricPrimitive::FaceWinding::GE_FW_COUNTER_CLOCKWISE,
		GeometricPrimitive::PolygonMode::GE_PM_FILL);

	cubeTextureArray.SetVertexBuffer(&vbCube);
	cubeTextureArray.SetIndexBuffer(&ibCube);

	//TODO - improve asset paths
	Shader vsCubeTextureArray(std::string() + GE_ASSET_PATH + "shaders/cubeTextureArray.vert");
	Shader fsCubeTextureArray(std::string() + GE_ASSET_PATH + "shaders/cubeTextureArray.frag");

	Texture2DArray textureArrayCube(std::string() + GE_ASSET_PATH + "textures/texturearray_rgba.ktx2");

	//TODO - add check support for all enabled features (physical device)

	//TODO - enable anisotropic filtering - via physical device enabled features
	GeometryNode cubeTextureArrayNode;
	cubeTextureArrayNode.AttachGeometry(&cubeTextureArray);
	cubeTextureArrayNode.GetComponent<VisualComponent>()->AddShader(&vsCubeTextureArray);
	cubeTextureArrayNode.GetComponent<VisualComponent>()->AddShader(&fsCubeTextureArray);
	cubeTextureArrayNode.GetComponent<VisualComponent>()->AddTexture(&textureArrayCube, Shader::ShaderStage::GE_SS_FRAGMENT);

	cubeTextureArrayNode.GetComponent<VisualComponent>()->GetUniformBuffer(Shader::ShaderStage::GE_SS_VERTEX)->AddUniform(GLSLShaderTypes::UniformType::GE_UT_CRR_TIME);

	cubeTextureArrayNode.SetModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, 0.0f)));

	/////////////////////////////

	// camera- TODO - to be moved as cull camera as part of the scene graph
	app.GetGraphicsSystem()->GetMainCamera()->SetPosition(glm::vec3(0.0f, 0.0f, 15.0f));
	app.GetGraphicsSystem()->GetMainCamera()->SetFOV(60.0f);
	app.GetGraphicsSystem()->GetMainCamera()->SetZNear(0.1f);
	app.GetGraphicsSystem()->GetMainCamera()->SetZFar(1000.0f);
	app.GetGraphicsSystem()->GetMainCamera()->EnableConstraints();

	////////////////////////////////////
	GroupNode scene;
	scene.AttachNode(&roomNode);
	scene.AttachNode(&cubeNode);
	scene.AttachNode(&cubeTextureArrayNode);

	app.GetGraphicsSystem()->SetScene(&scene);

	//TODO - improve startup loading time (code optimization, offload work to worker thread)
	auto endTime = std::chrono::system_clock::now();
	std::chrono::duration<float32_t> diffTime = endTime - startTime;
	LOG_INFO("Startup loading time: %f", diffTime.count());

	app.Run();

	app.Terminate();

	return 0;
}