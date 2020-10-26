#include "airsvk/Charset.hpp"



namespace airsvk
{
	void Charset::InitBuffer(std::size_t size)
	{
		TexelBuffer = OwnerGraphics.CreateBuffer(size,
			vk::BufferUsageFlagBits::eUniformTexelBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible |
			vk::MemoryPropertyFlagBits::eHostCoherent);
		TexelBufferView = TexelBuffer.CreateView(vk::Format::eR8Uint, 0, size);
	}
	void Charset::InitBasicGlyphs()
	{
        std::int32_t capacity = 0;
        for (std::size_t i = BasicFirst; i <= BasicLast; i++)
            capacity += Font.Size(i, GrayScale) + sizeof(std::int32_t) * 2;

        InitBuffer(capacity);

        std::uint8_t* data = TexelBuffer.Map<std::uint8_t>();
        std::int32_t cursor = 0;

        for (char32_t i = static_cast<char32_t>(BasicFirst); i <= static_cast<char32_t>(BasicLast); i++)
        {
            airs::Glyph glyph = Font.GetGlyph(i, GrayScale);

            BasicGlyphs[static_cast<std::size_t>(i) - BasicFirst] = { glyph.GetMetrics(), cursor };

            *reinterpret_cast<std::int32_t*>(data + cursor) = glyph.BitmapW();
            cursor += sizeof(std::int32_t);
            *reinterpret_cast<std::int32_t*>(data + cursor) = glyph.BitmapH();
            cursor += sizeof(std::int32_t);
            std::memcpy(data + cursor, glyph, glyph.Size());
            cursor += glyph.Size();
        }

        TexelBuffer.Unmap();
	}
	
	Charset::Charset(const Graphics& graphics, const airs::Font& font, bool gray) :
		OwnerGraphics(graphics), Font(font), GrayScale(gray)
	{
		InitBasicGlyphs();
	}

    const Glyph& Charset::operator[](char32_t i)
    {
        if (i >= BasicFirst && i <= BasicLast) return BasicGlyphs[i - BasicFirst];
        else
        {
            auto it = AdditionalGlyphs.find(i);
            if (it != AdditionalGlyphs.end()) return it->second;
            else
            {
                // load
            }
        }
    }
}