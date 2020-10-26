#include "CommandLayout.hpp"
#include "Command.hpp"



namespace airsvk
{
	void CommandLayout::Perform(vk::CommandBuffer cmdb)
	{
		for (Command* cmd : Commands) cmd->Perform(cmdb);
	}
	void CommandLayout::Rebuild()
	{
		if (Rebuilded) Rebuilded();
	}

	CommandLayout::~CommandLayout()
	{
		Clear();
	}

	void CommandLayout::Add(Command& cmd)
	{
		Commands.push_back(&cmd);
		cmd.Rebuilded.bind<CommandLayout, &CommandLayout::Rebuild>(this);
		Rebuild();
	}
	void CommandLayout::Remove(Command& cmd)
	{
		std::size_t j = 0;
		for (std::size_t i = 0; i < Commands.size(); i++)
			if (Commands[i] != &cmd) Commands[j++] = Commands[i];
		cmd.Rebuilded.clear();
		Commands.resize(j);
		Rebuild();
	}
	void CommandLayout::RemoveAt(std::size_t index)
	{
		Commands[index]->Rebuilded.clear();
		Commands.erase(Commands.begin() + index);
		Rebuild();
	}
	void CommandLayout::Clear()
	{
		for (Command* cmd : Commands) cmd->Rebuilded.clear();
		Commands.clear();
		Rebuild();
	}
}