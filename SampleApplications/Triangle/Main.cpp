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
		// coordinates defined in counter-clock wise winding
		// object space position, color
	{
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	   1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	   0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	std::vector<uint32_t> indexData = { 0, 1, 2 };

	auto vFormat = GE_ALLOC(VertexFormat)(3, 0, 0, 3, 0); //P3 C3

	auto vb = GE_ALLOC(VertexBuffer)(vFormat, Buffer::BufferUsage::GE_BU_STATIC,vertexData.data(), vertexData.size() * sizeof(float32_t));
	auto ib = GE_ALLOC(IndexBuffer)(Buffer::BufferUsage::GE_BU_STATIC, IndexBuffer::IndexType::GE_IT_UINT32, indexData.data(), indexData.size() * sizeof(uint32_t));

	auto geo = GE_ALLOC(GeometricPrimitive);

	geo->SetVertexBuffer(vb);
	geo->SetIndexBuffer(ib);

	auto geoNode = GE_ALLOC(GeometryNode)("Triangle");
	geoNode->SetGeometry(geo);
	
	auto visualEffect = GE_ALLOC(UnlitColorAttributeVisualEffect);

	// add the impacted node
	visualEffect->SetTargetNode(geoNode);
	geoNode->GetComponent<VisualComponent>()->SetVisualEffect(visualEffect);

	/////////////////////////////

	// camera- TODO - to be moved as cull camera as part of the scene graph
	app->GetGraphicsSystem()->GetMainCamera()->SetPosition(glm::vec3(0.0f, 0.0f, 2.5f));
	app->GetGraphicsSystem()->GetMainCamera()->SetFOV(60.0f);
	app->GetGraphicsSystem()->GetMainCamera()->SetZNear(0.1f);
	app->GetGraphicsSystem()->GetMainCamera()->SetZFar(100.0f);

	////////////////////////////////////
	auto scene = GE_ALLOC(GroupNode)();
	scene->AttachNode(geoNode);

	app->GetGraphicsSystem()->SetScene(scene);

	app->Run();

	GE_FREE(app);
	//The rest of heap memory is cleared automatically
	
	return 0;
}