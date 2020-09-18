#pragma once
#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 0
#pragma warning(push, 0)  
#include <vulkan/vulkan.hpp>
#pragma warning(pop)  



namespace airsvk
{
	class Pipeline
	{
		friend class Graphics;

	private:
		vk::Device GPU; 
		vk::DescriptorSetLayout DescriptorSetLayout;
		vk::DescriptorPool DescriptorPool;
		vk::DescriptorSet DescriptorSet;
		vk::PipelineLayout PipelineLayout;
		vk::Pipeline VulkanPipeline;
		vk::PipelineBindPoint BindPoint;

		Pipeline(vk::Device gpu, vk::RenderPass render_pass, const std::vector<vk::DescriptorSetLayoutBinding>& layout_bindings, vk::GraphicsPipelineCreateInfo info);
		Pipeline(vk::Device gpu, const std::vector<vk::DescriptorSetLayoutBinding>& layout_bindings, vk::ComputePipelineCreateInfo info);

	public:
		Pipeline() noexcept;
		Pipeline& operator=(Pipeline&& pipeline) noexcept;
		Pipeline(Pipeline&& vp) noexcept;
		~Pipeline();

		operator vk::Pipeline() const noexcept { return VulkanPipeline; }
		operator vk::PipelineBindPoint() const noexcept { return BindPoint; }

		void UpdateDescriptorSet(vk::WriteDescriptorSet desc);
		
		void Bind(vk::CommandBuffer cmdb) const;
	};
}