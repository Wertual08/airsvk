#include "airsvk/DescriptorPool.hpp"



namespace airsvk
{
	DescriptorPool::DescriptorPool(vk::Device gpu, const std::vector<vk::DescriptorPoolSize>& sizes, uint32_t sets) :
		GPU(gpu), Pool(GPU.createDescriptorPool(vk::DescriptorPoolCreateInfo({}, sets, static_cast<uint32_t>(sizes.size()), sizes.data())))
	{
	}
	DescriptorPool::DescriptorPool() noexcept : GPU(nullptr), Pool(nullptr)
	{
	}
	DescriptorPool& DescriptorPool::operator=(DescriptorPool&& pool) noexcept
	{
		std::swap(GPU, pool.GPU);
		std::swap(Pool, pool.Pool);
		return *this;
	}
	DescriptorPool::DescriptorPool(DescriptorPool&& pool) noexcept : GPU(nullptr), Pool(nullptr)
	{
		std::swap(GPU, pool.GPU);
		std::swap(Pool, pool.Pool);
	}
	DescriptorPool::~DescriptorPool()
	{
		if (GPU && Pool) GPU.destroyDescriptorPool(Pool);
		GPU = nullptr;
		Pool = nullptr;
	}
}