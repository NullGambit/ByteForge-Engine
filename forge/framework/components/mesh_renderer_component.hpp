#pragma once
#include "../../ecs/ecs.hpp"
#include "../../graphics/loaders/mesh_loader.hpp"
#include "../../graphics/ogl_renderer/ogl_texture.hpp"
#include "forge/graphics/ogl_renderer/ogl_renderer.hpp"

namespace forge
{
	struct RenderObject;

	enum class PrimitiveMeshType : u8
	{
		None,
		Cube,
		Sphere,
		Cone,
		Cylinder,
	};

	class MeshRendererComponent : public IComponent
	{
	public:

		Array<ComponentField> export_fields() override;

		void set_mesh(std::string_view path, MeshLoadOptions options = {});
		void set_mesh(RenderObjectTree &tree);
		void set_mesh(PrimitiveMeshType type);

		bool set_texture(std::string_view path, u32 type, TextureOptions options = {}) const;

		void reset_mesh();

		[[nodiscard]]
		inline bool has_mesh() const
		{
			return object != nullptr;
		}

		RenderObject *object = nullptr;

	protected:
		u32 m_transform_update_con;

		void on_destroy() override;
		void on_enabled() override;
		void on_disabled() override;

		void set_transform_update_con();
	};
}

