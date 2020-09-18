#pragma once
#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 0
#pragma warning(push, 0)  
#include <vulkan/vulkan.hpp>
#pragma warning(pop)  



namespace airsvk
{
	class BufferView
	{
		friend class Buffer;
	private:
		vk::Device GPU;
		vk::BufferView View;

		BufferView(vk::Device gpu, const vk::BufferViewCreateInfo& info);

	public:
		BufferView() noexcept;
		BufferView& operator=(BufferView&& bv) noexcept;
		BufferView(BufferView&& bv) noexcept;
		~BufferView();

		operator vk::BufferView() const { return View; }
		operator const vk::BufferView* () const { return &View; }
	};
}