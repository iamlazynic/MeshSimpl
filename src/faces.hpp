//
// Created by nickl on 5/25/19.
//

#ifndef MESH_SIMPL_FACES_HPP
#define MESH_SIMPL_FACES_HPP

#include <array>
#include <cassert>
#include <memory>
#include <utility>
#include <vector>

#include "erasable.hpp"
#include "types.hpp"

namespace MeshSimpl {
namespace Internal {

class Vertices;
class Edge;

class Faces : public Erasables {
 private:
  Indices _indices;

  struct Face {
    vec3i& vvv;
    std::array<Edge*, 3> sides;
    explicit Face(vec3i& corners) : vvv(corners), sides() {}
  };

  std::vector<Face> _faces;

 public:
  // Embed indices and allocate space for sides
  explicit Faces(Indices& indices)
      : Erasables(indices.size()), _indices(std::move(indices)), _faces() {
    _faces.reserve(size());
    for (idx f = 0; f < size(); ++f) _faces.emplace_back(_indices[f]);
  }

  // Get/set side: edge of a face
  Edge* side(idx f, order ord) const {
    assert(exists(f));
    return _faces[f].sides[ord];
  }
  void setSide(idx f, order ord, Edge* edge) {
    assert(exists(f));
    _faces[f].sides[ord] = edge;
  }
  Edge* edgeAcrossFrom(idx f, idx v) const { return side(f, orderOf(f, v)); }

  // Get/set vertex index
  idx v(idx f, order ord) const {
    assert(exists(f));
    return indices(f)[ord];
  }
  void setV(idx f, order ord, idx v) {
    assert(exists(f));
    _faces[f].vvv[ord] = v;
  }
  const vec3i& indices(idx f) const {
    assert(exists(f));
    return _faces[f].vvv;
  }
  const vec3i& operator[](idx f) const { return indices(f); }

  bool onBoundary(idx f) const;

  vec3d vPos(idx f, order k, const Vertices& vertices) const;

  vec3d edgeVec(idx f, order k, const Vertices& vertices) const;

  void compactIndicesAndDie(Indices& indices);

  // Return the order of a vertex v in some face f
  order orderOf(idx f, idx v) const {
    if (v == indices(f)[0])
      return 0;
    else if (v == indices(f)[1])
      return 1;
    else if (v == indices(f)[2])
      return 2;
    else
      assert(false);
    return INVALID;
  }
};

}  // namespace Internal
}  // namespace MeshSimpl

#endif  // MESH_SIMPL_FACES_HPP
