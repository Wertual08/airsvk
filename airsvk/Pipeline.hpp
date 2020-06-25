#pragma once
#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 0
#include <vulkan/vulkan.hpp>



namespace airsvk
{
	class Pipeline
	{
		friend class Graphics;

	private:
		vk::Device GPU; 
		vk::PipelineLayout PipelineLayout;
		vk::Pipeline VulkanPipeline;
		vk::PipelineBindPoint BindPoint;

		Pipeline(vk::Device gpu, vk::RenderPass render_pass, const vk::PipelineLayoutCreateInfo& layout_info, vk::GraphicsPipelineCreateInfo info);
		Pipeline(vk::Device gpu, const vk::PipelineLayoutCreateInfo& layout_info, vk::ComputePipelineCreateInfo info);

	public:
		Pipeline(Pipeline&& vp) noexcept;
		~Pipeline();

		operator vk::Pipeline() const noexcept { return VulkanPipeline; }
		operator vk::PipelineBindPoint() const noexcept { return BindPoint; }
	};
}