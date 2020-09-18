#include <airs/Font.hpp>
#include <unordered_map>
#include <array>
#include "Graphics.hpp"



namespace airsvk
{
	struct Glyph : airs::Glyph::Metrics
	{
		std::int32_t TextureInd;
	};
	
	class Charset
	{
		friend class Graphics;
	private:
		const Graphics& OwnerGraphics;
		const airs::Font& Font;
		//Pipeline* Pipeline;
		//Buffer UniformBuffer;
		Buffer TexelBuffer;
		BufferView TexelBufferView;

		static constexpr std::size_t BasicFirst = 0;
		static constexpr std::size_t BasicLast = 255;
		const bool GrayScale;
		std::array<Glyph, BasicLast - BasicFirst + 1> BasicGlyphs;
		std::unordered_map<char32_t, Glyph> AdditionalGlyphs;

		void InitBuffer(std::size_t size);
		void InitBasicGlyphs();

		Charset(const Graphics& graphics, const airs::Font& font, bool gray);

	public:

		const Glyph& operator[](char32_t i);
	};
}