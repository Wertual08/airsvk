#pragma once
#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 0
#include <vulkan/vulkan.hpp>



namespace airsvk
{
	class Buffer
	{
		friend class Graphics;
	private:
		vk::Device GPU;
		vk::Buffer VulkanBuffer;
		uint32_t MemoryType;
		size_t MemorySize;
		vk::DeviceMemory Memory;

		Buffer(vk::Device gpu, const vk::PhysicalDeviceMemoryProperties& properties, const vk::BufferCreateInfo& info, vk::MemoryPropertyFlags required);

	public:
		Buffer(Buffer&& buffer) noexcept;
		~Buffer();

		operator vk::Buffer() const { return VulkanBuffer; }
		operator const vk::Buffer*() const { return &VulkanBuffer; }

		size_t Size() const { return MemorySize; }

		void* Map(size_t offset, size_t size) { return GPU.mapMemory(Memory, offset, size); }
		void* Map() { return GPU.mapMemory(Memory, 0, MemorySize); }
		void Unmap() { GPU.unmapMemory(Memory); }
	};
}