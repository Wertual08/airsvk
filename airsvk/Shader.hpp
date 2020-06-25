#pragma once
#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 0
#include <vulkan/vulkan.hpp>



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
		Shader(Shader&& shader) noexcept;
		~Shader();

		operator vk::ShaderModule() { return VulkanShader; }
	};
}