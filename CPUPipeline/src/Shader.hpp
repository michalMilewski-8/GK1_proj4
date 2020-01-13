#pragma once

#include <string>
#include <unordered_map>
#include <Glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

#ifndef LOG_ERROR
	#define LOG_ERROR(...) fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\r\n");
#endif // !LOG_ERROR
#ifndef ASSERT
	#ifdef _DEBUG
		#define ASSERT(x, ...) { if(!(x)) { LOG_ERROR("Assertion Failed: %s", __VA_ARGS__); __debugbreak(); } }
	#else
		#define ASSERT(x, ...)
	#endif
#endif // !ASSERT


template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

class Shader
{
public:
	Shader(const std::string& filepath);
	Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	virtual ~Shader();

	virtual void Bind() const;
	virtual void Unbind() const;

	virtual void SetInt(const std::string& name, int value);
	virtual void SetFloat(const std::string& name,float value);
	virtual void SetFloat2(const std::string& name, const glm::vec2& value);
	virtual void SetFloat3(const std::string& name, const glm::vec3& value);
	virtual void SetFloat4(const std::string& name, const glm::vec4& value);
	virtual void SetMat3(const std::string& name, const glm::mat3& value);
	virtual void SetMat4(const std::string& name, const glm::mat4& value);

	virtual const std::string& GetName() const { return m_Name; }

	static Ref<Shader> Create(const std::string& filepath);
	static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

private:
	std::string ReadFile(const std::string& filepath);
	std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
	void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

private:
	uint32_t m_RendererID;
	std::string m_Name;
};