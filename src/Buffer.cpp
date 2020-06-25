#include "Buffer.hpp"



namespace airsvk
{
	uint32_t FindMemoryType(const vk::PhysicalDeviceMemoryProperties& device_properties, uint32_t typeFilter, vk::MemoryPropertyFlags properties)
	{
		for (uint32_t i = 0; i < device_properties.memoryTypeCount; i++) 
		{
			if ((typeFilter & (1 << i)) && (device_properties.memoryTypes[i].propertyFlags & properties) == properties) return i;
		}
		throw std::runtime_error("airs::VulkanBuffer error: Failed to find suitable device memory type.");
	}


	Buffer::Buffer(vk::Device gpu, const vk::PhysicalDeviceMemoryProperties& properties, const vk::BufferCreateInfo& info, vk::MemoryPropertyFlags required) : GPU(gpu)
	{
		VulkanBuffer = GPU.createBuffer(info);
		vk::MemoryRequirements requirements = GPU.getBufferMemoryRequirements(VulkanBuffer);
		MemoryType = FindMemoryType(properties, requirements.memoryTypeBits, required);
		MemorySize = requirements.size;
		Memory = GPU.allocateMemory(vk::MemoryAllocateInfo(MemorySize, MemoryType));
		GPU.bindBufferMemory(VulkanBuffer, Memory, 0);
	}

	Buffer::Buffer(Buffer&& buffer) noexcept : GPU(nullptr), VulkanBuffer(nullptr), Memory(nullptr)
	{
		std::swap(GPU, buffer.GPU);
		std::swap(VulkanBuffer, buffer.VulkanBuffer);
		std::swap(MemoryType, buffer.MemoryType);
		std::swap(MemorySize, buffer.MemorySize);
		std::swap(Memory, buffer.Memory);
	}
	Buffer::~Buffer()
	{
		if (GPU)
		{
			GPU.waitIdle();
			if (VulkanBuffer) GPU.destroyBuffer(VulkanBuffer);
			if (Memory) GPU.freeMemory(Memory);
		}
		GPU = nullptr;
		VulkanBuffer = nullptr;
		Memory = nullptr;
	}
}