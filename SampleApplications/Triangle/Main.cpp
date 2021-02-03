#include <GraphicsEngine.hpp>
#include <vector>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

int main()
{
	/*
	material -> shaders -> pipeline shader modules
	material -> textures -> texture descriptor -> pipeline layout
	uniform buffer -> uniform descriptor -> pipeline layout

	geometry -> geometric primitive -> vertex buffer
									-> index buffer
									-> pipeline vertex input
									-> pipeline input assembly

	tesselation ???
	window size -> view state -> pipeline viewport/scissors

	geometry -> geometric primitive -> vertex winding -> pipeline rasterization
	depth state -> pipeline rasterization
	cull state -> pipeline rasterization
	polygon state -> pipeline rasterization

	multisample state -> pipleine multisample

	depth stencil state -> pipeline depth & stencil

	alpha/color blend state -> pipleine color blend

	dynamic state -> pipeline dynamic


	*/

	/*Pipeline pipeline(....)
		type: graphics, compute, raytracing, etc.
		States:
			shaders
			vertex input
			input assembly
			tesselation
			viewport & scissors
			rasterization
			multisample
			depth & stencil
			color blending
			dynamic*/

	///////////////////////////////////////////////////

	//TODO All objects should be dynamically allocated and cached in a CAtalog/ResourceManager for memory management
	WindowApplication app;
	
	// TODO - remove hardcoded window size
	app.Init("Triangle", 1920, 1080);

	// geometry setup
	std::vector<float32_t> vertexData =
	// coordinates defined in clock-wise winding
	// object space position, color
	{ 1.0f, -1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
	 -1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
	  0.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f
	};

	// Vulkan - CW wwinding
	std::vector<uint32_t> indexData = { 0, 1, 2 };

	VertexFormat vFormat(3, 0, 0, 3, 0); //P3 C3

	VertexBuffer vb(&vFormat, Buffer::BufferUsage::GE_BU_STATIC, VertexBuffer::VertexInputRate::GE_VIR_VERTEX, vertexData.data(), vertexData.size() * sizeof(float32_t));
	IndexBuffer ib(Buffer::BufferUsage::GE_BU_STATIC, IndexBuffer::IndexType::GE_IT_UINT32, indexData.data(), indexData.size() * sizeof(uint32_t));

	GeometricPrimitive geo(GeometricPrimitive::PrimitiveTopology::GE_PT_TRIANGLE_LIST,
		GeometricPrimitive::FaceWinding::GE_FW_CLOCKWISE,
		GeometricPrimitive::PolygonMode::GE_PM_FILL);

	geo.SetVertexBuffer(&vb);
	geo.SetIndexBuffer(&ib);

	//TODO - improve asset paths
	Shader vertexShader(std::string() + GE_ASSET_PATH + "shaders/triangle.vert");
	Shader fragmentShader(std::string() + GE_ASSET_PATH + "shaders/triangle.frag");

	GeometryNode geoNode;
	geoNode.AttachGeometry(&geo);
	geoNode.GetComponent<VisualComponent>()->AddShader(&vertexShader);
	geoNode.GetComponent<VisualComponent>()->AddShader(&fragmentShader);
	
	/////////////////////////////

	// camera- TODO - to be moved as cull camera as part of the scene graph
	app.GetGraphicsSystem()->GetMainCamera()->SetPosition(glm::vec3(0.0f, 0.0f, 2.5f));
	app.GetGraphicsSystem()->GetMainCamera()->SetFOV(60.0f);
	app.GetGraphicsSystem()->GetMainCamera()->SetZNear(0.1f);
	app.GetGraphicsSystem()->GetMainCamera()->SetZFar(1000.0f);

	////////////////////////////////////
	GroupNode scene;
	scene.AttachNode(&geoNode);

	app.GetGraphicsSystem()->SetScene(&scene);

	app.Run();

	app.Terminate();

	return 0;
}