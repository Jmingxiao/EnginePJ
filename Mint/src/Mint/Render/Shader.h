#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

MT_NAMESPACE_BEGIN

enum class ShaderType
{
	VETEX_SHADER = 0,
	FRAGMENT_SHADER
};

enum class CompileType
{
	LINK_STATUS = 0,
	PROGRAM_STATUS
};
enum class BuiltinShaderType { simple = 0, pbr, custom };

struct ShaderInfo
{
	ShaderType type;
	std::string name;
	std::string path;
	ShaderInfo(ShaderType shaderType, const std::string& sPath, const std::string& sName = "")
		: type(shaderType), path(sPath), name(sName)
	{}
};

class Shader {
public :
	virtual ~Shader() = default;

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual void SetBool(const std::string& name, bool value) = 0;
	virtual void SetInt(const std::string& name, int value) = 0;
	virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
	virtual void SetFloat(const std::string& name, float value) = 0;
	virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
	virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
	virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
	virtual void SetMat3(const std::string& name, const glm::mat3& value) = 0;
	virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

	virtual const std::string& GetName() const = 0;
	static Ref<Shader> Create(const std::vector<ShaderInfo>& vecShaderInfo, const std::string& name);
protected:
	std::string ReadFile(const std::string& filepath);
};

class ShaderLibrary
{
public:
	void Init();
	void Add(const std::string& name, const Ref<Shader>& shader);
	void Add(const Ref<Shader>& shader);
	Ref<Shader> Load(const std::vector<ShaderInfo>& vecShaderInfo, const std::string& name);
	Ref<Shader> Get(const std::string& name);
	Ref<Shader> GetDefaultShader(BuiltinShaderType type = BuiltinShaderType::simple);

	bool Exists(const std::string& name) const;
private:
	std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	Ref<Shader> pbr;
	Ref<Shader> simple;
};


MT_NAMESPACE_END