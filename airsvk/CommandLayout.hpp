#pragma once
#include <vulkan/vulkan.hpp>
#include <airs/delegate.hpp>
#include <vector>



namespace airsvk
{
	class Command;
	class CommandLayout
	{
		friend class Graphics;
	private:
		std::vector<Command*> Commands;

		airs::delegate<void()> Rebuilded;

		void Perform(vk::CommandBuffer cmdb);
		void Rebuild();

	public:
		~CommandLayout();

		void Add(Command& cmd);
		void Remove(Command& cmd);
		void RemoveAt(std::size_t index);
		void Clear();
	};
}