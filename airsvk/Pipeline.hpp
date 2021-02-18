#pragma once
#include "airs_vulkan.hpp"



namespace airsvk
{
	class Graphics;
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


	public:
		Pipeline() noexcept;
		Pipeline& operator=(Pipeline&& pipeline) noexcept;
		Pipeline(Pipeline&& vp) noexcept;
		Pipeline(Graphics &gfx, const std::vector<vk::DescriptorSetLayoutBinding>& layout_bindings, vk::GraphicsPipelineCreateInfo info);
		Pipeline(Graphics &gfx, const std::vector<vk::DescriptorSetLayoutBinding>& layout_bindings, vk::ComputePipelineCreateInfo info);
		~Pipeline();

		operator vk::Pipeline() const noexcept { return VulkanPipeline; }
		operator vk::PipelineBindPoint() const noexcept { return BindPoint; }

		void UpdateDescriptorSet(vk::WriteDescriptorSet desc);
		
		void Bind(vk::CommandBuffer cmdb) const;
	};
}