#include "airsvk/Buffer.hpp"



namespace airsvk
{
	std::uint32_t FindMemoryType(const vk::PhysicalDeviceMemoryProperties& device_properties, std::uint32_t typeFilter, vk::MemoryPropertyFlags properties)
	{
		for (std::uint32_t i = 0; i < device_properties.memoryTypeCount; i++) 
			if ((typeFilter & (1 << i)) && (device_properties.memoryTypes[i].propertyFlags & properties) == properties) return i;
		throw std::runtime_error("airs::VulkanBuffer error: Failed to find suitable device memory type.");
	}


	Buffer::Buffer(vk::Device gpu, const vk::PhysicalDeviceMemoryProperties& properties, const vk::BufferCreateInfo& info, vk::MemoryPropertyFlags required) : 
		GPU(gpu)
	{
		CreateInfo = info;
		VulkanBuffer = GPU.createBuffer(info);
		vk::MemoryRequirements requirements = GPU.getBufferMemoryRequirements(VulkanBuffer);
		MemoryType = FindMemoryType(properties, requirements.memoryTypeBits, required);
		MemorySize = requirements.size;
		Memory = GPU.allocateMemory(vk::MemoryAllocateInfo(MemorySize, MemoryType));
		GPU.bindBufferMemory(VulkanBuffer, Memory, 0);
	}
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
		Swap(new_buffer);
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