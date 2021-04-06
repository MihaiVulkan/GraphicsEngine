
/*
	Implementation based on tiny gltf
	https://github.com/syoyo/tinygltf
*/

#include "Graphics/Loaders/glTF2Loader.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include "glm/common.hpp" //glm::max(), glm::ceil()
#include "glm/mat4x4.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <cstdio>
#include <cassert>

///// tinygltf setup ////////
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "tinygltf/tiny_gltf.h"
/////////////////////

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


struct glTF2Loader::Impl
{
	struct Node;

	/*
		glTF texture
	*/
	struct Texture
	{
		uint32_t width, height;
		uint32_t mipLevels;
		uint32_t layerCount;

		void FromglTfImage(tinygltf::Image& gltfimage, const std::string& filePath);
	};

	/*
		glTF material class
	*/
	struct Material
	{
		enum AlphaMode { ALPHAMODE_OPAQUE, ALPHAMODE_MASK, ALPHAMODE_BLEND };
		AlphaMode alphaMode = ALPHAMODE_OPAQUE;
		float alphaCutoff = 1.0f;
		float metallicFactor = 1.0f;
		float roughnessFactor = 1.0f;
		glm::vec4 baseColorFactor = glm::vec4(1.0f);

		glTF2Loader::Impl::Texture* baseColorTexture = nullptr;
		glTF2Loader::Impl::Texture* metallicRoughnessTexture = nullptr;
		glTF2Loader::Impl::Texture* normalTexture = nullptr;
		glTF2Loader::Impl::Texture* occlusionTexture = nullptr;
		glTF2Loader::Impl::Texture* emissiveTexture = nullptr;

		glTF2Loader::Impl::Texture* specularGlossinessTexture;
		glTF2Loader::Impl::Texture* diffuseTexture;
	};

	struct Dimensions
	{
		glm::vec3 min = glm::vec3(FLT_MAX);
		glm::vec3 max = glm::vec3(-FLT_MAX);
		glm::vec3 size;
		glm::vec3 center;
		float radius;
	} dimensions;

	/*
		glTF primitive
	*/
	struct Primitive
	{
		uint32_t firstIndex;
		uint32_t indexCount;
		uint32_t firstVertex;
		uint32_t vertexCount;
		glTF2Loader::Impl::Material& material;

		Dimensions dimensions;

		Primitive(uint32_t firstIndex, uint32_t indexCount, glTF2Loader::Impl::Material& material) 
			: firstIndex(firstIndex), indexCount(indexCount)
			, firstVertex(0), vertexCount(0)
			, material(material)
		{};

		void setDimensions(glm::vec3 min, glm::vec3 max);
	};

	/*
		glTF mesh
	*/
	struct Mesh
	{
		std::vector<glTF2Loader::Impl::Primitive*> primitives;
		std::string name;

		Mesh();
		~Mesh();
	};

	/*
		glTF node
	*/
	struct Node
	{
		glTF2Loader::Impl::Node* parent;
		uint32_t index;
		std::vector<glTF2Loader::Impl::Node*> children;
		glm::mat4 matrix{ 1.0f };
		std::string name;
		glTF2Loader::Impl::Mesh* mesh;

		glm::vec3 translation{ 0.0f };
		glm::vec3 scale{ 1.0f };
		glm::quat rotation{};

		glm::mat4 localMatrix();
		glm::mat4 getMatrix();
		void update();
		~Node();
	};

	Impl();
	virtual ~Impl();

	bool_t LoadFromFile(const std::string& filePath, glTF2Loader::FileLoadingFlags loadingFlags);

	bool_t LoadImages(tinygltf::Model& gltfModel);
	bool_t LoadMaterials(tinygltf::Model& gltfModel);
	bool_t LoadNode(glTF2Loader::Impl::Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model);

	void ApplyFlagsOnNode(glTF2Loader::Impl::Node* pNode, glTF2Loader::FileLoadingFlags loadingFlags);

	glTF2Loader::Impl::Texture* GetTexture(uint32_t index);

	void Draw(std::function<void(uint32_t indexCount, uint32_t firstVertex)> onDrawCB);
	void DrawNode(glTF2Loader::Impl::Node* node, std::function<void(uint32_t indexCount, uint32_t firstVertex)> onDrawCB);

	std::vector<glTF2Loader::Impl::Node*> mNodes;

	std::vector<glTF2Loader::Impl::Texture> mTextures;
	std::vector<glTF2Loader::Impl::Material> mMaterials;

	std::vector<float32_t> mVertexBuffer;
	std::vector<uint32_t> mIndexBuffer;

	glTF2Loader::VertexAttributes mVertexAttributes;
};

glTF2Loader::Impl::Impl()
{}

glTF2Loader::Impl::~Impl()
{}

static bool_t LoadImageDataFunc(tinygltf::Image* pImage, const int32_t imageIndex, std::string* pError, std::string* pWarning, int32_t req_width, int32_t req_height, const unsigned char* pBytes, int32_t size, void* pUserData)
{
	// KTX files will be handled by our own code
	if (pImage->uri.find_last_of(".") != std::string::npos)
	{
		if (pImage->uri.substr(pImage->uri.find_last_of(".") + 1) == "ktx2")
		{
			return true;
		}
	}

	return tinygltf::LoadImageData(pImage, imageIndex, pError, pWarning, req_width, req_height, pBytes, size, pUserData);
}

bool_t glTF2Loader::Impl::LoadFromFile(const std::string& filePath, glTF2Loader::FileLoadingFlags loadingFlags)
{
	if (filePath.empty())
	{
		LOG_ERROR("Empty file path!");
		return false;
	}

	tinygltf::TinyGLTF gltfContext;
	tinygltf::Model gltfModel;
	std::string error, warning;

	gltfContext.SetImageLoader(::LoadImageDataFunc, nullptr);

	bool_t result = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, filePath);
	if (result == false)
	{
		LOG_ERROR("Failed to load file: %s", filePath.c_str());
		return false;
	}

	result = LoadImages(gltfModel);
	if (result == false)
	{
		LOG_ERROR("Failed to load images");
		return false;
	}

	result = LoadMaterials(gltfModel);
	if (result == false)
	{
		LOG_ERROR("Failed to load materials");
		return false;
	}

	const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
	for (size_t i = 0; i < scene.nodes.size(); i++)
	{
		const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];

		//TODO for now we skip Light and Camera nodes
		if ((node.name == "Camera") || ((node.name == "Light")))
			continue;

		result = LoadNode(nullptr, node, scene.nodes[i], gltfModel);
		if (result == false)
		{
			LOG_ERROR("Failed to load node");
			return false;
		}
	}

	// Pre-Calculations for requested features
	if (loadingFlags & FileLoadingFlags::PreTransformVertices)
	{
		for (Node* node : mNodes)
		{
			ApplyFlagsOnNode(node, loadingFlags);
		}
	}

	return true;
}

bool_t glTF2Loader::Impl::LoadImages(tinygltf::Model& gltfModel)
{
	//TODO

	return true;
}

bool_t glTF2Loader::Impl::LoadMaterials(tinygltf::Model& gltfModel)
{
	for (tinygltf::Material& mat : gltfModel.materials)
	{
		glTF2Loader::Impl::Material material;
		if (mat.values.find("baseColorTexture") != mat.values.end()) {
			material.baseColorTexture = GetTexture(gltfModel.textures[mat.values["baseColorTexture"].TextureIndex()].source);
		}
		// Metallic roughness workflow
		if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
			material.metallicRoughnessTexture = GetTexture(gltfModel.textures[mat.values["metallicRoughnessTexture"].TextureIndex()].source);
		}
		if (mat.values.find("roughnessFactor") != mat.values.end()) {
			material.roughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
		}
		if (mat.values.find("metallicFactor") != mat.values.end()) {
			material.metallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
		}
		if (mat.values.find("baseColorFactor") != mat.values.end()) {
			material.baseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
		}
		if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
			material.normalTexture = GetTexture(gltfModel.textures[mat.additionalValues["normalTexture"].TextureIndex()].source);
		}
		else {
		//	material.normalTexture = &emptyTexture; //TODO
		}
		if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
			material.emissiveTexture = GetTexture(gltfModel.textures[mat.additionalValues["emissiveTexture"].TextureIndex()].source);
		}
		if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
			material.occlusionTexture = GetTexture(gltfModel.textures[mat.additionalValues["occlusionTexture"].TextureIndex()].source);
		}
		if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end()) {
			tinygltf::Parameter param = mat.additionalValues["alphaMode"];
			if (param.string_value == "BLEND") {
				material.alphaMode = Material::ALPHAMODE_BLEND;
			}
			if (param.string_value == "MASK") {
				material.alphaMode = Material::ALPHAMODE_MASK;
			}
		}
		if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end()) {
			material.alphaCutoff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
		}

		mMaterials.push_back(material);
	}
	// Push a default material at the end of the list for meshes with no material assigned
	mMaterials.push_back(Material());

	return true;
}

bool_t glTF2Loader::Impl::LoadNode(glTF2Loader::Impl::Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model)
{
	glTF2Loader::Impl::Node* newNode = GE_ALLOC(glTF2Loader::Impl::Node);
	newNode->index = nodeIndex;
	newNode->parent = parent;
	newNode->name = node.name;

	// Generate local node matrix
	if (node.translation.size() == 3) {
		glm::vec3 translation = glm::make_vec3(node.translation.data());
		newNode->translation = translation;
	}
	if (node.rotation.size() == 4) {
		glm::quat q = glm::make_quat(node.rotation.data());
		newNode->rotation = glm::mat4(q);
	}
	if (node.scale.size() == 3) {
		glm::vec3 scale = glm::make_vec3(node.scale.data());
		newNode->scale = scale;
	}
	if (node.matrix.size() == 16) {
		newNode->matrix = glm::make_mat4x4(node.matrix.data());
	};

	// Node with children
	if (node.children.size() > 0) {
		for (auto i = 0; i < node.children.size(); i++) {
			LoadNode(newNode, model.nodes[node.children[i]], node.children[i], model);
		}
	}

	// Node contains mesh data
	if (node.mesh > -1)
	{
		const tinygltf::Mesh& mesh = model.meshes[node.mesh];
		glTF2Loader::Impl::Mesh* newMesh = GE_ALLOC(glTF2Loader::Impl::Mesh);
		newMesh->name = mesh.name;
		for (size_t j = 0; j < mesh.primitives.size(); j++)
		{
			const tinygltf::Primitive& primitive = mesh.primitives[j];
			if (primitive.indices < 0)
			{
				continue;
			}
			uint32_t vertexStart = 0;
			uint32_t indexStart = 0;
			uint32_t vertexCount = 0;
			uint32_t indexCount = 0;
			glm::vec3 posMin{};
			glm::vec3 posMax{};


			// Vertices
			{
				const float* bufferPos = nullptr;
				const float* bufferNormals = nullptr;
				const float* bufferTexCoords = nullptr;
				const float* bufferColors = nullptr;
				const float* bufferTangents = nullptr;
				uint32_t numColorComponents;

				// Position attribute is required
				assert(primitive.attributes.find("POSITION") != primitive.attributes.end());

				const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
				bufferPos = reinterpret_cast<const float*>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
				posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
				posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);
				mVertexAttributes.pos = 3;

				if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
					const tinygltf::Accessor& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView& normView = model.bufferViews[normAccessor.bufferView];
					bufferNormals = reinterpret_cast<const float*>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
					mVertexAttributes.normal = 3;
				}

				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
					const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
					bufferTexCoords = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
					mVertexAttributes.uv = 2;
				}

				if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
				{
					const tinygltf::Accessor& colorAccessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
					const tinygltf::BufferView& colorView = model.bufferViews[colorAccessor.bufferView];
					// Color buffer is either of type vec3 or vec4
					numColorComponents = colorAccessor.type == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3 ? 3 : 4;
					bufferColors = reinterpret_cast<const float*>(&(model.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
					mVertexAttributes.color = numColorComponents;
				}

				if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
				{
					const tinygltf::Accessor& tangentAccessor = model.accessors[primitive.attributes.find("TANGENT")->second];
					const tinygltf::BufferView& tangentView = model.bufferViews[tangentAccessor.bufferView];
					bufferTangents = reinterpret_cast<const float*>(&(model.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset]));
					mVertexAttributes.tangent = 4;
				}

				vertexStart = static_cast<uint32_t>(mVertexBuffer.size() / mVertexAttributes.size());
				vertexCount = static_cast<uint32_t>(posAccessor.count);

				uint32_t offset = mVertexBuffer.size();
				size_t newVBSize = vertexCount * mVertexAttributes.size() + offset;
				mVertexBuffer.resize(newVBSize);

				for (size_t v = 0; v < posAccessor.count; v++)
				{
					if (bufferPos)
					{
						::memcpy(&mVertexBuffer[offset + v * mVertexAttributes.pos], &bufferPos[v * mVertexAttributes.pos], sizeof(float32_t)* mVertexAttributes.pos);
					}
					if (bufferNormals)
					{
						offset += mVertexAttributes.pos;
						::memcpy(&mVertexBuffer[offset + v * mVertexAttributes.normal], &bufferNormals[v * mVertexAttributes.normal], sizeof(float32_t) * mVertexAttributes.normal);
					}
					if (bufferTexCoords)
					{
						offset += mVertexAttributes.pos + mVertexAttributes.normal;
						::memcpy(&mVertexBuffer[offset + v * mVertexAttributes.uv], &bufferTexCoords[v * mVertexAttributes.uv], sizeof(float32_t) * mVertexAttributes.uv);
					}
					if (bufferColors)
					{
						offset += mVertexAttributes.pos + mVertexAttributes.normal + mVertexAttributes.uv;
						::memcpy(&mVertexBuffer[offset + v * mVertexAttributes.color], &bufferColors[v * mVertexAttributes.color], sizeof(float32_t) * mVertexAttributes.color);
					}
					if (bufferTangents)
					{
						offset += mVertexAttributes.pos + mVertexAttributes.normal + mVertexAttributes.uv + mVertexAttributes.color;
						::memcpy(&mVertexBuffer[offset + v * mVertexAttributes.tangent], &bufferTangents[v * mVertexAttributes.tangent], sizeof(float32_t) * mVertexAttributes.tangent);
					}
				}
			}

			// Indices
			{
			const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
			const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

			indexStart = static_cast<uint32_t>(mIndexBuffer.size());
			indexCount = static_cast<uint32_t>(accessor.count);

			uint32_t offset = mIndexBuffer.size();
			mIndexBuffer.resize(offset + indexCount);

			switch (accessor.componentType) {
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
				uint32_t* buf = GE_ALLOC_ARRAY(uint32_t, accessor.count);
				memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
				for (size_t index = 0; index < accessor.count; index++) {
					mIndexBuffer[offset + index] = (buf[index] + vertexStart);
				}
				GE_FREE_ARRAY(buf);
				break;
			}
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
				uint16_t* buf = GE_ALLOC_ARRAY(uint16_t, accessor.count);
				memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
				for (size_t index = 0; index < accessor.count; index++) {
					mIndexBuffer[offset + index] = (buf[index] + vertexStart);
				}
				GE_FREE_ARRAY(buf);
				break;
			}
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
				uint8_t* buf = GE_ALLOC_ARRAY(uint8_t, accessor.count);
				memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
				for (size_t index = 0; index < accessor.count; index++) {
					mIndexBuffer[offset + index] = (buf[index] + vertexStart);
				}
				GE_FREE_ARRAY(buf);
				break;
			}
			default:
				LOG_ERROR("Index component type %d not supported!", accessor.componentType);
				return false;
			}
			}
			glTF2Loader::Impl::Primitive* newPrimitive = GE_ALLOC(glTF2Loader::Impl::Primitive)(indexStart, indexCount, primitive.material > -1 ? mMaterials[primitive.material] : mMaterials.back());
			newPrimitive->firstVertex = vertexStart;
			newPrimitive->vertexCount = vertexCount;
			newPrimitive->setDimensions(posMin, posMax);
			newMesh->primitives.push_back(newPrimitive);
		}
		newNode->mesh = newMesh;
	}

	if (parent) {
		parent->children.push_back(newNode);
	}
	else {
		mNodes.push_back(newNode);
	}

	return true;
}

void glTF2Loader::Impl::ApplyFlagsOnNode(glTF2Loader::Impl::Node* pNode, glTF2Loader::FileLoadingFlags loadingFlags)
{
	if (pNode && pNode->mesh)
	{
		const bool preTransform = loadingFlags & FileLoadingFlags::PreTransformVertices;

		const glm::mat4 localMatrix = pNode->getMatrix();
		for (Primitive* primitive : pNode->mesh->primitives)
		{
			for (uint32_t i = 0; i < primitive->vertexCount; i++)
			{
				//// Pre-transform vertex positions by node-hierarchy
				if (preTransform)
				{
					uint32_t posIndex = (primitive->firstVertex + i) * mVertexAttributes.size();
					glm::vec3 position(localMatrix * glm::vec4(glm::make_vec3(&mVertexBuffer[posIndex]), 1.0f));
					::memcpy(&mVertexBuffer[posIndex], &position.x, sizeof(glm::vec3));

					uint32_t normIndex = posIndex + mVertexAttributes.pos;
					glm::vec3 normal = glm::normalize(glm::mat3(localMatrix) * glm::make_vec3(&mVertexBuffer[normIndex]));
					::memcpy(&mVertexBuffer[normIndex], &normal.x, sizeof(glm::vec3));
				}
			}
		}

		// Node with children
		if (pNode->children.size() > 0)
		{
			for (auto* pChild : pNode->children)
			{
				ApplyFlagsOnNode(pChild, loadingFlags);
			}
		}
	}
}

void glTF2Loader::Impl::Primitive::setDimensions(glm::vec3 min, glm::vec3 max)
{
	dimensions.min = min;
	dimensions.max = max;
	dimensions.size = max - min;
	dimensions.center = (min + max) / 2.0f;
	dimensions.radius = glm::distance(min, max) / 2.0f;
}

glTF2Loader::Impl::Mesh::Mesh()
{};

glTF2Loader::Impl::Mesh::~Mesh()
{
	for (auto& primitive : primitives)
	{
		GE_FREE(primitive);
	}
}

glm::mat4 glTF2Loader::Impl::Node::localMatrix()
{
	// T * R * S
	return glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), scale) * matrix;
}

glm::mat4 glTF2Loader::Impl::Node::getMatrix()
{
	glm::mat4 m = localMatrix();
	glTF2Loader::Impl::Node* p = parent;
	while (p)
	{
		m = p->localMatrix() * m;
		p = p->parent;
	}
	return m;
}

glTF2Loader::Impl::Node::~Node()
{
	if (mesh)
	{
		GE_FREE(mesh);
	}

	for (auto& child : children)
	{
		GE_FREE(child);
	}
}

glTF2Loader::Impl::Texture* glTF2Loader::Impl::GetTexture(uint32_t index)
{
	if (index < mTextures.size())
	{
		return &mTextures[index];
	}

	return nullptr;
}

void glTF2Loader::Impl::Draw(std::function<void(uint32_t indexCount, uint32_t firstIndex)> onDrawCB)
{
	for (auto& node : mNodes)
	{
		DrawNode(node, onDrawCB);
	}
}

void glTF2Loader::Impl::DrawNode(glTF2Loader::Impl::Node* node, std::function<void(uint32_t indexCount, uint32_t firstIndex)> onDrawCB)
{
	if (node)
	{
		if (node->mesh)
		{
			for (Primitive* primitive : node->mesh->primitives)
			{
				onDrawCB(primitive->indexCount, primitive->firstIndex);
			}
		}
		for (auto& child : node->children)
		{
			DrawNode(child, onDrawCB);
		}
	}
}


glTF2Loader::glTF2Loader()
	: mpImpl(GE_ALLOC(Impl))
{}

glTF2Loader::glTF2Loader(const std::string& filePath, glTF2Loader::FileLoadingFlags loadingFlags)
	: glTF2Loader()
{
	assert(mpImpl != nullptr);

	mpImpl->LoadFromFile(filePath, loadingFlags);
}

glTF2Loader::~glTF2Loader()
{
	GE_FREE(mpImpl);
}

void glTF2Loader::Draw(std::function<void(uint32_t indexCount, uint32_t firstIndex)> onDrawCB)
{
	assert(mpImpl != nullptr);

	mpImpl->Draw(onDrawCB);
}

const glTF2Loader::VertexAttributes& glTF2Loader::GetVertexAttributes() const
{
	assert(mpImpl != nullptr);

	return mpImpl->mVertexAttributes;
}

const std::vector<float32_t>& glTF2Loader::GetVertexBuffer() const
{
	assert(mpImpl != nullptr);

	return mpImpl->mVertexBuffer;
}

const std::vector<uint32_t>& glTF2Loader::GetIndexBuffer() const
{
	assert(mpImpl != nullptr);

	return mpImpl->mIndexBuffer;
}