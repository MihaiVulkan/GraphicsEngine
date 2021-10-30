
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

		Texture()
			: width(0), height(0), mipLevels(0), layerCount(0)
		{}

		void FromglTfImage(tinygltf::Image& gltfimage, const std::string& filePath);
	};

	/*
		glTF material class
	*/
	struct Material
	{
		enum AlphaMode { ALPHAMODE_OPAQUE = 0, ALPHAMODE_MASK, ALPHAMODE_BLEND };
		AlphaMode alphaMode;
		float32_t alphaCutoff;
		float32_t metallicFactor;
		float32_t roughnessFactor;
		glm::vec4 baseColorFactor;

		glTF2Loader::Impl::Texture* pBaseColorTexture;
		glTF2Loader::Impl::Texture* pMetallicRoughnessTexture;
		glTF2Loader::Impl::Texture* pNormalTexture;
		glTF2Loader::Impl::Texture* pOcclusionTexture;
		glTF2Loader::Impl::Texture* pEmissiveTexture;

		glTF2Loader::Impl::Texture* pSpecularGlossinessTexture;
		glTF2Loader::Impl::Texture* pDiffuseTexture;

		Material()
			: alphaMode(AlphaMode::ALPHAMODE_OPAQUE), alphaCutoff(1.0f), metallicFactor(1.0f), roughnessFactor(1.0f), baseColorFactor(1.0f)
			, pBaseColorTexture(nullptr), pMetallicRoughnessTexture(nullptr), pNormalTexture(nullptr), pOcclusionTexture(nullptr), pEmissiveTexture(nullptr)
			, pSpecularGlossinessTexture(nullptr), pDiffuseTexture(nullptr)
		{}
	};

	struct Dimensions
	{
		glm::vec3 min = glm::vec3(FLT_MAX);
		glm::vec3 max = glm::vec3(-FLT_MAX);
		glm::vec3 size;
		glm::vec3 center;
		float32_t radius;

		Dimensions()
			: min(+ FLT_MAX), max(- FLT_MAX), size(0), center(0), radius(0.0f)
		{}

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

		Mesh()
			: name()
		{}

		~Mesh();
	};

	/*
		glTF node
	*/
	struct Node
	{
		glTF2Loader::Impl::Node* pParent;
		uint32_t index;
		std::vector<glTF2Loader::Impl::Node*> children;
		glm::mat4 matrix;
		std::string name;
		glTF2Loader::Impl::Mesh* pMesh;

		glm::vec3 translation;
		glm::vec3 scale;
		glm::quat rotation;

		Node()
			: pParent(nullptr), index(0), matrix(1.0f), pMesh(nullptr), translation(0.0f), scale(1.0f), rotation()
		{}

		~Node();

		glm::mat4 localMatrix();
		glm::mat4 getMatrix();
	};

	Impl();
	virtual ~Impl();

	bool_t LoadFromFile(const std::string& filePath, uint32_t loadingFlags);

	bool_t LoadImages(tinygltf::Model& gltfModel);
	bool_t LoadMaterials(tinygltf::Model& gltfModel);
	bool_t LoadNode(glTF2Loader::Impl::Node* pParent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model);

	void ApplyFlagsOnNode(glTF2Loader::Impl::Node* pNode, uint32_t loadingFlags);

	glTF2Loader::Impl::Texture* GetTexture(uint32_t index);

	void Draw(std::function<void(uint32_t indexCount, uint32_t firstVertex)> onDrawCB);
	void DrawNode(glTF2Loader::Impl::Node* pNode, std::function<void(uint32_t indexCount, uint32_t firstVertex)> onDrawCB);

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
	if (pImage && (pImage->uri.find_last_of(".") != std::string::npos))
	{
		if (pImage->uri.substr(pImage->uri.find_last_of(".") + 1) == "ktx2")
		{
			return true;
		}
	}

	return tinygltf::LoadImageData(pImage, imageIndex, pError, pWarning, req_width, req_height, pBytes, size, pUserData);
}

bool_t glTF2Loader::Impl::LoadFromFile(const std::string& filePath, uint32_t loadingFlags)
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
		const tinygltf::Node& node = gltfModel.nodes[scene.nodes[i]];

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
	if ((loadingFlags & FileLoadingFlags::PreTransformVertices) ||
		(loadingFlags & FileLoadingFlags::PreMultiplyVertexColors))
	{
		for (auto* pNode : mNodes)
		{
			ApplyFlagsOnNode(pNode, loadingFlags);
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
			material.pBaseColorTexture = GetTexture(gltfModel.textures[mat.values["baseColorTexture"].TextureIndex()].source);
		}
		// Metallic roughness workflow
		if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
			material.pMetallicRoughnessTexture = GetTexture(gltfModel.textures[mat.values["metallicRoughnessTexture"].TextureIndex()].source);
		}
		if (mat.values.find("roughnessFactor") != mat.values.end()) {
			material.roughnessFactor = static_cast<float32_t>(mat.values["roughnessFactor"].Factor());
		}
		if (mat.values.find("metallicFactor") != mat.values.end()) {
			material.metallicFactor = static_cast<float32_t>(mat.values["metallicFactor"].Factor());
		}
		if (mat.values.find("baseColorFactor") != mat.values.end()) {
			material.baseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
		}
		if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
			material.pNormalTexture = GetTexture(gltfModel.textures[mat.additionalValues["normalTexture"].TextureIndex()].source);
		}
		else {
		//	material.normalTexture = &emptyTexture; //TODO
		}
		if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
			material.pEmissiveTexture = GetTexture(gltfModel.textures[mat.additionalValues["emissiveTexture"].TextureIndex()].source);
		}
		if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
			material.pOcclusionTexture = GetTexture(gltfModel.textures[mat.additionalValues["occlusionTexture"].TextureIndex()].source);
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
			material.alphaCutoff = static_cast<float32_t>(mat.additionalValues["alphaCutoff"].Factor());
		}

		mMaterials.push_back(material);
	}
	// Push a default material at the end of the list for meshes with no material assigned
	mMaterials.push_back(Material());

	return true;
}

bool_t glTF2Loader::Impl::LoadNode(glTF2Loader::Impl::Node* pParent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model)
{
	glTF2Loader::Impl::Node* pNewNode = GE_ALLOC(glTF2Loader::Impl::Node);
	pNewNode->index = nodeIndex;
	pNewNode->pParent = pParent;
	pNewNode->name = node.name;

	// Generate local node matrix
	if (node.translation.size() == 3) {
		glm::vec3 translation = glm::make_vec3(node.translation.data());
		pNewNode->translation = translation;
	}
	if (node.rotation.size() == 4) {
		glm::quat q = glm::make_quat(node.rotation.data());
		pNewNode->rotation = glm::mat4(q);
	}
	if (node.scale.size() == 3) {
		glm::vec3 scale = glm::make_vec3(node.scale.data());
		pNewNode->scale = scale;
	}
	if (node.matrix.size() == 16) {
		pNewNode->matrix = glm::make_mat4x4(node.matrix.data());
	}

	// Node with children
	if (node.children.size() > 0) {
		for (auto i = 0; i < node.children.size(); i++) {
			LoadNode(pNewNode, model.nodes[node.children[i]], node.children[i], model);
		}
	}

	// Node contains mesh data
	if (node.mesh > -1)
	{
		const tinygltf::Mesh& mesh = model.meshes[node.mesh];
		glTF2Loader::Impl::Mesh* pNewMesh = GE_ALLOC(glTF2Loader::Impl::Mesh);
		pNewMesh->name = mesh.name;
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
				const float32_t* bufferPos = nullptr;
				const float32_t* bufferNormals = nullptr;
				const float32_t* bufferTexCoords = nullptr;
				const float32_t* bufferColors = nullptr;
				const float32_t* bufferTangents = nullptr;
				uint32_t numColorComponents;

				const float32_t defaultColor[3] = { 1.0f, 1.0f, 1.0f }; // white

				// Position attribute is required
				assert(primitive.attributes.find("POSITION") != primitive.attributes.end());

				const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
				bufferPos = reinterpret_cast<const float32_t*>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset])); // no copy, just reference to
				posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
				posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);
				mVertexAttributes.pos = 3;

				if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
				{
					const tinygltf::Accessor& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView& normView = model.bufferViews[normAccessor.bufferView];
					bufferNormals = reinterpret_cast<const float32_t*>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset])); // no copy, just reference to
					mVertexAttributes.normal = 3;
				}

				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) 
				{
					const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
					bufferTexCoords = reinterpret_cast<const float32_t*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset])); // no copy, just reference to
					mVertexAttributes.uv = 2;
				}

				if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
				{
					const tinygltf::Accessor& colorAccessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
					const tinygltf::BufferView& colorView = model.bufferViews[colorAccessor.bufferView];
					// Color buffer is either of type vec3 or vec4
					numColorComponents = colorAccessor.type == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3 ? 3 : 4;
					bufferColors = reinterpret_cast<const float32_t*>(&(model.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset])); // no copy, just reference to
					mVertexAttributes.color = numColorComponents;
				}
				else // defaults to vec3 white color
				{
					mVertexAttributes.color = sizeof(defaultColor)/sizeof(float32_t);
				}

				if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
				{
					const tinygltf::Accessor& tangentAccessor = model.accessors[primitive.attributes.find("TANGENT")->second];
					const tinygltf::BufferView& tangentView = model.bufferViews[tangentAccessor.bufferView];
					bufferTangents = reinterpret_cast<const float32_t*>(&(model.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset])); // no copy, just reference to
					mVertexAttributes.tangent = 4;
				}

				vertexStart = static_cast<uint32_t>(mVertexBuffer.size() / mVertexAttributes.size());
				vertexCount = static_cast<uint32_t>(posAccessor.count);

				uint32_t offset = mVertexBuffer.size();
				size_t newVBSize = vertexCount * mVertexAttributes.size() + offset;
				mVertexBuffer.resize(newVBSize);

				/*
				Attribute order convention:
				POSITION
				NORMAL
				TANGENT
				COLOR
				UV
				...
				*/

				for (uint32_t v = 0; v < vertexCount; v++)
				{
					uint32_t index = 0;
					if (bufferPos)
					{
						index = offset + mVertexAttributes.posOffset();
						::memcpy(&mVertexBuffer[index], &bufferPos[v * mVertexAttributes.pos], sizeof(float32_t) * mVertexAttributes.pos);
					}

					if (bufferNormals)
					{
						index = offset + mVertexAttributes.normalOffset();
						::memcpy(&mVertexBuffer[index], &bufferNormals[v * mVertexAttributes.normal], sizeof(float32_t) * mVertexAttributes.normal);
					}

					if (bufferTangents)
					{
						index = offset + mVertexAttributes.tangentOffset();
						::memcpy(&mVertexBuffer[index], &bufferTangents[v * mVertexAttributes.tangent], sizeof(float32_t) * mVertexAttributes.tangent);
					}

					if (bufferColors)
					{
						index = offset + mVertexAttributes.colorOffset();
						::memcpy(&mVertexBuffer[index], &bufferColors[v * mVertexAttributes.color], sizeof(float32_t) * mVertexAttributes.color);
					}
					else // defaults to vec3 white color
					{
						index = offset + mVertexAttributes.colorOffset();
						::memcpy(&mVertexBuffer[index], defaultColor, sizeof(float32_t) * mVertexAttributes.color);
					}

					if (bufferTexCoords)
					{
						index = offset + mVertexAttributes.uvOffset();
						::memcpy(&mVertexBuffer[index], &bufferTexCoords[v * mVertexAttributes.uv], sizeof(float32_t) * mVertexAttributes.uv);
					}

					offset += mVertexAttributes.size();
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
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
			{
				uint32_t* buf = GE_ALLOC_ARRAY(uint32_t, accessor.count);
				::memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));

				for (size_t index = 0; index < accessor.count; index++)
				{
					mIndexBuffer[offset + index] = (buf[index] + vertexStart);
				}
				GE_FREE_ARRAY(buf);
				break;
			}
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
			{
				uint16_t* buf = GE_ALLOC_ARRAY(uint16_t, accessor.count);
				::memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));

				for (size_t index = 0; index < accessor.count; index++)
				{
					mIndexBuffer[offset + index] = (buf[index] + vertexStart);
				}
				GE_FREE_ARRAY(buf);
				break;
			}
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
			{
				uint8_t* buf = GE_ALLOC_ARRAY(uint8_t, accessor.count);
				::memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));

				for (size_t index = 0; index < accessor.count; index++)
				{
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
			glTF2Loader::Impl::Primitive* pNewPrimitive = GE_ALLOC(glTF2Loader::Impl::Primitive)(indexStart, indexCount, primitive.material > -1 ? mMaterials[primitive.material] : mMaterials.back());
			pNewPrimitive->firstVertex = vertexStart;
			pNewPrimitive->vertexCount = vertexCount;
			pNewPrimitive->setDimensions(posMin, posMax);
			pNewMesh->primitives.push_back(pNewPrimitive);
		}
		pNewNode->pMesh = pNewMesh;
	}

	if (pParent) {
		pParent->children.push_back(pNewNode);
	}
	else {
		mNodes.push_back(pNewNode);
	}

	return true;
}

void glTF2Loader::Impl::ApplyFlagsOnNode(glTF2Loader::Impl::Node* pNode, uint32_t loadingFlags)
{
	if (pNode && (pNode->pMesh || pNode->children.size() > 0))
	{
		const bool preTransform = loadingFlags & FileLoadingFlags::PreTransformVertices;
		const bool preMultiplyColor = loadingFlags & FileLoadingFlags::PreMultiplyVertexColors;

		if (pNode->pMesh)
		{
			const glm::mat4 localMatrix = pNode->getMatrix();
			for (auto* pPrimitive : pNode->pMesh->primitives)
			{
				if (pPrimitive)
				{
					assert(pPrimitive->vertexCount * mVertexAttributes.size() <= mVertexBuffer.size());

					for (uint32_t i = 0; i < pPrimitive->vertexCount; i++)
					{
						uint32_t vertexIndex = (pPrimitive->firstVertex + i) * mVertexAttributes.size();

						//// Pre-transform vertex positions by node-hierarchy
						if (preTransform)
						{
							if (mVertexAttributes.pos > 0)
							{
								uint32_t posIndex = vertexIndex + mVertexAttributes.posOffset();
								glm::vec3 position(localMatrix * glm::vec4(glm::make_vec3(&mVertexBuffer[posIndex]), 1.0f));
								::memcpy(&mVertexBuffer[posIndex], &position.x, sizeof(glm::vec3));
							}

							if (mVertexAttributes.normal > 0)
							{
								uint32_t normIndex = vertexIndex + mVertexAttributes.normalOffset();
								glm::vec3 normal = glm::normalize(glm::mat3(localMatrix) * glm::make_vec3(&mVertexBuffer[normIndex]));
								::memcpy(&mVertexBuffer[normIndex], &normal.x, sizeof(glm::vec3));
							}
						}

						//// Pre-multiply color with the material base color
						if (preMultiplyColor)
						{
							if (mVertexAttributes.color > 0)
							{
								uint32_t colorIndex = vertexIndex + mVertexAttributes.colorOffset();
								if (mVertexAttributes.color == 3) //vec3
								{
									// when caching a vec4 in a vec3, w is ditched away
									glm::vec3 color = pPrimitive->material.baseColorFactor * glm::vec4(glm::make_vec3(&mVertexBuffer[colorIndex]), 1.0f);
									::memcpy(&mVertexBuffer[colorIndex], &color.x, sizeof(glm::vec3));
								}
								else // vec4 - default
								{
									glm::vec4 color = pPrimitive->material.baseColorFactor * glm::make_vec4(&mVertexBuffer[colorIndex]);
									::memcpy(&mVertexBuffer[colorIndex], &color.x, sizeof(glm::vec4));
								}
							}
						}
					}
				}
			}
		}

		// Node with children
		for (auto* pChild : pNode->children)
		{
			ApplyFlagsOnNode(pChild, loadingFlags);
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
	glTF2Loader::Impl::Node* p = pParent;
	while (p)
	{
		m = p->localMatrix() * m;
		p = p->pParent;
	}
	return m;
}

glTF2Loader::Impl::Node::~Node()
{
	if (pMesh)
	{
		GE_FREE(pMesh);
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

void glTF2Loader::Impl::DrawNode(glTF2Loader::Impl::Node* pNode, std::function<void(uint32_t indexCount, uint32_t firstIndex)> onDrawCB)
{
	if (pNode && pNode->pMesh)
	{
		for (auto* pPrimitive : pNode->pMesh->primitives)
		{
			if (pPrimitive)
			{
				onDrawCB(pPrimitive->indexCount, pPrimitive->firstIndex);
			}
		}

		for (auto& child : pNode->children)
		{
			DrawNode(child, onDrawCB);
		}
	}
}


glTF2Loader::glTF2Loader()
	: mpImpl(GE_ALLOC(Impl))
{}

glTF2Loader::glTF2Loader(const std::string& filePath, uint32_t loadingFlags)
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