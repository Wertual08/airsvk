#pragma once
#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 0
#pragma warning(push, 0)  
#include <vulkan/vulkan.hpp>
#pragma warning(pop)  
#include <airs/math.hpp>



namespace airsvk
{
	class RenderPass
	{
		friend class Graphics;
	private:
		const std::uint32_t ClearColor = 0x000000ffu;
		const bool FlipViewport = true;
		airs::vec2i Extent;

	protected:
		airs::vec2i GetExtent() const { return Extent; }
		virtual void Perform(vk::CommandBuffer cmdb) = 0;

	public:
		RenderPass() {}
		RenderPass(std::uint32_t clear_color) : ClearColor(clear_color) {}
		RenderPass(bool flip_viewport) : FlipViewport(flip_viewport) {}
		RenderPass(std::uint32_t clear_color, bool flip_viewport) : ClearColor(clear_color), FlipViewport(flip_viewport) {}
	};
}