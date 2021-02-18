#pragma once
#include "airs_vulkan.hpp"
#include "BufferView.hpp"



namespace airsvk
{
	class Graphics;
	class Buffer
	{
	private:
		vk::Device GPU;
		vk::BufferCreateInfo CreateInfo;
		vk::Buffer VulkanBuffer;
		uint32_t MemoryType;
		vk::DeviceSize MemorySize;
		vk::DeviceMemory Memory;

		Buffer(vk::Device gpu, std::uint32_t memory_type, const vk::BufferCreateInfo& info);

	public:
		Buffer() noexcept;
		Buffer& operator=(Buffer&& buffer) noexcept;
		Buffer(Buffer&& buffer) noexcept;
		Buffer(Graphics &gfx, vk::DeviceSize size, vk::BufferUsageFlags usage, 
			vk::MemoryPropertyFlags required, vk::SharingMode mode = vk::SharingMode::eExclusive);
		~Buffer();

		void Swap(Buffer& buffer) noexcept;

		operator vk::Buffer() const { return VulkanBuffer; }
		operator const vk::Buffer*() const { return &VulkanBuffer; }

		Buffer Recreate(vk::DeviceSize size);

		vk::DeviceSize Size() const { return CreateInfo.size; }

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