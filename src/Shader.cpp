#include "Shader.hpp"



namespace airsvk
{
	Shader::Shader(vk::Device gpu, const vk::ShaderModuleCreateInfo& info) :
		GPU(gpu), VulkanShader(GPU.createShaderModule(info))
	{
	}
	Shader::Shader(Shader&& shader) noexcept : GPU(nullptr), VulkanShader(nullptr)
	{
		std::swap(GPU, shader.GPU);
		std::swap(VulkanShader, shader.VulkanShader);
	}
	Shader::~Shader()
	{
		if (GPU && VulkanShader) GPU.destroyShaderModule(VulkanShader);
		GPU = nullptr;
		VulkanShader = nullptr;
	}
}