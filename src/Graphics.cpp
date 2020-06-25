#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX
#include "Graphics.hpp"
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
    vk::Extent2D FindExtent(vk::PhysicalDevice physical_gpu, vk::SurfaceKHR surface, uint32_t w, uint32_t h)
    {
        vk::SurfaceCapabilitiesKHR capabilities = physical_gpu.getSurfaceCapabilitiesKHR(surface);
        return vk::Extent2D()
            .setWidth(std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, w)))
            .setHeight(std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, h)));
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
            .setApiVersion(VK_API_VERSION_1_0);
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
            if (PhysicalGPUSuitable(VulkanSurface, device, required_extensions))
            {
                if (!PhysicalGPU) PhysicalGPU = device;
                else
                {
                    vk::PhysicalDeviceProperties properties = device.getProperties();
                    vk::PhysicalDeviceProperties current_properties = PhysicalGPU.getProperties();

                    switch (properties.deviceType)
                    {
                    case vk::PhysicalDeviceType::eIntegratedGpu:
                        if (current_properties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu)
                        {
                            PhysicalGPU = device;
                        }
                        break;
                    case vk::PhysicalDeviceType::eDiscreteGpu:
                        if (true)
                        {
                            PhysicalGPU = device;
                        }
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
        std::vector<vk::QueueFamilyProperties> QueueFamilies = PhysicalGPU.getQueueFamilyProperties();
        GraphicsQueueFamilyIndex = UINT32_MAX;
        PresentQueueFamilyIndex = UINT32_MAX;
        for (uint32_t i = 0; i < QueueFamilies.size(); i++)
        {
            if (QueueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                if (GraphicsQueueFamilyIndex == UINT32_MAX)
                    GraphicsQueueFamilyIndex = i;

                if (PhysicalGPU.getSurfaceSupportKHR(i, VulkanSurface))
                {
                    GraphicsQueueFamilyIndex = i;
                    PresentQueueFamilyIndex = i;
                    break;
                }
            }
        }
        if (PresentQueueFamilyIndex == UINT32_MAX)
        {
            for (uint32_t i = 0; i < QueueFamilies.size(); ++i)
            {
                if (PhysicalGPU.getSurfaceSupportKHR(i, VulkanSurface))
                {
                    PresentQueueFamilyIndex = i;
                    break;
                }
            }
        }
        if (GraphicsQueueFamilyIndex == UINT32_MAX || PresentQueueFamilyIndex == UINT32_MAX)
            throw std::runtime_error("Vulkan initialization error: Could not find both graphics and present queues.");

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
        GPU = PhysicalGPU.createDevice(deviceInfo, nullptr);

        GraphicsQueue = GPU.getQueue(GraphicsQueueFamilyIndex, 0);
        PresentQueue = GPU.getQueue(PresentQueueFamilyIndex, 0);

        vk::SemaphoreCreateInfo semaphore_info;
        ImageAvailableSemaphore = GPU.createSemaphore(semaphore_info);
        RenderFinishedSemaphore = GPU.createSemaphore(semaphore_info);

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

    void Graphics::InitSurfaceExtent(uint32_t w, uint32_t h)
    {
        Extent = FindExtent(PhysicalGPU, VulkanSurface, w, h);
        Viewport = vk::Viewport(0.0f, 0.0f,
            static_cast<float>(Extent.width),
            static_cast<float>(Extent.height),
            0.0f, 1.0f);
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

        Swapchain = GPU.createSwapchainKHR(create_info);
        Images = GPU.getSwapchainImagesKHR(Swapchain);
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
    void Graphics::InitFrameBuffers()
    {
        Framebuffers.resize(ImageViews.size());
        for (size_t i = 0; i < ImageViews.size(); i++)
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
    void Graphics::InitCommandBuffers()
    {
        if (Framebuffers.size() != CommandBuffers.size())
        {
            GPU.freeCommandBuffers(CommandPool, CommandBuffers);
            vk::CommandBufferAllocateInfo allocate_info(
                CommandPool, vk::CommandBufferLevel::ePrimary, 
                static_cast<uint32_t>(Framebuffers.size()));
            CommandBuffers = GPU.allocateCommandBuffers(allocate_info);
        }

        vk::Rect2D RenderArea({ 0, 0 }, Extent);

        for (size_t i = 0; i < CommandBuffers.size(); i++)
        {
            vk::CommandBuffer cmdb = CommandBuffers[i];

            vk::ClearValue clear_color;
            clear_color.color.float32[0] = 0.0f;
            clear_color.color.float32[1] = 0.0f;
            clear_color.color.float32[2] = 0.0f;
            clear_color.color.float32[3] = 1.0f;
            auto render_pass_begin_info = vk::RenderPassBeginInfo()
                .setRenderPass(RenderPass)
                .setFramebuffer(Framebuffers[i])
                .setRenderArea(RenderArea)
                .setClearValueCount(1)
                .setPClearValues(&clear_color);
            cmdb.begin(vk::CommandBufferBeginInfo());
            cmdb.setViewport(0, 1, &Viewport);
            cmdb.setScissor(0, 1, &RenderArea);
            cmdb.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);
            if (CommandBuffer) CommandBuffer(cmdb);
            cmdb.endRenderPass();
            cmdb.end();
        }
    }

    Graphics::Graphics(const airs::Window& window, std::vector<const char*> layers, vk::Format prefered_format, vk::ColorSpaceKHR prefered_color_space,
		vk::PresentModeKHR prefered_present_mode, uint32_t prefered_image_count) 
	{
        std::vector<const char*> extensions = {
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
            VK_KHR_SURFACE_EXTENSION_NAME,
        };

        InitVulkanInstance(layers, extensions);
        InitSurface(window);
        InitPhysicalGPU({ VK_KHR_SWAPCHAIN_EXTENSION_NAME });
        InitSurfaceProperties(vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear, vk::PresentModeKHR::eFifo, 2);
        InitGPU({ VK_KHR_SWAPCHAIN_EXTENSION_NAME });
        InitRenderPass();
	}
    Graphics::~Graphics()
    {
        GPU.waitIdle();
        GPU.destroySemaphore(ImageAvailableSemaphore);
        GPU.destroySemaphore(RenderFinishedSemaphore);
        for (auto framebuffer : Framebuffers)
            GPU.destroyFramebuffer(framebuffer);
        for (auto image_view : ImageViews)
            GPU.destroyImageView(image_view);
        GPU.destroySwapchainKHR(Swapchain);
        GPU.destroyRenderPass(RenderPass);
        GPU.destroyCommandPool(CommandPool);
        GPU.destroy();
        VulkanInstance.destroySurfaceKHR(VulkanSurface);
        VulkanInstance.destroy();
    }

    void Graphics::Resize(int32_t w, int32_t h)
    {
        GPU.waitIdle();

        for (auto framebuffer : Framebuffers)
            GPU.destroyFramebuffer(framebuffer);
        for (auto image_view : ImageViews)
            GPU.destroyImageView(image_view);
        GPU.destroySwapchainKHR(Swapchain);

        InitSurfaceExtent(static_cast<uint32_t>(w), static_cast<uint32_t>(h));
        InitSwapchain();
        InitImageViews();
        InitFrameBuffers();
        InitCommandBuffers();
    }
    
    Shader Graphics::CreateShader(const std::vector<int8_t>& code) const
    {
        return std::move(Shader(GPU, vk::ShaderModuleCreateInfo({}, code.size(), reinterpret_cast<const uint32_t*>(code.data()))));
    }
    Pipeline Graphics::CreatePipeline(const vk::PipelineLayoutCreateInfo& layout_info, const vk::GraphicsPipelineCreateInfo& info) const
    {
        return std::move(Pipeline(GPU, RenderPass, layout_info, info));
    }
    Pipeline Graphics::CreatePipeline(const vk::PipelineLayoutCreateInfo& layout_info, const vk::ComputePipelineCreateInfo& info) const
    {
        return std::move(Pipeline(GPU, layout_info, info));
    }
    Buffer Graphics::CreateBuffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags required, vk::SharingMode mode)
    {
        return Buffer(GPU, GPUMemoryProperties, vk::BufferCreateInfo({}, size, usage, mode), required);
    }
    
    void Graphics::Present()
    {
        auto compound_result = GPU.acquireNextImageKHR(Swapchain, UINT64_MAX, ImageAvailableSemaphore, nullptr);
        uint32_t image_index = compound_result.value;

        std::array<vk::PipelineStageFlags, 1> wait_stages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        auto submit_info = vk::SubmitInfo()
            .setWaitSemaphoreCount(1)
            .setPWaitSemaphores(&ImageAvailableSemaphore)
            .setPWaitDstStageMask(wait_stages.data())
            .setCommandBufferCount(1)
            .setPCommandBuffers(&CommandBuffers[image_index])
            .setSignalSemaphoreCount(1)
            .setPSignalSemaphores(&RenderFinishedSemaphore);

        GraphicsQueue.waitIdle();

        vk::Result result = GraphicsQueue.submit(1, &submit_info, nullptr);

        auto present_info = vk::PresentInfoKHR()
            .setWaitSemaphoreCount(1)
            .setPWaitSemaphores(&RenderFinishedSemaphore)
            .setSwapchainCount(1)
            .setPSwapchains(&Swapchain)
            .setPImageIndices(&image_index)
            .setPResults(&result);

        result = PresentQueue.presentKHR(present_info);
    }
}