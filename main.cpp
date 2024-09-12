#include <iostream>
#include <thread>
#include <typeindex>

#include "core/engine.hpp"
#include "src/ecs/ecs.hpp"
#include "src/events/signal.hpp"
#include "src/fmt/fmt.hpp"
#include "src/memory/mem_pool.hpp"
#include "src/util/types.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "src/system/window.hpp"
#include "src/system/window_sub_system.hpp"
#include "type_box/type_box.hpp"

int main()
{
	// auto &engine = forge::Engine::get_instance();
	//
	// engine.init(
	// {
	// 	.window_title = "ByteForge Engine",
	// 	.window_width = 720,
	// 	.window_height = 480,
	// });
	//
	// engine.run();
	//
	// engine.shutdown();

	forge::TypeBox type_box;

	std::thread
	{
		[&type_box]
		{
			auto i = 0;

			while (true)
			{
				type_box.emplace<std::string>(std::string{"hello #"} + std::to_string(i++));
			}
		}
	}.detach();

	std::thread
	{
		[&type_box]
		{
			while (true)
			{
				auto opt = type_box.fetch<std::string>();

				if (opt.has_value())
				{
					auto &funnel = opt.value();

					for (int i = 0; i < funnel.size(); i++)
					{
						auto *value = funnel.next();

						if (value == nullptr)
						{
							continue;
						}

						std::cout << "got from thread " << *value << '\n';
					}
				}
			}
		}
	}.detach();

	while (true)
	{
		auto opt = type_box.fetch<std::string>(false);

		if (opt.has_value())
		{
			auto &funnel = opt.value();

			for (int i = 0; i < funnel.size(); i++)
			{
				auto *value = funnel.next();

				if (value == nullptr)
				{
					continue;
				}

				std::cout << *value << '\n';
			}

			type_box.clear_all();
		}
	}
}
