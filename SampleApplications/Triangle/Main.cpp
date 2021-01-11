#ifdef _WIN32
#pragma comment(linker, "/subsystem:console")
//#pragma comment(linker, "/subsystem:windows")
#endif


#include "GraphicsEngine.hpp"
#include <vector>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

int main()
//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
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
	app.Init("My Vulkan App", 1920, 1080);

	// geometry setup
	std::vector<float32_t> vertexData =
		// the clip space coordinates are defined as expected by Vulkan viewport - Y points down
		// coordinates defined in clock-wise winding

		// clip space position, color
	{ 0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	  1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	 -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	std::vector<uint32_t> indexData = { 0, 1, 2 };

	VertexFormat vFormat(3, 0, 0, 3, 0); //P3 C3

	VertexBuffer vb(&vFormat, Buffer::BufferUsage::GE_BU_STATIC, VertexBuffer::VertexInputRate::GE_VIR_VERTEX, vertexData.data(), vertexData.size() * sizeof(float32_t));
	IndexBuffer ib(Buffer::BufferUsage::GE_BU_STATIC, IndexBuffer::IndexType::GE_IT_UINT32, indexData.data(), indexData.size() * sizeof(uint32_t));

	GeometricPrimitive geo(GeometricPrimitive::PrimitiveTopology::GE_PT_TRIANGLE_LIST,
		GeometricPrimitive::FaceWinding::GE_FW_CLOCKWISE,
		GeometricPrimitive::PolygonMode::GE_PM_FILL);

	geo.SetVertexBuffer(&vb);
	geo.SetIndexBuffer(&ib);

	// material setup
//	Material mat;

	Shader vertexShader("../../SampleApplications/Triangle/res/shaders/triangle.vert");
	Shader fragmentShader("../../SampleApplications/Triangle/res/shaders/triangle.frag");

	//Textures
	//Materials

	VisualComponent visComp;

	visComp.AddShader(&vertexShader);
	visComp.AddShader(&fragmentShader);

	// add uniform buffer
	UniformBuffer ub;
	ub.AddUniform(GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4);

	visComp.AddUniformBuffer(Shader::ShaderStage::GE_SS_VERTEX, &ub);

	CullFaceState cull_s(true);
	cull_s.SetCullMode(CullFaceState::CullMode::GE_CM_BACK);
	visComp.SetCullFaceState(cull_s);

	DepthStencilState depthStencil_s(true);
	depthStencil_s.SetIsDepthEnabled(true);
	depthStencil_s.SetIsDepthWritable(true);
	depthStencil_s.SetDepthCompareOp(DepthStencilState::CompareOp::GE_CO_LESS_OR_EQUAL);

	depthStencil_s.SetIsStencilEnabled(false);
	visComp.SetDepthStencilState(depthStencil_s);

	ColorBlendState colorBlend_s(true);
	colorBlend_s.SetIsBlendEnabled(false);
	colorBlend_s.SetSrcColorBlendFactor(ColorBlendState::BlendFactor::GE_BF_ZERO);
	colorBlend_s.SetDstColorBlendFactor(ColorBlendState::BlendFactor::GE_BF_ZERO);
	colorBlend_s.SetColorBlendOp(ColorBlendState::BlendOp::GE_BO_ADD);
	colorBlend_s.SetSrcAlphaBlendFactor(ColorBlendState::BlendFactor::GE_BF_ZERO);
	colorBlend_s.SetDstAlphaBlendFactor(ColorBlendState::BlendFactor::GE_BF_ZERO);
	colorBlend_s.SetAlphaBlendOp(ColorBlendState::BlendOp::GE_BO_ADD);
	colorBlend_s.SetColorWriteMask(0x0f);
	visComp.SetColorBlendState(colorBlend_s);

	DynamicState dynamic_s;
	dynamic_s.Add(DynamicState::State::GE_DS_VIEWPORT);
	dynamic_s.Add(DynamicState::State::GE_DS_SCISSOR);
	visComp.SetDynamicState(dynamic_s);

	/////////////////////////////
	GeometryNode geoNode;
	geoNode.AttachGeometry(&geo);

	//MaterialComponent matComp;
	//matComp.SetMaterial(&mat);

	//geoNode.AttachComponent(&matComp);
	//geoNode.GetComponent< MaterialComponent >()->SetMaterial(&mat);

	geoNode.AttachComponent(&visComp);

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