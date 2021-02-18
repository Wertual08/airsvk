#pragma once
#include "airs_vulkan.hpp"
#include <airs/math.hpp>
#include <vector>



namespace airsvk
{
	class Command;
	class Graphics;
	class CommandLayout
	{
		friend class Graphics;
	private:
		std::vector<Command*> Commands;

		Graphics *Owner = nullptr;

		void Perform(vk::CommandBuffer cmdb, airs::vec2ui extent);

	public:
		~CommandLayout();

		void Rebuild();

		void Add(Command& cmd);
		void Remove(Command& cmd);
		void RemoveAt(std::size_t index);
		void Clear();
	};
}