#include "airsvk/Buffer.hpp"
#include "airsvk/Graphics.hpp"



namespace airsvk
{
	Buffer::Buffer(vk::Device gpu, std::uint32_t memory_type, const vk::BufferCreateInfo& info) : GPU(gpu)
	{
		CreateInfo = info;
		VulkanBuffer = GPU.createBuffer(info);
		MemoryType = memory_type;
		vk::MemoryRequirements requirements = GPU.getBufferMemoryRequirements(VulkanBuffer);
		MemorySize = requirements.size;
		Memory = GPU.allocateMemory(vk::MemoryAllocateInfo(MemorySize, MemoryType));
		GPU.bindBufferMemory(VulkanBuffer, Memory, 0);
	}

	Buffer::Buffer() noexcept : GPU(nullptr), VulkanBuffer(nullptr), Memory(nullptr)
	{
	}
	Buffer& Buffer::operator=(Buffer&& buffer) noexcept
	{
		Swap(buffer);
		return *this;
	}
	Buffer::Buffer(Buffer&& buffer) noexcept : GPU(nullptr), VulkanBuffer(nullptr), Memory(nullptr)
	{
		Swap(buffer);
	}
	Buffer::Buffer(Graphics &gfx, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags required, vk::SharingMode mode) :
		GPU(gfx.GetGPU())
	{
		CreateInfo = vk::BufferCreateInfo({}, size, usage, mode);
		VulkanBuffer = GPU.createBuffer(CreateInfo);
		vk::MemoryRequirements requirements = GPU.getBufferMemoryRequirements(VulkanBuffer);
		MemoryType = gfx.FindMemoryType(requirements.memoryTypeBits, required);
		MemorySize = requirements.size;
		Memory = GPU.allocateMemory(vk::MemoryAllocateInfo(MemorySize, MemoryType));
		GPU.bindBufferMemory(VulkanBuffer, Memory, 0);
	}
	Buffer::~Buffer()
	{
		if (GPU)
		{
			if (VulkanBuffer) GPU.destroyBuffer(VulkanBuffer);
			if (Memory) GPU.freeMemory(Memory);
		}
		GPU = nullptr;
		VulkanBuffer = nullptr;
		Memory = nullptr;
	}

	void Buffer::Swap(Buffer& buffer) noexcept
	{
		std::swap(GPU, buffer.GPU);
		std::swap(CreateInfo, buffer.CreateInfo);
		std::swap(VulkanBuffer, buffer.VulkanBuffer);
		std::swap(MemoryType, buffer.MemoryType);
		std::swap(MemorySize, buffer.MemorySize);
		std::swap(Memory, buffer.Memory);
	}

	Buffer Buffer::Recreate(vk::DeviceSize size)
	{
		auto new_info = CreateInfo;
		new_info.size = size;
		Buffer new_buffer(GPU, MemoryType, new_info);

		std::memcpy(new_buffer.Map(), Map(), std::min(new_buffer.Size(), Size()));
		Unmap();
		new_buffer.Unmap();

		return new_buffer;
	}
	
	BufferView Buffer::CreateView(vk::Format format, vk::DeviceSize offset, vk::DeviceSize range)
	{
		return std::move(BufferView(GPU, vk::BufferViewCreateInfo({}, VulkanBuffer, format, offset, range)));
	}
	BufferView Buffer::CreateView(vk::Format format)
	{
		return std::move(BufferView(GPU, vk::BufferViewCreateInfo({}, VulkanBuffer, format, 0, MemorySize)));
	}
}