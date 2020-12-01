#include "Shader.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


Shader::Shader()
	: mPath()
	, mType(Shader::Type::T_COUNT)
{}

Shader::Shader(Shader::Type type, const char_t* pPath)
	: mPath(pPath)
	, mType(type)
{
	Create(pPath);
}

Shader::~Shader()
{
	Destroy();
}

void Shader::Create(const char_t* pPath)
{
	//stub
}

void Shader::Destroy()
{
	mType = Shader::Type::T_COUNT;
	mPath.clear();
}

const Shader::Type& Shader::GetType() const
{
	return mType;
}

const std::string& Shader::GetPath() const
{
	return mPath;
}