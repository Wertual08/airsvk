#pragma once
#include "airs_vulkan.hpp"



namespace airsvk
{
	class DescriptorPool
	{
	private:
		vk::Device GPU;
		vk::DescriptorPool Pool;

		DescriptorPool(vk::Device gpu, const std::vector<vk::DescriptorPoolSize>& sizes, uint32_t sets);

	public:
		DescriptorPool() noexcept;
		DescriptorPool& operator=(DescriptorPool&& pool) noexcept;
		DescriptorPool(DescriptorPool&& pool) noexcept;
		~DescriptorPool();
	};
}