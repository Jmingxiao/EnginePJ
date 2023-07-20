#pragma once
#include <glm/glm.hpp>


namespace Acg {
	enum class ShaderType
	{
		VETEX_SHADER = 0,
		FRAGMENT_SHADER,
		GEOMETRY_SHADER
	};

	enum class CompileType
	{
		LINK_STATUS = 0,
		PROGRAM_STATUS
	};
	enum class BuiltinShaderType 
	{ 
		simple = 0, pbr, background, custom 
	};

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

	public:
		Shader(const std::vector<ShaderInfo>shaderInfos, const std::string& name);
		~Shader();

		void Bind() const ;
		void Unbind() const ;

		bool CheckCompileError(unsigned int shaderprogram, CompileType type);

		void SetBool(const std::string& name, bool value) ;
		void SetInt(const std::string& name, int value) ;
		void SetIntArray(const std::string& name, int* values, uint32_t count) ;
		void SetFloat(const std::string& name, float value) ;
		void SetFloat2(const std::string& name, const glm::vec2& value) ;
		void SetFloat3(const std::string& name, const glm::vec3& value) ;
		void SetFloat4(const std::string& name, const glm::vec4& value) ;
		void SetMat3(const std::string& name, const glm::mat3& matrix) ;
		void SetMat4(const std::string& name, const glm::mat4& matrix) ;

		const std::string& GetName() const  { return m_Name; }


	private:
		std::string ReadFile(const std::string& filepath);

		uint32_t m_RendererID;
		std::string m_FilePath;
		std::string m_Name;

		std::unordered_map<unsigned int, std::string> m_OpenGLSourceCode;
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
		Ref<Shader> background;
	};


}