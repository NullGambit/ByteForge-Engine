#include "mesh.hpp"

forge::MeshView::MeshView(Mesh &mesh) :
	name(mesh.name),
	vertices(mesh.vertices),
	indices(mesh.indices),
	materials(mesh.materials)
{}
