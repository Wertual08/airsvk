#pragma once
#include "airs_vulkan.hpp"



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