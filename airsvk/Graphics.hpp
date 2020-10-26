#pragma once
#include <mutex>
#include <airs/Window.hpp>
#include <airs/math.hpp>
#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 0
#pragma warning(push, 0)  
#include <vulkan/vulkan.hpp>
#pragma warning(pop)  
#include "Shader.hpp"
#include "Pipeline.hpp"
#include "Buffer.hpp"
#include "BufferView.hpp"
#include "CommandLayout.hpp"



namespace airsvk
{
	class Graphics
	{
	private:
		vk::Instance VulkanInstance;

		vk::SurfaceKHR VulkanSurface;

		vk::PhysicalDevice PhysicalGPU;
		vk::PhysicalDeviceMemoryProperties GPUMemoryProperties;

		vk::SurfaceFormatKHR SurfaceFormat;
		vk::PresentModeKHR PresentMode;
		uint32_t ImageCount;

		vk::Device GPU;
		uint32_t GraphicsQueueFamilyIndex;
		uint32_t PresentQueueFamilyIndex;
		vk::Queue PresentQueue;
		vk::Queue GraphicsQueue;
		vk::CommandPool CommandPool;
		vk::RenderPass RenderPass; 
		
		vk::Extent2D Extent;
		vk::SwapchainKHR VulkanSwapchain;
		std::vector<vk::Image> Images;
		std::vector<vk::ImageView> ImageViews;
		std::vector<vk::Framebuffer> Framebuffers;
		std::size_t CurrentSemaphore;
		std::vector<vk::Semaphore> ImageAvailableSemaphores;
		std::vector<vk::Semaphore> RenderFinishedSemaphores;
		std::vector<vk::Fence> CommandFinishedFences;
		std::vector<vk::CommandBuffer> CommandBuffers;

		std::mutex CommandBufferMutex;
		CommandLayout* Layout;

		void InitVulkanInstance(const std::vector<const char*>& layers, const std::vector<const char*>& extensions);
		void InitSurface(const airs::Window& window);
		void InitPhysicalGPU(const std::vector<std::string>& required_extensions);
		void InitSurfaceProperties(vk::Format prefered_format, vk::ColorSpaceKHR prefered_color_space,
			vk::PresentModeKHR prefered_present_mode, uint32_t prefered_image_count);
		void InitGPU(const std::vector<std::string>& required_extensions);
		void InitRenderPass();

		void InitSurfaceExtent();
		void InitSwapchain();
		void InitImageViews();
		void InitFramebuffers();
		void InitSemaphores();
		void InitFences();
		void InitCommandBuffers();

		void PerformCommandBuffers();
		void ThreadSafePerformCommandBuffers();

		void Resize();

	public:
		Graphics() = delete;
		Graphics(Graphics&&) = delete;
		Graphics(const Graphics&) = delete;
		Graphics(const airs::Window& window, std::vector<const char*> layers = {}, 
			vk::Format prefered_format = vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR prefered_color_space = vk::ColorSpaceKHR::eSrgbNonlinear,
			vk::PresentModeKHR prefered_present_mode = vk::PresentModeKHR::eFifo, uint32_t prefered_image_count = 2);
		~Graphics();

		Shader CreateShader(const std::vector<uint8_t>& code) const;
		Pipeline CreatePipeline(const std::vector<vk::DescriptorSetLayoutBinding>& layout_bindings, const vk::GraphicsPipelineCreateInfo& info) const;
		Pipeline CreatePipeline(const std::vector<vk::DescriptorSetLayoutBinding>& layout_bindings, const vk::ComputePipelineCreateInfo& info) const;
		Buffer CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags required, vk::SharingMode mode = vk::SharingMode::eExclusive) const;

		vk::CommandBuffer StartCommandBuffer() const;
		void FinishCommandBuffer(vk::CommandBuffer cmdb) const;

		void SetCommandLayout(CommandLayout& layout);
		void ClearCommandLayout();

		void Present();
	};
}