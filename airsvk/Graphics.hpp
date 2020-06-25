#pragma once
#include <airs/Window.hpp>
#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 0
#include <vulkan/vulkan.hpp>
#include "Shader.hpp"
#include "Pipeline.hpp"
#include "Buffer.hpp"



namespace airsvk
{
	class Graphics
	{
	private:
		vk::Instance VulkanInstance;

		vk::SurfaceKHR VulkanSurface;
		vk::SurfaceFormatKHR SurfaceFormat;
		vk::PresentModeKHR PresentMode;
		uint32_t ImageCount;
		vk::Extent2D Extent;
		vk::Viewport Viewport;

		vk::PhysicalDevice PhysicalGPU;
		vk::PhysicalDeviceMemoryProperties GPUMemoryProperties;
		vk::Device GPU;
		uint32_t PresentQueueFamilyIndex;
		uint32_t GraphicsQueueFamilyIndex;
		vk::Queue PresentQueue;
		vk::Queue GraphicsQueue;

		vk::Semaphore ImageAvailableSemaphore;
		vk::Semaphore RenderFinishedSemaphore;

		vk::CommandPool CommandPool;

		vk::RenderPass RenderPass;

		vk::SwapchainKHR Swapchain;
		std::vector<vk::Image> Images;
		std::vector<vk::ImageView> ImageViews;
		std::vector<vk::Framebuffer> Framebuffers;
		std::vector<vk::CommandBuffer> CommandBuffers;

		void InitVulkanInstance(const std::vector<const char*>& layers, const std::vector<const char*>& extensions);
		void InitSurface(const airs::Window& window);
		void InitPhysicalGPU(const std::vector<std::string>& required_extensions);
		void InitSurfaceProperties(vk::Format prefered_format, vk::ColorSpaceKHR prefered_color_space,
			vk::PresentModeKHR prefered_present_mode, uint32_t prefered_image_count);
		void InitGPU(const std::vector<std::string>& required_extensions);
		void InitRenderPass();

		void InitSurfaceExtent(uint32_t w, uint32_t h);
		void InitSwapchain();
		void InitImageViews();
		void InitFrameBuffers();
		void InitCommandBuffers();

	public:
		Graphics() = delete;
		Graphics(Graphics&&) = delete;
		Graphics(const Graphics&) = delete;
		Graphics(const airs::Window& window, std::vector<const char*> layers, vk::Format prefered_format, vk::ColorSpaceKHR prefered_color_space,
			vk::PresentModeKHR prefered_present_mode, uint32_t prefered_image_count);
		~Graphics();

		airs::delegate<void(vk::CommandBuffer)> CommandBuffer;
		void Resize(int32_t w, int32_t h);

		Shader CreateShader(const std::vector<int8_t>& code) const;
		Pipeline CreatePipeline(const vk::PipelineLayoutCreateInfo& layout_info, const vk::GraphicsPipelineCreateInfo& info) const;
		Pipeline CreatePipeline(const vk::PipelineLayoutCreateInfo& layout_info, const vk::ComputePipelineCreateInfo& info) const;
		Buffer CreateBuffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags required, vk::SharingMode mode = vk::SharingMode::eExclusive);
		
		void Present();
	};
}