

#include <iostream>
#include <forge/framework/components/mesh_primitive_component.hpp>

#include "cpu_benchmark.hpp"
#include "forge/editor/editor_subsystem.hpp"

#include "forge/util/random.hpp"

struct RenderObject
{
	u32 flags = 0;
	forge::Material material;
	glm::mat4 normal_matrix {5.0};
	glm::mat4 model {1.0};
	u32 id;

	void compute_model(const glm::mat4 &new_model)
	{
		model = new_model;
		normal_matrix = glm::transpose(glm::inverse(model));
	}
};

struct RenderDataSoa
{
	forge::Array<char[120]> padding;
	forge::Array<bool> is_available;
};

struct RenderData
{
	RenderObject object;
	char padding[120];
	bool is_available;
};

int main()
{
	Benchmarker bench;

	static forge::MemPool render_objects;
	static RenderDataSoa data_soa;
	static forge::MemPool data;

	constexpr auto COUNT = 5000;
	constexpr size_t FREED = COUNT / 2.5f;

	render_objects.init<RenderObject>(sizeof(RenderObject) * COUNT);

	for (auto i = 0; i < COUNT; i++)
	{
		auto [obj, id] = render_objects.emplace<RenderObject>();

		obj->id = id;

		data_soa.is_available.emplace_back(true);
	}

	srand(1);

	for (auto i = 0; i < FREED; i++)
	{
		auto index = rand() % COUNT - 1;

		render_objects.free_at(index);

		data_soa.is_available[index] = false;
	}

	data.init<RenderData>(sizeof(RenderData) * COUNT);

	for (auto i = 0; i < COUNT; i++)
	{
		auto [rd, id] = data.emplace<RenderData>();

		rd->object.id = id;
		rd->is_available = true;
	}

	srand(1);

	for (auto i = 0; i < FREED; i++)
	{
		auto index = rand() % COUNT - 1;

		auto *rd = data.get_from_index<RenderData>(index);
		rd->is_available = false;
		data.free_at(index);
	}

	bench.cases.emplace_back("SOA",
	[]
	{
		for (size_t i = 0; i < render_objects.get_length(); i++)
		{
			if (data_soa.is_available[i])
			{
				auto *obj = render_objects.get_from_index<RenderObject>(i);
				auto model = glm::inverse(obj->model);
				obj->compute_model(model);
			}
		}
	});

	bench.cases.emplace_back("Normal",
	[]
	{
		for (auto &rd : data.get_iterator<RenderData>())
		{
			if (rd.is_available)
			{
				auto model = glm::inverse(rd.object.model);
				rd.object.compute_model(model);
			}
		}
	});

	bench.run_cases(100);
	bench.display_results();
}
