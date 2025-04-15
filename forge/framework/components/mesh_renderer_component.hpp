#pragma once
#include "../../ecs/ecs.hpp"
#include "../../graphics/loaders/mesh_loader.hpp"
#include "../../graphics/ogl_renderer/ogl_texture.hpp"
#include "forge/graphics/ogl_renderer/ogl_renderer.hpp"

namespace forge
{
	struct RenderObject;
}

class MeshRendererComponent : public forge::IComponent
{
public:

	forge::Array<forge::ComponentField> export_fields() override;

	void set_mesh(std::string_view path, forge::MeshLoadOptions options = {});
	void set_mesh(forge::RenderObjectTree &tree);

	bool set_texture(std::string_view path, u32 type, forge::TextureOptions options = {}) const;

	void reset_mesh();

	[[nodiscard]]
	inline bool has_mesh() const
	{
		return m_object != nullptr;
	}

	forge::RenderObject *m_object = nullptr;

protected:
	u32 m_transform_update_con;

	void on_destroy() override;
	void on_enabled() override;
	void on_disabled() override;
};
