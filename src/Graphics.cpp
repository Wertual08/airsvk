#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX
#include "airsvk/Graphics.hpp"
#include <string>
#include <set>



namespace airsvk
{
    vk::SurfaceFormatKHR FindSurfaceFormat(vk::PhysicalDevice physical_gpu, vk::SurfaceKHR surface, vk::Format prefered_format, vk::ColorSpaceKHR prefered_color_space)
    {
        std::vector<vk::SurfaceFormatKHR> SupportedFormats = physical_gpu.getSurfaceFormatsKHR(surface);
        if (SupportedFormats.size() == 1 && SupportedFormats[0].format == vk::Format::eUndefined) return SupportedFormats[0];
        else
        {
            vk::SurfaceFormatKHR selected = SupportedFormats[0];
            for (vk::SurfaceFormatKHR surf_format : SupportedFormats)
            {
                if (surf_format.format == prefered_format && surf_format.colorSpace == prefered_color_space) return surf_format;
                if (surf_format.format == prefered_format) selected = surf_format;
                if (selected.format != prefered_format && surf_format.colorSpace == prefered_color_space) selected = surf_format;
            }
            return selected;
        }
    }
    vk::PresentModeKHR FindPresentMode(vk::PhysicalDevice physical_gpu, vk::SurfaceKHR surface, vk::PresentModeKHR prefered_present_mode)
    {
        std::vector<vk::PresentModeKHR> SurfacePresentModes = physical_gpu.getSurfacePresentModesKHR(surface);
        for (vk::PresentModeKHR mode : SurfacePresentModes)
            if (mode == prefered_present_mode) return mode;
        return SurfacePresentModes[0];
    }
    uint32_t FindImageCount(vk::PhysicalDevice physical_gpu, vk::SurfaceKHR surface, uint32_t prefered_count)
    {
        vk::SurfaceCapabilitiesKHR capabilities = physical_gpu.getSurfaceCapabilitiesKHR(surface);
        if (prefered_count < capabilities.minImageCount) prefered_count = capabilities.minImageCount;
        if (capabilities.maxImageCount > 0 && prefered_count > capabilities.maxImageCount)
            prefered_count = capabilities.maxImageCount;
        return prefered_count;
    }
    bool PhysicalGPUSuitable(vk::SurfaceKHR surface, vk::PhysicalDevice device, const std::vector<std::string>& required_extensions)
    {
        if (device.getSurfaceFormatsKHR(surface).empty()) return false;
        if (device.getSurfacePresentModesKHR(surface).empty()) return false;
        std::set<std::string> requiredExtensions(required_extensions.begin(), required_extensions.end());
        for (const auto& extension : device.enumerateDeviceExtensionProperties())
        {
            requiredExtensions.erase(extension.extensionName);
            if (requiredExtensions.empty()) break;
        }
        if (!requiredExtensions.empty()) return false;

        std::vector<vk::QueueFamilyProperties> QueueFamilies = device.getQueueFamilyProperties();
        uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
        uint32_t presentQueueFamilyIndex = UINT32_MAX;
        for (uint32_t i = 0; i < QueueFamilies.size(); i++)
        {
            if (QueueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                if (graphicsQueueFamilyIndex == UINT32_MAX)
                    graphicsQueueFamilyIndex = i;

                if (device.getSurfaceSupportKHR(i, surface))
                {
                    graphicsQueueFamilyIndex = i;
                    presentQueueFamilyIndex = i;
                    break;
                }
            }
        }
        if (presentQueueFamilyIndex == UINT32_MAX)
        {
            for (uint32_t i = 0; i < QueueFamilies.size(); ++i)
            {
                if (device.getSurfaceSupportKHR(i, surface))
                {
                    presentQueueFamilyIndex = i;
                    break;
                }
            }
        }
        if (graphicsQueueFamilyIndex == UINT32_MAX || presentQueueFamilyIndex == UINT32_MAX) return false;

        return true;
    }


    void Graphics::InitVulkanInstance(const std::vector<const char*>& layers, const std::vector<const char*>& extensions)
    {
        auto const app = vk::ApplicationInfo()
            .setPApplicationName("AIRS Vulkan Application")
            .setApplicationVersion(0x00000000u)
            .setPEngineName("AIRS Vulkan")
            .setEngineVersion(0x00000000u)
            .setApiVersion(VK_API_VERSION_1_1);
        auto const inst_info = vk::InstanceCreateInfo()
            .setPApplicationInfo(&app)
            .setEnabledLayerCount(static_cast<uint32_t>(layers.size()))
            .setPpEnabledLayerNames(layers.data())
            .setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()))
            .setPpEnabledExtensionNames(extensions.data());

        VulkanInstance = vk::createInstance(inst_info);
    }
    void Graphics::InitSurface(const airs::Window& window)
    {
        vk::Win32SurfaceCreateInfoKHR info({}, GetModuleHandleW(nullptr), static_cast<HWND>(window.Handle()));
        VulkanSurface = VulkanInstance.createWin32SurfaceKHR(info);
    }
    void Graphics::InitPhysicalGPU(const std::vector<std::string>& required_extensions)
    {
        PhysicalGPU = nullptr;
        for (vk::PhysicalDevice device : VulkanInstance.enumeratePhysicalDevices())
        {
            vk::PhysicalDeviceProperties properties = device.getProperties();
            if (PhysicalGPUSuitable(VulkanSurface, device, required_extensions))
            {
                if (!PhysicalGPU) PhysicalGPU = device;
                else
                {
                    vk::PhysicalDeviceProperties properties = device.getProperties();
                    vk::PhysicalDeviceProperties current_properties = PhysicalGPU.getProperties();
                    //PhysicalGPU = device;
                    switch (properties.deviceType)
                    {
                    case vk::PhysicalDeviceType::eIntegratedGpu:
                        if (current_properties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu)
                        {
                            PhysicalGPU = device;
                        }
                        break;
                    case vk::PhysicalDeviceType::eDiscreteGpu:
                            PhysicalGPU = device;
                        break;
                    case vk::PhysicalDeviceType::eVirtualGpu:
                        if (current_properties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu &&
                            current_properties.deviceType != vk::PhysicalDeviceType::eIntegratedGpu)
                        {
                            PhysicalGPU = device;
                        }
                        break;
                    case vk::PhysicalDeviceType::eCpu:
                        break;
                    default: break;
                    }
                }
            }
        }
        if (!PhysicalGPU) throw std::runtime_error("airs::VulkanWindow initialization error: No suitable GPU available.");
        GPUMemoryProperties = PhysicalGPU.getMemoryProperties();
    }
    void Graphics::InitSurfaceProperties(vk::Format prefered_format, vk::ColorSpaceKHR prefered_color_space, vk::PresentModeKHR prefered_present_mode, uint32_t prefered_image_count)
    {
        SurfaceFormat = FindSurfaceFormat(PhysicalGPU, VulkanSurface, prefered_format, prefered_color_space);
        PresentMode = FindPresentMode(PhysicalGPU, VulkanSurface, prefered_present_mode);
        ImageCount = FindImageCount(PhysicalGPU, VulkanSurface, prefered_image_count);
    }
    void Graphics::InitGPU(const std::vector<std::string>& required_extensions)
    {
        std::vector<vk::QueueFamilyProperties> queue_families = PhysicalGPU.getQueueFamilyProperties();
        GraphicsQueueFamilyIndex = std::numeric_limits<uint32_t>::max();
        PresentQueueFamilyIndex = std::numeric_limits<uint32_t>::max();
        for (uint32_t i = 0; i < queue_families.size(); i++)
        {
            if (queue_families[i].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                if (GraphicsQueueFamilyIndex == std::numeric_limits<uint32_t>::max())
                    GraphicsQueueFamilyIndex = i;

                if (PhysicalGPU.getSurfaceSupportKHR(i, VulkanSurface))
                {
                    GraphicsQueueFamilyIndex = i;
                    PresentQueueFamilyIndex = i;
                    break;
                }
            }
        }
        if (PresentQueueFamilyIndex == std::numeric_limits<uint32_t>::max())
        {
            for (uint32_t i = 0; i < queue_families.size(); ++i)
            {
                if (PhysicalGPU.getSurfaceSupportKHR(i, VulkanSurface))
                {
                    PresentQueueFamilyIndex = i;
                    break;
                }
            }
        }
        if (GraphicsQueueFamilyIndex == std::numeric_limits<uint32_t>::max())
            throw std::runtime_error("Vulkan initialization error: Could not find graphics queue.");
        if (PresentQueueFamilyIndex == std::numeric_limits<uint32_t>::max())
            throw std::runtime_error("Vulkan initialization error: Could not find present queue.");

        std::vector<const char*> SelectedDeviceExtensions(required_extensions.size());
        for (size_t i = 0; i < required_extensions.size(); i++)
            SelectedDeviceExtensions[i] = required_extensions[i].c_str();

        auto features = vk::PhysicalDeviceFeatures()
            .setGeometryShader(true);

        float const priorities[1] = { 0.0 };
        vk::DeviceQueueCreateInfo queues[2];
        queues[0].setQueueFamilyIndex(GraphicsQueueFamilyIndex);
        queues[0].setQueueCount(1);
        queues[0].setPQueuePriorities(priorities);
        auto deviceInfo = vk::DeviceCreateInfo()
            .setPEnabledFeatures(&features)
            .setQueueCreateInfoCount(1)
            .setPQueueCreateInfos(queues)
            .setEnabledExtensionCount(static_cast<uint32_t>(SelectedDeviceExtensions.size()))
            .setPpEnabledExtensionNames(SelectedDeviceExtensions.data());
        if (GraphicsQueueFamilyIndex != PresentQueueFamilyIndex)
        {
            queues[1].setQueueFamilyIndex(PresentQueueFamilyIndex);
            queues[1].setQueueCount(1);
            queues[1].setPQueuePriorities(priorities);
            deviceInfo.setQueueCreateInfoCount(2);
        }

        GPU = PhysicalGPU.createDevice(deviceInfo);

        GraphicsQueue = GPU.getQueue(GraphicsQueueFamilyIndex, 0);
        PresentQueue = GPU.getQueue(PresentQueueFamilyIndex, 0);

        auto command_pool_info = vk::CommandPoolCreateInfo()
            .setQueueFamilyIndex(GraphicsQueueFamilyIndex)
            .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        CommandPool = GPU.createCommandPool(command_pool_info);
    }
    void Graphics::InitRenderPass()
    {
        auto color_attachment = vk::AttachmentDescription()
            .setFormat(SurfaceFormat.format)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

        vk::AttachmentReference attachment_reference(0, vk::ImageLayout::eColorAttachmentOptimal);
        auto subpass_description = vk::SubpassDescription()
            .setColorAttachmentCount(1)
            .setPColorAttachments(&attachment_reference);

        auto dependency = vk::SubpassDependency()
            .setSrcSubpass(VK_SUBPASS_EXTERNAL)
            .setDstSubpass(0)
            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

        auto render_pass_info = vk::RenderPassCreateInfo()
            .setAttachmentCount(1)
            .setPAttachments(&color_attachment)
            .setSubpassCount(1)
            .setPSubpasses(&subpass_description)
            .setDependencyCount(1)
            .setPDependencies(&dependency);

        RenderPass = GPU.createRenderPass(render_pass_info);
    }

    void Graphics::InitSurfaceExtent()
    {
        Extent = PhysicalGPU.getSurfaceCapabilitiesKHR(VulkanSurface).currentExtent;
    }
    void Graphics::InitSwapchain()
    {
        uint32_t QueueFamilyIndices[] = { GraphicsQueueFamilyIndex, PresentQueueFamilyIndex };
        auto create_info = vk::SwapchainCreateInfoKHR()
            .setSurface(VulkanSurface)
            .setImageFormat(SurfaceFormat.format)
            .setImageColorSpace(SurfaceFormat.colorSpace)
            .setPresentMode(PresentMode)
            .setMinImageCount(ImageCount)
            .setImageExtent(Extent)
            .setImageArrayLayers(1)
            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
            .setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
            .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
            .setClipped(true)
            .setOldSwapchain(nullptr);

        if (GraphicsQueueFamilyIndex != PresentQueueFamilyIndex)
        {
            create_info.setImageSharingMode(vk::SharingMode::eConcurrent);
            create_info.setQueueFamilyIndexCount(2);
            create_info.setPQueueFamilyIndices(QueueFamilyIndices);
        }
        else
        {
            create_info.setImageSharingMode(vk::SharingMode::eExclusive);
            create_info.setQueueFamilyIndexCount(1);
            create_info.setPQueueFamilyIndices(QueueFamilyIndices);
        }

        VulkanSwapchain = GPU.createSwapchainKHR(create_info);
        Images = GPU.getSwapchainImagesKHR(VulkanSwapchain);
    }
    void Graphics::InitImageViews()
    {
        ImageViews.resize(Images.size());
        for (size_t i = 0; i < Images.size(); i++)
        {
            auto info = vk::ImageViewCreateInfo()
                .setImage(Images[i])
                .setViewType(vk::ImageViewType::e2D)
                .setFormat(SurfaceFormat.format)
                .setComponents(vk::ComponentMapping(
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity))
                .setSubresourceRange(vk::ImageSubresourceRange()
                    .setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setBaseMipLevel(0)
                    .setLevelCount(1)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1));
            ImageViews[i] = GPU.createImageView(info);
        }
    }
    void Graphics::InitFramebuffers()
    {
        Framebuffers.resize(Images.size());
        for (size_t i = 0; i < Images.size(); i++)
        {
            vk::ImageView attachments[] = {
                ImageViews[i]
            };
            auto framebuffer_info = vk::FramebufferCreateInfo()
                .setRenderPass(RenderPass)
                .setAttachmentCount(1)
                .setPAttachments(attachments)
                .setWidth(Extent.width)
                .setHeight(Extent.height)
                .setLayers(1);
            Framebuffers[i] = GPU.createFramebuffer(framebuffer_info);
        }
    }
    void Graphics::InitSemaphores()
    {
        CurrentSemaphore = 0;
        ImageAvailableSemaphores.resize(Images.size());
        RenderFinishedSemaphores.resize(Images.size());
        for (auto& semaphore : RenderFinishedSemaphores)
            semaphore = GPU.createSemaphore(vk::SemaphoreCreateInfo());
        for (auto& semaphore : ImageAvailableSemaphores)
            semaphore = GPU.createSemaphore(vk::SemaphoreCreateInfo());
    }
    void Graphics::InitFences()
    {
        CommandFinishedFences.resize(Images.size());
        for (auto& fence : CommandFinishedFences)
            fence = GPU.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
    }
    void Graphics::InitCommandBuffers()
    {
        vk::CommandBufferAllocateInfo allocate_info(
            CommandPool, vk::CommandBufferLevel::ePrimary,
            static_cast<uint32_t>(Images.size()));
        CommandBuffers = GPU.allocateCommandBuffers(allocate_info);
    }

    void Graphics::Resize()
    {
        std::scoped_lock lock(CommandBufferMutex);
        GPU.waitForFences(CommandFinishedFences, true, std::numeric_limits<std::uint64_t>::max());

        InitSurfaceExtent();
        if (Extent.width < 1 || Extent.height < 1) return;

        for (auto fence : CommandFinishedFences)
            GPU.destroyFence(fence);
        for (auto semaphore : RenderFinishedSemaphores)
            GPU.destroySemaphore(semaphore);
        for (auto semaphore : ImageAvailableSemaphores)
            GPU.destroySemaphore(semaphore);
        GPU.freeCommandBuffers(CommandPool, CommandBuffers);
        for (auto framebuffer : Framebuffers)
            GPU.destroyFramebuffer(framebuffer);
        for (auto image_view : ImageViews)
            GPU.destroyImageView(image_view);
        GPU.destroySwapchainKHR(VulkanSwapchain);

        InitSwapchain();
        InitImageViews();
        InitFramebuffers();
        InitSemaphores();
        InitFences();
        InitCommandBuffers();
        PerformCommandBuffers();
    }

    Graphics::Graphics(const airs::Window& window, std::vector<const char*> layers, vk::Format prefered_format, vk::ColorSpaceKHR prefered_color_space,
		vk::PresentModeKHR prefered_present_mode, uint32_t prefered_image_count) : Layout(nullptr)
	{
        std::vector<const char*> extensions = {
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
            VK_KHR_SURFACE_EXTENSION_NAME,
        };

        InitVulkanInstance(layers, extensions);
        InitSurface(window);
        InitPhysicalGPU({ VK_KHR_SWAPCHAIN_EXTENSION_NAME });
        InitSurfaceProperties(prefered_format, prefered_color_space, prefered_present_mode, prefered_image_count);
        InitGPU({ VK_KHR_SWAPCHAIN_EXTENSION_NAME });
        InitRenderPass();

        InitSurfaceExtent();
        InitSwapchain();
        InitImageViews();
        InitFramebuffers();
        InitSemaphores();
        InitFences();
        InitCommandBuffers();
        PerformCommandBuffers();
	}
    Graphics::~Graphics()
    {
        GPU.waitIdle();

        GPU.waitForFences(CommandFinishedFences, true, std::numeric_limits<std::uint64_t>::max());
        for (auto fence : CommandFinishedFences)
            GPU.destroyFence(fence);
        for (auto semaphore : RenderFinishedSemaphores)
            GPU.destroySemaphore(semaphore);
        for (auto semaphore : ImageAvailableSemaphores)
            GPU.destroySemaphore(semaphore);
        GPU.freeCommandBuffers(CommandPool, CommandBuffers);
        for (auto framebuffer : Framebuffers)
            GPU.destroyFramebuffer(framebuffer);
        for (auto image_view : ImageViews)
            GPU.destroyImageView(image_view);
        GPU.destroySwapchainKHR(VulkanSwapchain);

        GPU.destroyRenderPass(RenderPass);
        GPU.destroyCommandPool(CommandPool);
        GPU.destroy();
        VulkanInstance.destroySurfaceKHR(VulkanSurface);
        VulkanInstance.destroy();
    }

    vk::Device Graphics::GetGPU()
    {
        return GPU;
    }
    const vk::PhysicalDeviceMemoryProperties &Graphics::GetGPUMemoryProperties() const
    {
        return GPUMemoryProperties;
    }
    vk::RenderPass Graphics::GetRenderPass()
    {
        return RenderPass;
    }

    std::uint32_t Graphics::FindMemoryType(std::uint32_t typeFilter, vk::MemoryPropertyFlags properties) const
    {
        for (std::uint32_t i = 0; i < GPUMemoryProperties.memoryTypeCount; i++)
            if ((typeFilter & (1 << i)) && (GPUMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties) return i;
        throw std::runtime_error("airs::VulkanBuffer error: Failed to find suitable device memory type.");
    }

    vk::CommandBuffer Graphics::StartCommandBuffer() const
    {
        vk::CommandBuffer cmdb;
        vk::CommandBufferAllocateInfo info(CommandPool, vk::CommandBufferLevel::ePrimary, 1);
        GPU.allocateCommandBuffers(&info, &cmdb);
        cmdb.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        return cmdb;
    }
    void Graphics::FinishCommandBuffer(vk::CommandBuffer cmdb) const
    {
        cmdb.end();
        auto submit_info = vk::SubmitInfo()
            .setCommandBufferCount(1)
            .setPCommandBuffers(&cmdb);
        GraphicsQueue.submit(submit_info, nullptr);
        GraphicsQueue.waitIdle();
        GPU.freeCommandBuffers(CommandPool, cmdb);
    }

    void Graphics::PerformCommandBuffers()
    {
        vk::Viewport viewport(
            0.0f,
            static_cast<float>(Extent.height),
            static_cast<float>(Extent.width),
            -static_cast<float>(Extent.height),
            0.0f, 1.0f);
        vk::Rect2D render_area({ 0, 0 }, Extent);
        vk::ClearValue clear_color;
        clear_color.color.float32[0] = 0.0f;
        clear_color.color.float32[1] = 0.0f;
        clear_color.color.float32[2] = 0.0f;
        clear_color.color.float32[3] = 1.0f;

        GPU.waitForFences(CommandFinishedFences, true, std::numeric_limits<std::uint64_t>::max());
        for (std::uint32_t i = 0; i < Images.size(); i++)
        {
            vk::CommandBuffer cmdb = CommandBuffers[i];

            auto render_pass_begin_info = vk::RenderPassBeginInfo()
                .setRenderPass(RenderPass)
                .setFramebuffer(Framebuffers[i])
                .setRenderArea(render_area)
                .setClearValueCount(1)
                .setPClearValues(&clear_color);
            cmdb.begin(vk::CommandBufferBeginInfo());
            cmdb.setViewport(0, viewport);
            cmdb.setScissor(0, render_area);
            cmdb.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);

            if (Layout) Layout->Perform(cmdb, { Extent.width, Extent.height });

            cmdb.endRenderPass();
            cmdb.end();
        }
    }
    void Graphics::ThreadSafePerformCommandBuffers()
    {
        std::scoped_lock lock(CommandBufferMutex);
        PerformCommandBuffers();
    }
    std::unique_lock<std::mutex> Graphics::LockCommandBuffers()
    {
        std::unique_lock<std::mutex> lock(CommandBufferMutex);
        GPU.waitForFences(CommandFinishedFences, true, std::numeric_limits<std::uint64_t>::max());
        return std::move(lock);
    }

    void Graphics::SetCommandLayout(CommandLayout& layout)
    {
        if (Layout) Layout->Owner = nullptr;
        Layout = &layout;
        Layout->Owner = this;
        ThreadSafePerformCommandBuffers();
    }
    void Graphics::ClearCommandLayout()
    {
        if (Layout) Layout->Owner = nullptr;
        Layout = nullptr;
        ThreadSafePerformCommandBuffers();
    }

    void Graphics::Present()
    {
        if (Extent.width < 1 || Extent.height < 1)
        {
            Resize();
            if (Extent.width < 1 || Extent.height < 1) return;
        }

        vk::ResultValue<uint32_t> image_result = GPU.acquireNextImageKHR(
            VulkanSwapchain, 
            std::numeric_limits<std::uint64_t>::max(), 
            ImageAvailableSemaphores[CurrentSemaphore],
            nullptr);

        std::array<vk::PipelineStageFlags, 1> wait_stages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        auto submit_info = vk::SubmitInfo()
            .setWaitSemaphoreCount(1)
            .setPWaitSemaphores(&ImageAvailableSemaphores[CurrentSemaphore])
            .setPWaitDstStageMask(wait_stages.data())
            .setCommandBufferCount(1)
            .setPCommandBuffers(&CommandBuffers[image_result.value])
            .setSignalSemaphoreCount(1)
            .setPSignalSemaphores(&RenderFinishedSemaphores[image_result.value]);

        GPU.waitForFences(CommandFinishedFences[image_result.value], true, std::numeric_limits<std::uint64_t>::max());
        std::unique_lock lock(CommandBufferMutex); // Where it should be???
        GPU.resetFences(CommandFinishedFences[image_result.value]);
        GraphicsQueue.submit(submit_info, CommandFinishedFences[image_result.value]);
        lock.unlock();

        vk::Result result;
        auto present_info = vk::PresentInfoKHR()
            .setWaitSemaphoreCount(1)
            .setPWaitSemaphores(&RenderFinishedSemaphores[image_result.value])
            .setSwapchainCount(1)
            .setPSwapchains(&VulkanSwapchain)
            .setPImageIndices(&image_result.value)
            .setPResults(&result);
        VkResult present_result = vkQueuePresentKHR(static_cast<VkQueue>(PresentQueue), reinterpret_cast<VkPresentInfoKHR*>(&present_info));
        if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR) Resize();
        else if (present_result != VK_SUCCESS) vk::throwResultException(static_cast<vk::Result>(present_result), VULKAN_HPP_NAMESPACE_STRING"::Queue::presentKHR");

        CurrentSemaphore = (CurrentSemaphore + 1) % Images.size();
    }
}