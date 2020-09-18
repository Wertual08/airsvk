#pragma once
#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 0
#pragma warning(push, 0)  
#include <vulkan/vulkan.hpp>
#pragma warning(pop)  



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