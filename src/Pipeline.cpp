#include "Pipeline.hpp"



namespace airsvk
{
	Pipeline::Pipeline(vk::Device gpu, vk::RenderPass render_pass, const vk::PipelineLayoutCreateInfo& layout_info, vk::GraphicsPipelineCreateInfo info) :
		GPU(gpu), PipelineLayout(GPU.createPipelineLayout(layout_info)), VulkanPipeline(nullptr), BindPoint(vk::PipelineBindPoint::eGraphics)
	{
		vk::Viewport viewport;
		vk::Rect2D scissor;
		auto viewport_state_info = vk::PipelineViewportStateCreateInfo()
			.setViewportCount(1)
			.setPViewports(&viewport)
			.setScissorCount(1)
			.setPScissors(&scissor);

		VulkanPipeline = GPU.createGraphicsPipeline(nullptr, info.setLayout(PipelineLayout).setRenderPass(render_pass).setPViewportState(&viewport_state_info));
	}
	Pipeline::Pipeline(vk::Device gpu, const vk::PipelineLayoutCreateInfo& layout_info, vk::ComputePipelineCreateInfo info) :
		GPU(gpu), PipelineLayout(GPU.createPipelineLayout(layout_info)), VulkanPipeline(GPU.createComputePipeline(nullptr, info.setLayout(PipelineLayout))),
		BindPoint(vk::PipelineBindPoint::eCompute)
	{
	}

	Pipeline::Pipeline(Pipeline&& vp) noexcept : GPU(nullptr), PipelineLayout(nullptr), VulkanPipeline(nullptr), BindPoint()
	{
		std::swap(GPU, vp.GPU);
		std::swap(PipelineLayout, vp.PipelineLayout);
		std::swap(VulkanPipeline, vp.VulkanPipeline);
		std::swap(BindPoint, vp.BindPoint);
	}
	Pipeline::~Pipeline()
	{
		if (GPU)
		{
			GPU.waitIdle();
			if (PipelineLayout) GPU.destroyPipelineLayout(PipelineLayout);
			if (VulkanPipeline) GPU.destroyPipeline(VulkanPipeline);
		}
		GPU = nullptr;
		PipelineLayout = nullptr;
		VulkanPipeline = nullptr;
	}
}