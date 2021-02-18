#pragma once
#include "airs_vulkan.hpp"



namespace airsvk
{
	class Graphics;
	class Shader
	{
	private:
		vk::Device GPU;
		vk::ShaderModule VulkanShader;

	public:
		Shader() noexcept;
		Shader &operator=(Shader &&shader) noexcept;
		Shader(Shader &&shader) noexcept;
		Shader(Graphics &gfx, const std::vector<std::uint8_t> &code);
		~Shader();

		operator vk::ShaderModule() { return VulkanShader; }
	};
}