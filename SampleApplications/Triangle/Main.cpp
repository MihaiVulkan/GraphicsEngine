#include <GraphicsEngine.hpp>
#include <vector>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

int main()
{
	auto app = GE_ALLOC(WindowApplication)("Triangle", 1920, 1080);

	// geometry setup
	std::vector<float32_t> vertexData =
	// coordinates defined in clock-wise winding
	// object space position, color
	{ 1.0f, -1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
	 -1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
	  0.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f
	};

	// Vulkan - CW winding
	std::vector<uint32_t> indexData = { 0, 1, 2 };

	auto vFormat = GE_ALLOC(VertexFormat)(3, 0, 0, 3, 0); //P3 C3

	auto vb = GE_ALLOC(VertexBuffer)(vFormat, Buffer::BufferUsage::GE_BU_STATIC, VertexBuffer::VertexInputRate::GE_VIR_VERTEX, vertexData.data(), vertexData.size() * sizeof(float32_t));
	auto ib = GE_ALLOC(IndexBuffer)(Buffer::BufferUsage::GE_BU_STATIC, IndexBuffer::IndexType::GE_IT_UINT32, indexData.data(), indexData.size() * sizeof(uint32_t));

	auto geo = GE_ALLOC(GeometricPrimitive)(GeometricPrimitive::PrimitiveTopology::GE_PT_TRIANGLE_LIST,
		GeometricPrimitive::FaceWinding::GE_FW_CLOCKWISE,
		GeometricPrimitive::PolygonMode::GE_PM_FILL);

	geo->SetVertexBuffer(vb);
	geo->SetIndexBuffer(ib);

	//TODO - improve asset paths
	auto vertexShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/triangle.vert");
	auto fragmentShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/triangle.frag");

	auto geoNode = GE_ALLOC(GeometryNode);
	geoNode->AttachGeometry(geo);
	geoNode->GetComponent<VisualComponent>()->AddShader(vertexShader);
	geoNode->GetComponent<VisualComponent>()->AddShader(fragmentShader);
	
	/////////////////////////////

	// camera- TODO - to be moved as cull camera as part of the scene graph
	app->GetGraphicsSystem()->GetMainCamera()->SetPosition(glm::vec3(0.0f, 0.0f, 2.5f));
	app->GetGraphicsSystem()->GetMainCamera()->SetFOV(60.0f);
	app->GetGraphicsSystem()->GetMainCamera()->SetZNear(0.1f);
	app->GetGraphicsSystem()->GetMainCamera()->SetZFar(1000.0f);

	////////////////////////////////////
	auto scene = GE_ALLOC(GroupNode)();
	scene->AttachNode(geoNode);

	app->GetGraphicsSystem()->SetScene(scene);

	app->Run();

	GE_FREE(app);
	//The rest of heap memory is cleared automatically
	
	return 0;
}