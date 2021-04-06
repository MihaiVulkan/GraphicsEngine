#include "Graphics/ShaderTools/GLSL/GLSLShaderParser.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderTypes.hpp"
#include "Foundation/FileUtils.hpp"
#include "Foundation/Logger.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

// glsl shader stages
constexpr const char_t* VERTEX_NAME = "vert";
constexpr const char_t* GEOMETRY_NAME = "geom";
constexpr const char_t* TESS_CONTROL_NAME = "tesc";
constexpr const char_t* TESS_EVAL_NAME = "tese";
constexpr const char_t* FRAGMENT_NAME = "frag";
constexpr const char_t* COMPUTE_NAME = "comp";

// Specific glsl shader keywords
constexpr const char_t* LAYOUT_NAME = "layout";
constexpr const size_t LAYOUT_NAME_SIZE = 6;

constexpr const char_t* INPUT_NAME = "in";
constexpr const char_t* OUTPUT_NAME = "out";
constexpr const char_t* UNIFORM_NAME = "uniform";
constexpr const char_t* LOCATION_NAME = "location";
constexpr const char_t* SET_NAME = "set";
constexpr const char_t* BINDING_NAME = "binding";
constexpr const char_t* VERSION_NAME = "#version";

// chars to remove
constexpr const char_t LAYOUT_START_NAME = '(';
constexpr const char_t LAYOUT_END_NAME = ')';
constexpr const char_t ASSIGN_NAME = '=';
constexpr const char_t TAB_NAME = '\t';
constexpr const char_t CR_NAME = '\r';
constexpr const char_t LF_NAME = '\n';
constexpr const size_t UNWANTED_CHARS_COUNT = 6;

// Primitive types
constexpr const char_t* BOOL_NAME = "bool";
constexpr const char_t* INT_NAME = "int";
constexpr const char_t* FLOAT_NAME = "float";
constexpr const char_t* DOUBLE_NAME = "double";
constexpr const char_t* ARRAY_NAME = "[]";

// vectors & matrices
constexpr const char_t* VEC_NAME = "vec";
constexpr const char_t* MAT_NAME = "mat";

// Uniform buffers
constexpr const char_t* UBO_NAME = "UBO";

// images
constexpr const char_t* SAMPLER_NAME = "sampler";
constexpr const char_t* IMAGE_NAME = "image";

constexpr const char_t* SINGLE_COMMENT_NAME = "//";

constexpr const char_t DELIMITER_EXTENSION_NAME = '.';
constexpr const char_t* DELIMITER_LINE_NAME = "\r\n";
constexpr const char_t DELIMITER_TOKEN_NAME = ' ';
constexpr const char_t DELIMITER_EXP_NAME = ';';
constexpr const char_t DELIMITER_UBO_START_NAME = '{';
constexpr const char_t DELIMITER_UBO_END_NAME = '}';


GLSLShaderParser::GLSLShaderParser()
	: mStage(Shader::ShaderStage::GE_SS_COUNT)
	, mVersion()
{}

GLSLShaderParser::GLSLShaderParser(const std::string& shaderSourcePath)
	: GLSLShaderParser()
{
	// To know the shader stage bases on file extension ASAP!
	if (ParseExtension(shaderSourcePath) == false)
	{
		LOG_ERROR("Error in shader: %s", shaderSourcePath.c_str());
	}
}

GLSLShaderParser::~GLSLShaderParser()
{
	mStage = Shader::ShaderStage::GE_SS_COUNT;
	mVersion = {};
	mUniformBlock = {};

	mInputMap.clear();
	mVertexAttributeMap.clear();
	mOutputMap.clear();
	mUniformMap.clear();
}

bool_t GLSLShaderParser::Parse(const std::string& shaderSourcePath)
{
	assert(shaderSourcePath.empty() == false);

	if (mStage == Shader::ShaderStage::GE_SS_COUNT)
	{
		if (ParseExtension(shaderSourcePath) == false)
		{
			LOG_ERROR("Error in shader: %s", shaderSourcePath.c_str());
			return false;
		}
	}

	std::string shaderCode;
	FileUtils::ReadTextFile(shaderSourcePath, shaderCode);
	if (shaderCode.empty())
	{
		LOG_ERROR("Failed to read shader: %s", shaderSourcePath.c_str());
		return false;
	}

	// NOTE! First we extract the shader glsl version
	// then we pasrse by the 'layout' keyword instead of new line to get the layout info
	// this approach helps us we can have uniform blocks defined on multiple lines

	if (ParseSource(shaderCode) == false)
	{
		LOG_ERROR("Error in shader: %s", shaderSourcePath.c_str());
		return false;
	}

	return true;
}

bool_t GLSLShaderParser::ParseExtension(const std::string& shaderSourcePath)
{
	size_t iter = shaderSourcePath.find_last_of(DELIMITER_EXTENSION_NAME);
	assert(iter != std::string::npos);

	std::string extension = shaderSourcePath.substr(iter + 1);
	if (extension.empty())
		return false;

	if (extension == VERTEX_NAME)
	{
		mStage = Shader::ShaderStage::GE_SS_VERTEX;
	}
	else if (extension == GEOMETRY_NAME)
	{
		mStage = Shader::ShaderStage::GE_SS_GEOMETRY;
	}
	else if (extension == TESS_CONTROL_NAME)
	{
		mStage = Shader::ShaderStage::GE_SS_TESSELATION_CONTROL;
	}
	else if (extension == TESS_EVAL_NAME)
	{
		mStage = Shader::ShaderStage::GE_SS_TESSELATION_EVALUATION;
	}
	else if (extension == FRAGMENT_NAME)
	{
		mStage = Shader::ShaderStage::GE_SS_FRAGMENT;
	}
	else if (extension == COMPUTE_NAME)
	{
		mStage = Shader::ShaderStage::GE_SS_COMPUTE;
	}
	else
	{
		LOG_ERROR("Invalid shader file extension!");
		return false;
	}

	return true;
}

bool_t GLSLShaderParser::ParseSource(const std::string& shaderCode)
{
	if (shaderCode.empty())
		return false;

	// parse version
	size_t lineIter = shaderCode.find(DELIMITER_LINE_NAME);
	std::string versionInfo = shaderCode.substr(0, lineIter);
	if (versionInfo.empty())
		return false;

	size_t tokenIter = 0;
	tokenIter = versionInfo.find_first_of(DELIMITER_TOKEN_NAME);
	std::string token = versionInfo.substr(0, tokenIter);
	if (token.empty())
		return false;

	if (token != VERSION_NAME)
	{
		LOG_ERROR("#version not found! Abort!");
		return false;
	}

	std::string tempVersionInfo = versionInfo.substr(tokenIter, versionInfo.length());
	if (tempVersionInfo.empty())
		return false;

	// remove unwanted spaces (DELIMITER_TOKEN_NAME)
	tempVersionInfo.erase(std::remove(tempVersionInfo.begin(), tempVersionInfo.end(), DELIMITER_TOKEN_NAME), tempVersionInfo.end());

	mVersion = tempVersionInfo;

	////////////////////////

	size_t crrLayoutIter = 0, nextLayoutIter = 0;

	// remove comments
	std::string tempShaderCode(shaderCode);

	while (true)
	{
		crrLayoutIter = tempShaderCode.find(SINGLE_COMMENT_NAME);
		if (crrLayoutIter == std::string::npos)
			break;

		nextLayoutIter = tempShaderCode.find(DELIMITER_LINE_NAME, crrLayoutIter);

		tempShaderCode.erase(crrLayoutIter, nextLayoutIter - crrLayoutIter);
	}

	// parse layout info
	crrLayoutIter = lineIter, nextLayoutIter = 0;
	while(true)
	{
		nextLayoutIter = tempShaderCode.find(LAYOUT_NAME, crrLayoutIter);
		std::string tempLayoutInfo = tempShaderCode.substr(crrLayoutIter, nextLayoutIter - crrLayoutIter);

		crrLayoutIter = nextLayoutIter + LAYOUT_NAME_SIZE + 1; //layout info without the layout taken itself
		if (tempLayoutInfo.empty())
			return false;

		if ((tempLayoutInfo.length() < LAYOUT_NAME_SIZE) 
			|| (tempLayoutInfo[0] == DELIMITER_LINE_NAME[0])
			|| (tempLayoutInfo[1] == DELIMITER_LINE_NAME[1])) //doesn't contain valid layout info
		{
			continue;
		}

		std::string crrLayoutInfo;
		if (nextLayoutIter == std::string::npos) //last layout info
		{
			crrLayoutIter = tempLayoutInfo.find(UBO_NAME);
			if (crrLayoutIter != std::string::npos) // in case of UBO
			{
				crrLayoutIter = tempLayoutInfo.find(DELIMITER_UBO_END_NAME);
				size_t eosIter = tempLayoutInfo.find(DELIMITER_EXP_NAME, crrLayoutIter);

				crrLayoutInfo = tempLayoutInfo.substr(0, eosIter);
			}
			else
			{
				size_t eosIter = tempLayoutInfo.find(DELIMITER_LINE_NAME);
				crrLayoutInfo = tempLayoutInfo.substr(0, eosIter);
			}
		}
		else
		{
			crrLayoutInfo = tempLayoutInfo;
		}

		if (ParseLayoutInfo(crrLayoutInfo) == false)
		{
			LOG_ERROR("Failed to parse layout info: %s", crrLayoutInfo.c_str());
			return false;
		}

		if (nextLayoutIter == std::string::npos) //exit
			break;
	}

	if ((mStage == Shader::ShaderStage::GE_SS_VERTEX) && ComputeVertexInputs() == false)
	{
		LOG_ERROR("Failed to compute the vertex input data");
		return false;
	}

	return true;
}

bool_t GLSLShaderParser::ParseLayoutInfo(const std::string& crrLayoutInfo)
{
	if (crrLayoutInfo.empty())
		return false;

	std::string layoutInfo(crrLayoutInfo);

	// replace all duplicate DELIMITER_TOKEN_NAME (whitespace) with ASSIGN_NAME
	for (size_t i = 0; i < layoutInfo.size(); ++i)
	{
		if (layoutInfo[i] == DELIMITER_TOKEN_NAME)
		{
			size_t prevIdx = i - 1;
			if (prevIdx >= 0 && prevIdx < crrLayoutInfo.size())
			{
				if (layoutInfo[prevIdx] == DELIMITER_TOKEN_NAME ||
					layoutInfo[prevIdx] == ASSIGN_NAME)
					layoutInfo[i] = ASSIGN_NAME;
			}
		}
	}

	// remove unwanted chars
	const char_t unwantedChars[] = { LAYOUT_START_NAME, LAYOUT_END_NAME, ASSIGN_NAME, TAB_NAME, CR_NAME, LF_NAME };

	for (size_t i = 0; i < UNWANTED_CHARS_COUNT; ++i)
	{
		layoutInfo.erase(std::remove(layoutInfo.begin(), layoutInfo.end(), unwantedChars[i]), layoutInfo.end());
	}

	///////////////////////////

	std::string qualifierName_1, qualifierValue_1, qualifierName_2, qualifierValue_2, varDefinition, varType, varName;

	size_t crrTokenIter = 0, nextTokenIter = 0;

	nextTokenIter = layoutInfo.find_first_of(DELIMITER_TOKEN_NAME, crrTokenIter);
	qualifierName_1 = layoutInfo.substr(crrTokenIter, nextTokenIter - crrTokenIter);
	crrTokenIter = nextTokenIter + 1;
	if (qualifierName_1.empty())
		return false;

	nextTokenIter = layoutInfo.find_first_of(DELIMITER_TOKEN_NAME, crrTokenIter);
	qualifierValue_1 = layoutInfo.substr(crrTokenIter, nextTokenIter - crrTokenIter);
	crrTokenIter = nextTokenIter + 1;
	if (qualifierValue_1.empty())
		return false;

	if (qualifierName_1 == SET_NAME) //then binding should be qualifier2
	{
		nextTokenIter = layoutInfo.find_first_of(DELIMITER_TOKEN_NAME, crrTokenIter);
		qualifierName_2 = layoutInfo.substr(crrTokenIter, nextTokenIter - crrTokenIter);
		crrTokenIter = nextTokenIter + 1;
		if (qualifierName_2.empty())
			return false;

		nextTokenIter = layoutInfo.find_first_of(DELIMITER_TOKEN_NAME, crrTokenIter);
		qualifierValue_2 = layoutInfo.substr(crrTokenIter, nextTokenIter - crrTokenIter);
		crrTokenIter = nextTokenIter + 1;
		if (qualifierValue_2.empty())
			return false;
	}

	nextTokenIter = layoutInfo.find_first_of(DELIMITER_TOKEN_NAME, crrTokenIter);
	varDefinition = layoutInfo.substr(crrTokenIter, nextTokenIter - crrTokenIter);
	crrTokenIter = nextTokenIter + 1;
	if (varDefinition.empty())
		return false;

	nextTokenIter = layoutInfo.find_first_of(DELIMITER_TOKEN_NAME, crrTokenIter);
	varType = layoutInfo.substr(crrTokenIter, nextTokenIter - crrTokenIter);
	crrTokenIter = nextTokenIter + 1;
	if (varType.empty())
		return false;

	// Uniform Buffer
	if (varType == UBO_NAME && 
		qualifierName_1 == SET_NAME && qualifierName_2 == BINDING_NAME && varDefinition == UNIFORM_NAME)
	{
		nextTokenIter = layoutInfo.find_first_of(DELIMITER_UBO_START_NAME, crrTokenIter);
		crrTokenIter = nextTokenIter + 1;

		nextTokenIter = layoutInfo.find_last_of(DELIMITER_UBO_END_NAME);

		std::string uboData = layoutInfo.substr(crrTokenIter, nextTokenIter - crrTokenIter);
		crrTokenIter = nextTokenIter + 2;
		if (uboData.empty())
			return false;

		nextTokenIter = layoutInfo.find_last_of(DELIMITER_EXP_NAME);
		varName = layoutInfo.substr(crrTokenIter, nextTokenIter - crrTokenIter);
		if (varName.empty())
			return false;

		//////////////////
		mUniformBlock.name = varName;
		mUniformBlock.setId = StrToInt(qualifierValue_1);
		mUniformBlock.binding = StrToInt(qualifierValue_2);

		if (ParseUboData(uboData) == false)
			return false;
	}
	else
	{
		nextTokenIter = layoutInfo.find_first_of(DELIMITER_EXP_NAME, crrTokenIter);
		varName = layoutInfo.substr(crrTokenIter, nextTokenIter - crrTokenIter);
		crrTokenIter = nextTokenIter + 1;
		if (varName.empty())
			return false;
	}
	////////////////////////////
	int32_t intQualifierVal = StrToInt(qualifierValue_1);

	// Inputs
	if (varDefinition == INPUT_NAME && qualifierName_1 == LOCATION_NAME)
	{
		Input input{};
		input.type = varType;
		input.location = intQualifierVal;

		mInputMap[varName] = input;
	}

	// Outputs
	if (varDefinition == OUTPUT_NAME && qualifierName_1 == LOCATION_NAME)
	{
		Output output{};
		output.type = varType;
		output.location = intQualifierVal;

		mOutputMap[varName] = output;
	}

	// Uniforms
	if (varType != UBO_NAME && qualifierName_1 == SET_NAME && varDefinition == UNIFORM_NAME && qualifierName_2 == BINDING_NAME)
	{
		Uniform uniform{};;
		uniform.type = varType;
		uniform.setId = intQualifierVal;
		uniform.binding = StrToInt(qualifierValue_2);
		uniform.location = uniform.binding; //TODO - clarify this

		mUniformMap[varName] = uniform;
	}

	return true;
}

bool_t GLSLShaderParser::ParseUboData(const std::string& crrUboData)
{
	if (crrUboData.empty())
		return false;

	std::string uboData(crrUboData);

	// replace all unneeded white spaces (DELIMITER_TOKEN_NAME) with ASSIGN_NAME
	for (size_t i = 0; i < uboData.size(); ++i)
	{
		if (uboData[i] == DELIMITER_TOKEN_NAME)
		{
			size_t prevIdx = i - 1, nextIdx = i + 1;
			if (prevIdx >= 0 && prevIdx < uboData.size())
			{
				if (uboData[prevIdx] == DELIMITER_EXP_NAME)
					uboData[i] = ASSIGN_NAME;
			}
			if (nextIdx > 0 && nextIdx < uboData.size())
			{
				if (uboData[nextIdx] == DELIMITER_EXP_NAME)
					uboData[i] = ASSIGN_NAME;
			}
		}
	}

	// remove unwanted char - ASSIGN NAME
	uboData.erase(std::remove(uboData.begin(), uboData.end(), ASSIGN_NAME), uboData.end());

	size_t crrTokenIter = 0, nextTokenIter = 0;
	while (true)
	{
		nextTokenIter = uboData.find_first_of(DELIMITER_TOKEN_NAME, crrTokenIter);
		if (nextTokenIter == std::string::npos)
			break;

		UniformMember data;

		data.type = uboData.substr(crrTokenIter, nextTokenIter - crrTokenIter);
		crrTokenIter = nextTokenIter + 1;
		if (data.type.empty())
			return false;

		// we replace the delimiters for already processed data to avoid this data
		assert(nextTokenIter != std::string::npos);
		uboData[nextTokenIter] = ASSIGN_NAME;

		nextTokenIter = uboData.find_first_of(DELIMITER_EXP_NAME, crrTokenIter);
		std::string name = uboData.substr(crrTokenIter, nextTokenIter - crrTokenIter);
		crrTokenIter = nextTokenIter + 1;
		if (name.empty())
			return false;

		assert(nextTokenIter != std::string::npos);
		uboData[nextTokenIter] = ASSIGN_NAME;

		mUniformBlock.members[name] = data;
	}

	return true;
}

bool_t GLSLShaderParser::ComputeVertexInputs()
{
	if (mInputMap.empty())
		return false;

	// compute vertex attribute data
	for (auto iter = mInputMap.begin(); iter != mInputMap.end(); ++ iter)
	{
		VertexInput attributeData{};
		attributeData.name = iter->first;
		attributeData.pInput = &(iter->second);

		if (attributeData.name == GLSLShaderTypes::Constants::VERTEX_POSITION)
		{
			mVertexAttributeMap[VertexFormat::VertexAttribute::GE_VA_POSITION] = attributeData;
		}
		else if (attributeData.name == GLSLShaderTypes::Constants::VERTEX_NORMAL)
		{
			mVertexAttributeMap[VertexFormat::VertexAttribute::GE_VA_NORMAL] = attributeData;
		}
		else if (attributeData.name == GLSLShaderTypes::Constants::VERTEX_TANGET)
		{
			mVertexAttributeMap[VertexFormat::VertexAttribute::GE_VA_TANGENT] = attributeData;
		}
		else if (attributeData.name == GLSLShaderTypes::Constants::VERTEX_COLOR)
		{
			mVertexAttributeMap[VertexFormat::VertexAttribute::GE_VA_COLOR] = attributeData;
		}
		else if (attributeData.name == GLSLShaderTypes::Constants::VERTEX_TEX_COORDS)
		{
			mVertexAttributeMap[VertexFormat::VertexAttribute::GE_VA_TEXTURE_COORD] = attributeData;
		}
		else
		{
			LOG_ERROR("Invalid attribute name!");
			return false;
		}
	}

	return true;
}

int32_t GLSLShaderParser::StrToInt(const std::string& str)
{
	int32_t res = -1;
	try
	{
		res = std::stoi(str);
	}
	catch (...)
	{
		LOG_ERROR("%s failed to be converted to int!", str.c_str());
	}

	return res;
}

Shader::ShaderStage GLSLShaderParser::GetStage() const
{
	return mStage;
}

const std::string& GLSLShaderParser::GetVersion() const
{
	return mVersion;
}

const GLSLShaderParser::InputMap& GLSLShaderParser::GetInputs() const
{
	return mInputMap;
}

const GLSLShaderParser::VertexAttributeMap& GLSLShaderParser::GetVertexAttributes() const
{
	return mVertexAttributeMap;
}

const GLSLShaderParser::OutputMap& GLSLShaderParser::GetOutputs() const
{
	return mOutputMap;
}

const GLSLShaderParser::UniformMap& GLSLShaderParser::GetUniforms() const
{
	return mUniformMap;
}

const GLSLShaderParser::UniformBlock& GLSLShaderParser::GetUniformBlock() const
{
	return mUniformBlock;
}

//const GLSLShaderParser::Input& GLSLShaderParser::GetInput(const std::string& name) const
//{
//	assert(name.empty() == false);
//
//	auto iter = mInputMap.find(name);
//	if (iter != mInputMap.end())
//	{
//		return iter->second;
//	}
//}
//
//const GLSLShaderParser::Output& GLSLShaderParser::GetOutput(const std::string& name) const
//{
//
//}
//
//const GLSLShaderParser::Uniform& GLSLShaderParser::GetUniform(const std::string& name) const
//{
//
//}