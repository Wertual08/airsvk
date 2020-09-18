#include "airsvk/BufferView.hpp"



namespace airsvk
{
	BufferView::BufferView(vk::Device gpu, const vk::BufferViewCreateInfo& info) :
		GPU(gpu), View(GPU.createBufferView(info))
	{
	}
	BufferView::BufferView() noexcept : GPU(nullptr), View(nullptr)
	{
	}
	BufferView& BufferView::operator=(BufferView&& bv) noexcept
	{
		std::swap(GPU, bv.GPU);
		std::swap(View, bv.View);
		return *this;
	}
	BufferView::BufferView(BufferView&& bv) noexcept : GPU(nullptr), View(nullptr)
	{
		std::swap(GPU, bv.GPU);
		std::swap(View, bv.View);
	}
	BufferView::~BufferView()
	{
		if (GPU && View) GPU.destroyBufferView(View);
		GPU = nullptr;
		View = nullptr;
	}
}