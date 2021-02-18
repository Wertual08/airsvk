#pragma once
#include "airs_vulkan.hpp"
#include <airs/math.hpp>
#include <type_traits>
#include <array>



namespace airsvk
{
    template<typename T> struct formats { static constexpr vk::Format value = vk::Format::eUndefined; };
    template <class T> struct formats<const T> : public formats<T> {};
    template <class T> struct formats<volatile T> : public formats<T> {};
    template <class T> struct formats<const volatile T> : public formats<T> {};

    template<> struct formats<std::int8_t> { static constexpr vk::Format value = vk::Format::eR8Sint; };
    template<> struct formats<std::uint8_t> { static constexpr vk::Format value = vk::Format::eR8Uint; };
    template<> struct formats<std::int16_t> { static constexpr vk::Format value = vk::Format::eR16Sint; };
    template<> struct formats<std::uint16_t> { static constexpr vk::Format value = vk::Format::eR16Uint; };
    template<> struct formats<std::int32_t> { static constexpr vk::Format value = vk::Format::eR32Sint; };
    template<> struct formats<std::uint32_t> { static constexpr vk::Format value = vk::Format::eR32Uint; };
    template<> struct formats<std::int64_t> { static constexpr vk::Format value = vk::Format::eR64Sint; };
    template<> struct formats<std::uint64_t> { static constexpr vk::Format value = vk::Format::eR64Uint; };
    template<> struct formats<float> { static constexpr vk::Format value = vk::Format::eR32Sfloat; };
    template<> struct formats<double> { static constexpr vk::Format value = vk::Format::eR64Sfloat; };

    template<> struct formats<airs::vec2b> { static constexpr vk::Format value = vk::Format::eR8G8Sint; };
    template<> struct formats<airs::vec2ub> { static constexpr vk::Format value = vk::Format::eR8G8Uint; };
    template<> struct formats<airs::vec2s> { static constexpr vk::Format value = vk::Format::eR16G16Sint; };
    template<> struct formats<airs::vec2us> { static constexpr vk::Format value = vk::Format::eR16G16Uint; };
    template<> struct formats<airs::vec2i> { static constexpr vk::Format value = vk::Format::eR32G32Sint; };
    template<> struct formats<airs::vec2ui> { static constexpr vk::Format value = vk::Format::eR32G32Uint; };
    template<> struct formats<airs::vec2l> { static constexpr vk::Format value = vk::Format::eR64G64Sint; };
    template<> struct formats<airs::vec2ul> { static constexpr vk::Format value = vk::Format::eR64G64Uint; };
    template<> struct formats<airs::vec2f> { static constexpr vk::Format value = vk::Format::eR32G32Sfloat; };
    template<> struct formats<airs::vec2d> { static constexpr vk::Format value = vk::Format::eR64G64Sfloat; };

    template<> struct formats<airs::vec3b> { static constexpr vk::Format value = vk::Format::eR8G8B8Sint; };
    template<> struct formats<airs::vec3ub> { static constexpr vk::Format value = vk::Format::eR8G8B8Uint; };
    template<> struct formats<airs::vec3s> { static constexpr vk::Format value = vk::Format::eR16G16B16Sint; };
    template<> struct formats<airs::vec3us> { static constexpr vk::Format value = vk::Format::eR16G16B16Uint; };
    template<> struct formats<airs::vec3i> { static constexpr vk::Format value = vk::Format::eR32G32B32Sint; };
    template<> struct formats<airs::vec3ui> { static constexpr vk::Format value = vk::Format::eR32G32B32Uint; };
    template<> struct formats<airs::vec3l> { static constexpr vk::Format value = vk::Format::eR64G64B64Sint; };
    template<> struct formats<airs::vec3ul> { static constexpr vk::Format value = vk::Format::eR64G64B64Uint; };
    template<> struct formats<airs::vec3f> { static constexpr vk::Format value = vk::Format::eR32G32B32Sfloat; };
    template<> struct formats<airs::vec3d> { static constexpr vk::Format value = vk::Format::eR64G64B64Sfloat; };

    template<> struct formats<airs::vec4b> { static constexpr vk::Format value = vk::Format::eR8G8B8A8Sint; };
    template<> struct formats<airs::vec4ub> { static constexpr vk::Format value = vk::Format::eR8G8B8A8Uint; };
    template<> struct formats<airs::vec4s> { static constexpr vk::Format value = vk::Format::eR16G16B16A16Sint; };
    template<> struct formats<airs::vec4us> { static constexpr vk::Format value = vk::Format::eR16G16B16A16Uint; };
    template<> struct formats<airs::vec4i> { static constexpr vk::Format value = vk::Format::eR32G32B32A32Sint; };
    template<> struct formats<airs::vec4ui> { static constexpr vk::Format value = vk::Format::eR32G32B32A32Uint; };
    template<> struct formats<airs::vec4l> { static constexpr vk::Format value = vk::Format::eR64G64B64A64Sint; };
    template<> struct formats<airs::vec4ul> { static constexpr vk::Format value = vk::Format::eR64G64B64A64Uint; };
    template<> struct formats<airs::vec4f> { static constexpr vk::Format value = vk::Format::eR32G32B32A32Sfloat; };
    template<> struct formats<airs::vec4d> { static constexpr vk::Format value = vk::Format::eR64G64B64A64Sfloat; };


    template<typename... Attributes>
    class VertexCreateInfo
    {
    private:
        std::array<vk::VertexInputBindingDescription, 1> binding_descriptions;
        std::array<vk::VertexInputAttributeDescription, sizeof...(Attributes)> attribute_descriptions;

        template<typename T>
        constexpr void insert_member(std::uint32_t& count, std::uint32_t& size)
        {
            attribute_descriptions[count] = vk::VertexInputAttributeDescription(count, 0, formats<T>::value, size);
            count++;
            size += sizeof(T);
        }
        constexpr void forward_attributes(std::uint32_t& count, std::uint32_t& size)
        {
        }
        template<typename T>
        constexpr void forward_attributes(std::uint32_t& count, std::uint32_t& size)
        {
            if constexpr (std::is_same<T, void>::value) size++;
            else insert_member<T>(count, size);
        }
        template<typename T, typename C, typename... Ts>
        constexpr void forward_attributes(std::uint32_t& count, std::uint32_t& size)
        {
            if constexpr (std::is_same<T, void>::value) size++;
            else insert_member<T>(count, size);
            forward_attributes<C, Ts...>(count, size);
        }
        constexpr VertexCreateInfo()
        {
            std::uint32_t count = 0;
            std::uint32_t size = 0;
            forward_attributes<Attributes...>(count, size);
            binding_descriptions = {
                vk::VertexInputBindingDescription(0, size, vk::VertexInputRate::eVertex)
            };
        }

        static vk::PipelineVertexInputStateCreateInfo get()
        {
            static VertexCreateInfo instance;
            return vk::PipelineVertexInputStateCreateInfo({},
                static_cast<std::uint32_t>(instance.binding_descriptions.size()), instance.binding_descriptions.data(),
                static_cast<std::uint32_t>(instance.attribute_descriptions.size()), instance.attribute_descriptions.data()
            );
        }

    public:
        static const vk::PipelineVertexInputStateCreateInfo value;
    };

    template<typename... Attributes>
    const vk::PipelineVertexInputStateCreateInfo VertexCreateInfo<Attributes...>::value = VertexCreateInfo<Attributes...>::get();
}