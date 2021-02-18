#include "CommandLayout.hpp"
#include "Graphics.hpp"
#include "Command.hpp"



namespace airsvk
{
	void CommandLayout::Perform(vk::CommandBuffer cmdb, airs::vec2ui extent)
	{
		for (Command* cmd : Commands)
		{
			cmd->Extent = extent;
			cmd->Perform(cmdb);
		}
	}
	void CommandLayout::Rebuild()
	{
		if (Owner) Owner->ThreadSafePerformCommandBuffers();
	}

	CommandLayout::~CommandLayout()
	{
		Clear();
	}

	void CommandLayout::Add(Command& cmd)
	{
		Commands.push_back(&cmd);
		cmd.Layout = this;
		Rebuild();
	}
	void CommandLayout::Remove(Command& cmd)
	{
		std::size_t j = 0;
		for (std::size_t i = 0; i < Commands.size(); i++)
			if (Commands[i] != &cmd) Commands[j++] = Commands[i];
		cmd.Layout = nullptr;
		Commands.resize(j);
		Rebuild();
	}
	void CommandLayout::RemoveAt(std::size_t index)
	{
		Commands[index]->Layout = nullptr;
		Commands.erase(Commands.begin() + index);
		Rebuild();
	}
	void CommandLayout::Clear()
	{
		for (Command* cmd : Commands) cmd->Layout = nullptr;
		Commands.clear();
		Rebuild();
	}
}