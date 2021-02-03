#ifndef GRAPHICS_SHADER_TOOLS_GLSL_GLSL_SHADER_PARSER_HPP
#define GRAPHICS_SHADER_TOOLS_GLSL_GLSL_SHADER_PARSER_HPP

#include "Foundation/Object.hpp"
#include "Graphics/Rendering/Resources/VertexFormat.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			A simple GLSL shader parser, for now limited to only retrieve input, outputs, uniform and uniform block info per shader.
		*/
		class GLSLShaderParser: public Object
		{
			GE_RTTI(GraphicsEngine::Graphics::GLSLShaderParser)

		public:
			struct Input
			{
				std::string type;
				int32_t location;

				bool_t IsValid() const
				{
					bool_t res = (type.empty() != true) && (location != -1);
					return res;
				}
			};

			struct VertexInput
			{
				std::string name;
				Input* pInput;

				bool_t IsValid() const
				{
					bool_t res = (name.empty() != true) && pInput && pInput->IsValid();
					return res;
				}
			};

			struct Output
			{
				std::string type;
				int32_t location;

				bool_t IsValid() const
				{ 
					bool_t res = (type.empty() != true) && (location != -1);
					return res;
				}
			};

			struct Uniform
			{
				std::string type;
				uint32_t binding;
				int32_t location;

				bool_t IsValid() const
				{ 
					bool_t res = (type.empty() != true) && (location != -1);
					return res;
				}
			};

			struct UniformMember
			{
				std::string type;

				bool_t IsValid() const 
				{
					bool_t res = type.empty() != true;
					return res;
				}
			};

			struct UniformBlock
			{
				std::string name;
				int32_t binding;
				std::unordered_map<std::string, UniformMember> members;

				bool_t IsValid() const
				{
					bool_t res = (name.empty() != true) && (binding != -1) && (members.empty() != true);
					return res;
				}
			};

			typedef std::unordered_map<std::string, Input> InputMap;
			typedef std::unordered_map<std::string, Output> OutputMap;
			typedef std::unordered_map<std::string, Uniform> UniformMap;
			// NOTE! ordered_map as we want to maitain the order of attributes!
			typedef std::map<VertexFormat::VertexAttribute, VertexInput> VertexAttributeMap;
			
			GLSLShaderParser();
			explicit GLSLShaderParser(const std::string& shaderSourcePath);
			virtual ~GLSLShaderParser();

			bool_t Parse(const std::string& shaderSourcePath);

			Shader::ShaderStage GetStage() const;
			const std::string& GetVersion() const;

			const GLSLShaderParser::InputMap& GetInputs() const;
			const GLSLShaderParser::VertexAttributeMap& GetVertexAttributes() const;
			const GLSLShaderParser::OutputMap& GetOutputs() const;
			const GLSLShaderParser::UniformMap& GetUniforms() const;
			const GLSLShaderParser::UniformBlock& GetUniformBlock() const;

			/*const GLSLShaderParser::Input& GetInput(const std::string& name) const;
			const GLSLShaderParser::Output& GetOutput(const std::string& name) const;
			const GLSLShaderParser::Uniform& GetUniform(const std::string& name) const;*/

		private:
			NO_COPY_NO_MOVE(GLSLShaderParser)

			bool_t ParseExtension(const std::string& shaderSourcePath);
			bool_t ParseSource(const std::string& shaderCode);
			bool_t ParseLayoutInfo(const std::string& crrLayoutInfo);
			bool_t ParseUboData(const std::string& crrUboData);
			bool_t ComputeVertexInputs();

			int32_t StrToInt(const std::string& str);

			Shader::ShaderStage mStage;
			std::string mVersion;


			// maybe use maps for easier and faster access
			InputMap mInputMap;
			VertexAttributeMap mVertexAttributeMap;
			OutputMap mOutputMap;
			UniformMap mUniformMap;

			UniformBlock mUniformBlock; // NOTE! For nwo we consider to have only one UBO per shader
		};
	}
}


#endif // GRAPHICS_SHADER_TOOLS_GLSL_GLSL_SHADER_PARSER_HPP