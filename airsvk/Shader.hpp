#pragma once
#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 0
#pragma warning(push, 0)  
#include <vulkan/vulkan.hpp>
#pragma warning(pop)  



namespace airsvk
{
	class Shader
	{
		friend class Graphics;
	private:
		vk::Device GPU;
		vk::ShaderModule VulkanShader;

		Shader(vk::Device gpu, const vk::ShaderModuleCreateInfo& info);

	public:
		Shader() noexcept;
		Shader& operator=(Shader&& shader) noexcept;
		Shader(Shader&& shader) noexcept;
		~Shader();

		operator vk::ShaderModule() { return VulkanShader; }
	};
}