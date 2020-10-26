#include "airsvk/Pipeline.hpp"



namespace airsvk
{
	Pipeline::Pipeline(vk::Device gpu, vk::RenderPass render_pass, const std::vector<vk::DescriptorSetLayoutBinding>& layout_bindings, vk::GraphicsPipelineCreateInfo info) :
		GPU(gpu), DescriptorSetLayout(nullptr), PipelineLayout(nullptr), DescriptorPool(nullptr), DescriptorSet(nullptr), VulkanPipeline(nullptr), BindPoint(vk::PipelineBindPoint::eGraphics)
	{
		if (layout_bindings.size() > 0)
		{
			DescriptorSetLayout = GPU.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({}, static_cast<uint32_t>(layout_bindings.size()), layout_bindings.data()));

			PipelineLayout = GPU.createPipelineLayout(vk::PipelineLayoutCreateInfo({}, 1, &DescriptorSetLayout));

			std::vector<vk::DescriptorPoolSize> sizes;
			for (auto& binding : layout_bindings)
			{
				bool found = false;
				for (auto& size : sizes)
				{
					if (size.type == binding.descriptorType)
					{
						size.descriptorCount += binding.descriptorCount;
						found = true;
						break;
					}
				}
				if (!found) sizes.push_back(vk::DescriptorPoolSize(binding.descriptorType, binding.descriptorCount));
			}
			DescriptorPool = GPU.createDescriptorPool(vk::DescriptorPoolCreateInfo({}, 1, static_cast<uint32_t>(sizes.size()), sizes.data()));

			auto set_info = vk::DescriptorSetAllocateInfo()
				.setDescriptorPool(DescriptorPool)
				.setDescriptorSetCount(1)
				.setPSetLayouts(&DescriptorSetLayout);
			GPU.allocateDescriptorSets(&set_info, &DescriptorSet);
		}
		else PipelineLayout = GPU.createPipelineLayout(vk::PipelineLayoutCreateInfo());

		vk::Viewport viewport;
		vk::Rect2D scissor;
		auto viewport_state_info = 
			vk::PipelineViewportStateCreateInfo()
			.setViewportCount(1)
			.setPViewports(&viewport)
			.setScissorCount(1)
			.setPScissors(&scissor);

		info.setLayout(PipelineLayout).setRenderPass(render_pass).setPViewportState(&viewport_state_info);
		VulkanPipeline = GPU.createGraphicsPipeline(nullptr, info);
	}
	Pipeline::Pipeline(vk::Device gpu, const std::vector<vk::DescriptorSetLayoutBinding>& layout_bindings, vk::ComputePipelineCreateInfo info) :
		GPU(gpu), DescriptorSetLayout(nullptr), PipelineLayout(nullptr), DescriptorPool(nullptr), DescriptorSet(nullptr), 
		VulkanPipeline(nullptr), BindPoint(vk::PipelineBindPoint::eCompute)
	{
		DescriptorSetLayout = GPU.createDescriptorSetLayout(
			vk::DescriptorSetLayoutCreateInfo({}, static_cast<uint32_t>(layout_bindings.size()), layout_bindings.data()));
		
		PipelineLayout = GPU.createPipelineLayout(vk::PipelineLayoutCreateInfo({}, 1, &DescriptorSetLayout));

		VulkanPipeline = GPU.createComputePipeline(nullptr, info.setLayout(PipelineLayout));
	}

	Pipeline::Pipeline() noexcept : GPU(nullptr), DescriptorSetLayout(nullptr), PipelineLayout(nullptr), DescriptorPool(nullptr), DescriptorSet(nullptr), VulkanPipeline(nullptr), BindPoint()
	{
	}
	Pipeline& Pipeline::operator=(Pipeline&& pipeline) noexcept
	{
		std::swap(GPU, pipeline.GPU);
		std::swap(DescriptorSetLayout, pipeline.DescriptorSetLayout);
		std::swap(PipelineLayout, pipeline.PipelineLayout);
		std::swap(DescriptorPool, pipeline.DescriptorPool);
		std::swap(DescriptorSet, pipeline.DescriptorSet);
		std::swap(VulkanPipeline, pipeline.VulkanPipeline);
		std::swap(BindPoint, pipeline.BindPoint);
		return *this;
	}
	Pipeline::Pipeline(Pipeline&& vp) noexcept : GPU(nullptr), PipelineLayout(nullptr), DescriptorPool(nullptr), DescriptorSet(nullptr), VulkanPipeline(nullptr), BindPoint()
	{
		std::swap(GPU, vp.GPU);
		std::swap(DescriptorSetLayout, vp.DescriptorSetLayout);
		std::swap(PipelineLayout, vp.PipelineLayout);
		std::swap(DescriptorPool, vp.DescriptorPool);
		std::swap(DescriptorSet, vp.DescriptorSet);
		std::swap(VulkanPipeline, vp.VulkanPipeline);
		std::swap(BindPoint, vp.BindPoint);
	}
	Pipeline::~Pipeline()
	{
		if (GPU)
		{
			if (VulkanPipeline) GPU.destroyPipeline(VulkanPipeline);
			if (DescriptorPool) GPU.destroyDescriptorPool(DescriptorPool);
			if (PipelineLayout) GPU.destroyPipelineLayout(PipelineLayout);
			if (DescriptorSetLayout) GPU.destroyDescriptorSetLayout(DescriptorSetLayout);
		}
		GPU = nullptr; 
		DescriptorSetLayout = nullptr;
		PipelineLayout = nullptr;
		DescriptorPool = nullptr;
		VulkanPipeline = nullptr;
	}
	
	void Pipeline::UpdateDescriptorSet(vk::WriteDescriptorSet desc)
	{
		desc.dstSet = DescriptorSet;
		GPU.updateDescriptorSets(1, &desc, 0, nullptr);
	}
	
	void Pipeline::Bind(vk::CommandBuffer cmdb) const
	{
		cmdb.bindPipeline(BindPoint, VulkanPipeline);
		if (DescriptorSet) cmdb.bindDescriptorSets(BindPoint, PipelineLayout, 0, 1, &DescriptorSet, 0, nullptr);
	}
}