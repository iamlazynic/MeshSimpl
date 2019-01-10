#ifndef LIB_MESH_SIMPL_ECOL_H
#define LIB_MESH_SIMPL_ECOL_H

#include "qem_heap.h"
#include "types.h"
#include "util.h"

namespace MeshSimpl {

namespace Internal {

static const size_t MIN_NR_VERTICES = 4;
static const double FOLD_OVER_COS_ANGLE = std::cos(160);

// Set error and center of edge by choosing a position to collapse into
void optimal_ecol_vertex_placement(const V& vertices, Edge& edge);

// Set quadric, error, and collapse center for a given edge
void set_edge_error(const V& vertices, const Q& quadrics, Edge& edge);

// Compute quadric error for every edge at initialization
void compute_errors(const V& vertices, const Q& quadrics, E& edges);

// Recompute quadric error for evert edge with idx in range
void recompute_errors(const V& vertices, const Q& quadrics, E& edges,
                      std::vector<idx>::const_iterator edge_idx_begin,
                      std::vector<idx>::const_iterator edge_idx_end);

// Returns true if the movement of vertex will cause this face to flip too much to accept
bool face_fold_over(const V& vertices, const idx v0, const idx v1, const idx v2,
                    const vec3d& v2_new_pos);

// Replace a vertex in edge, and update other members then fix priority in heap
void update_error_and_center(const V& vertices, const Q& quadrics, QEMHeap& heap, Edge* edge_ptr);

void iter_next(const F& indices, const E& edges, const F2E& face2edge, idx& f, idx& v, idx& e);

// Find relevant faces one by one around the collapsed edge.
// For each face around v_del/v_kept, a 3-tuple called `fve' is stored:
//      `f' - idx of face;
//      `e' - local idx (0/1/2) of the edge to this face, shared by this face and previous face';
//      `v' - local idx (0/1/2) of the vertex to this face, on edge `e' and not centered vertex;
// Meanwhile this function does geometry and connectivity check to avoid fold-over faces and
// non-manifold edges.
// Outputs: star_v_del and star_v_kept are filled with `fve' of faces found.
// Returns: true if no problem is found -- this edge collapse operation is acceptable
bool scan_neighbors(const V& vertices, const F& indices, const E& edges, const F2E& face2edge,
                    const Edge& edge, std::vector<vec3i>& fve_star_v_del,
                    std::vector<idx>& e_star_v_kept);

inline idx vi_in_face(const F& indices, const idx f, const idx v) {
    assert(indices[f][0] == v || indices[f][1] == v || indices[f][2] == v);
    return indices[f][0] == v ? 0 : (indices[f][1] == v ? 1 : 2);
}

inline idx fi_in_edge(const Edge& edge, const idx f) {
    assert(edge.faces[0] == f || edge.faces[1] == f);
    return edge.faces[0] == f ? 0 : 1;
};

inline idx fve_center(const vec3i& fve) { return 3 - fve[1] - fve[2]; }

inline idx choose_v_del(const Edge& edge) {
    assert(edge.boundary_v != BOUNDARY_V::BOTH);
    return edge.boundary_v == BOUNDARY_V::NONE ? 0 : static_cast<idx>(1 - edge.boundary_v);
}

// Returns true if edge is collapsed
bool collapse_interior_edge(V& vertices, F& indices, E& edges, F2E& face2edge, Q& quadrics,
                            QEMHeap& heap, idx ecol_target);

} // namespace Internal

} // namespace MeshSimpl

#endif // LIB_MESH_SIMPL_ECOL_H
