#ifdef _WIN32
#pragma comment(linker, "/subsystem:console")
//#pragma comment(linker, "/subsystem:windows")
#endif


#include "GraphicsEngine.hpp"
#include <vector>
#include <cassert>

#define NEW_GRAPHICS

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

int main()
//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	WindowApplication app;
	
	app.Init("My Vulkan App", 1920, 1080);

#ifdef NEW_GRAPHICS
	////////////////////////////////////

	// geometry setup
	std::vector<bfloat32_t> vertexData =
		// the clip space coordinates are defined as expected by Vulkan viewport - Y points down
		// coordinates defined in clock-wise winding

		// clip space position, color
	{ 0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	  1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	 -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	std::vector<uint32_t> indexData = { 0, 1, 2 };

	VertexFormat vFormat(3, 0, 0, 3, 0); //P3 C3

	VertexBuffer vb(&vFormat, VertexBuffer::Usage::U_STATIC, VertexBuffer::InputRate::IR_VERTEX, vertexData.data(), vertexData.size() * sizeof(bfloat32_t));
	IndexBuffer ib(IndexBuffer::Usage::U_STATIC, IndexBuffer::IndexType::IT_UINT32, indexData.data(), indexData.size() * sizeof(uint32_t));

	GeometricPrimitive geo(GeometricPrimitive::PrimitiveTopology::PT_TRIANGLE_LIST);
	geo.SetVertexBuffer(&vb);
	geo.SetIndexBuffer(&ib);

	// material setup
	Material mat;

	Shader vertexShader(Shader::Type::T_VERTEX, "../../SampleApplications/Triangle/res/shaders/triangle.vert");
	Shader fragmentShader(Shader::Type::T_FRAGMENT, "../../SampleApplications/Triangle/res/shaders/triangle.frag");

	//Textures
	//Materials

	std::vector<Shader*> shaders(2);
	shaders[0] = &vertexShader;
	shaders[1] = &fragmentShader;

	// shader bindings - uniforms
	UniformBuffer ub(UniformBuffer::UniformUsage::UU_PVM);


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


	// Set Camera config

	mat.SetShaders(shaders);
	mat.SetUniformBuffer(&ub);

	MaterialComponent matComp;
	matComp.SetMaterial(&mat);

	GeometryNode geoNode;
	geoNode.AttachGeometry(&geo);

	geoNode.AttachComponent(&matComp);
	geoNode.GetComponent< MaterialComponent >()->SetMaterial(&mat);

	////////////////////////////////////
	app.GetGraphicsSystem()->SetScene(&geoNode);
#endif

	app.Run();

	app.Terminate();

	return 0;
}