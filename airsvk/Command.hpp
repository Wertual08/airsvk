#pragma once
#include <vulkan/vulkan.hpp>



namespace airsvk
{
	class Command
	{
		friend class CommandLayout;
	private:
		airs::delegate<void()> Rebuilded;
		
	protected:
		void Rebuild() const { if (Rebuilded) Rebuilded(); }
		virtual void Perform(vk::CommandBuffer cmdb) = 0;
	};
}