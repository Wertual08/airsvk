#include "airsvk/Shader.hpp"
#include "airsvk/Graphics.hpp"



namespace airsvk
{
	inline vk::ShaderModule create_shader(vk::Device gpu, const std::vector<std::uint8_t> &code)
	{
		vk::ShaderModuleCreateInfo info({}, code.size(), reinterpret_cast<const uint32_t *>(code.data()));
		return gpu.createShaderModule(info);
	}

	Shader::Shader() noexcept : GPU(nullptr), VulkanShader(nullptr)
	{
	}
	Shader& Shader::operator=(Shader&& shader) noexcept
	{
		std::swap(GPU, shader.GPU);
		std::swap(VulkanShader, shader.VulkanShader);
		return *this;
	}
	Shader::Shader(Shader&& shader) noexcept : GPU(nullptr), VulkanShader(nullptr)
	{
		std::swap(GPU, shader.GPU);
		std::swap(VulkanShader, shader.VulkanShader);
	}
	Shader::Shader(Graphics &gfx, const std::vector<std::uint8_t> &code):
		GPU(gfx.GetGPU()), VulkanShader(create_shader(GPU, code))
	{
	}
	Shader::~Shader()
	{
		if (GPU && VulkanShader) GPU.destroyShaderModule(VulkanShader);
		GPU = nullptr;
		VulkanShader = nullptr;
	}
}