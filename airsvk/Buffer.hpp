#pragma once
#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 0
#pragma warning(push, 0)  
#include <vulkan/vulkan.hpp>
#pragma warning(pop)  
#include "BufferView.hpp"



namespace airsvk
{
	class Buffer
	{
		friend class Graphics;
	private:
		//vk::BufferCreateInfo CreateInfo;
		vk::SharingMode SharingMode;

		vk::Device GPU;
		vk::Buffer VulkanBuffer;
		uint32_t MemoryType;
		vk::DeviceSize MemorySize;
		vk::DeviceMemory Memory;

		Buffer(vk::Device gpu, const vk::PhysicalDeviceMemoryProperties& properties, const vk::BufferCreateInfo& info, vk::MemoryPropertyFlags required);

	public:
		Buffer() noexcept;
		Buffer& operator=(Buffer&& buffer) noexcept;
		Buffer(Buffer&& buffer) noexcept;
		~Buffer();

		operator vk::Buffer() const { return VulkanBuffer; }
		operator const vk::Buffer*() const { return &VulkanBuffer; }

		//void Resize(std::size_t size);

		vk::DeviceSize Size() const { return MemorySize; }

		void* Map(vk::DeviceSize offset, vk::DeviceSize size) { return GPU.mapMemory(Memory, offset, size); }
		void* Map() { return GPU.mapMemory(Memory, 0, MemorySize); }
		template<typename T>
		T* Map(vk::DeviceSize offset, vk::DeviceSize size) { return reinterpret_cast<T*>(GPU.mapMemory(Memory, offset, size)); }
		template<typename T>
		T* Map() { return reinterpret_cast<T*>(GPU.mapMemory(Memory, 0, MemorySize)); }
		void Unmap() { GPU.unmapMemory(Memory); }

		BufferView CreateView(vk::Format format, vk::DeviceSize offset, vk::DeviceSize range);
		BufferView CreateView(vk::Format format);
	};
}