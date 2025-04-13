#pragma once
#include "forge/ecs/ecs.hpp"
#include "forge/graphics/loaders/mesh_load_options.hpp"

namespace forge
{
	struct RenderObject;
}

class MeshRendererComponent : public forge::IComponent
{
public:
	// EXPORT_FIELDS(
	// 	forge::FieldSeperator{"Material"}
	// 	forge::ButtonField("load mesh",
	// [&]
	// {
	//
	// }));

	forge::Array<forge::ComponentField> export_fields() override;

	void set_mesh(std::string_view path, forge::MeshLoadOptions options = {});

	void reset_mesh();

	[[nodiscard]]
	inline bool has_mesh() const
	{
		return m_object != nullptr;
	}

protected:
	forge::RenderObject *m_object = nullptr;
	u32 m_transform_update_con;

	void on_destroy() override;
	void on_enabled() override;
	void on_disabled() override;
};
