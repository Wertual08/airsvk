#pragma once
#include <airs/math.hpp>
#include "airs_vulkan.hpp"
#include "CommandLayout.hpp"



namespace airsvk
{
	class Command
	{
		friend class CommandLayout;
	private:
		CommandLayout *Layout = nullptr;
		airs::vec2ui Extent;
		
	protected:
		void Rebuild() { if (Layout) Layout->Rebuild(); }
		airs::vec2ui GetExtent() const { return Extent; }
		virtual void Perform(vk::CommandBuffer cmdb) = 0;
	};
}