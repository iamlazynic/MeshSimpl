#ifndef LIB_MESH_SIMPL_ECOL_H
#define LIB_MESH_SIMPL_ECOL_H

#include "neighbor.h"
#include "qem_heap.h"
#include "types.h"
#include "util.h"

namespace MeshSimpl {

namespace Internal {

static const double FOLD_OVER_COS_ANGLE = std::cos(160);

// Set error and center of edge by choosing a position to collapse into
void optimal_ecol_vertex_placement(const V& vertices, Edge& edge);

// Set quadric, error, and collapse center for a given edge
void set_edge_error(const V& vertices, const Q& quadrics, Edge& edge);

// Compute quadric error for every edge at initialization
void compute_errors(const V& vertices, const Q& quadrics, E& edges);

// Returns true if the movement of vertex will cause this face to flip too much to accept
bool face_fold_over(const V& vertices, const F& indices, const Neighbor& nb, idx v_move,
                    const vec3d& move_to);

// Replace a vertex in edge, and update other members then fix priority in heap
void update_error_and_center(const V& vertices, const Q& quadrics, QEMHeap& heap,
                             Edge* edge_ptr);

// Find relevant faces (neighbor faces of two endpoints) one by one around the
// collapsed edge. Meanwhile this function does geometry and connectivity check to
// avoid fold-over faces and non-manifold edges.
//
// Traverse neighbors around deleted vertex first, then around kept vertex,
// both clockwise. One exception is when traversing neighbors of boundary kept vertex.
// Also refer to data structure "Neighbor" in neighbor.h
//
// Outputs: v_del_neighbors -- neighbors of deleted vertex
//          c_kept_neighbor_edges -- index of edges incident to kept vertex
// Returns: true if no problem is found -- this edge collapse operation is acceptable
bool scan_neighbors(const V& vertices, const F& indices, const E& edges,
                    const F2E& face2edge, const Edge& edge,
                    std::vector<Neighbor>& v_del_neighbors,
                    std::vector<idx>& v_kept_neighbor_edges);
;

inline idx choose_v_del(const Edge& edge) {
    assert(edge.boundary_v != Edge::BOTH);
    return edge.boundary_v == Edge::NONE ? 0 : static_cast<idx>(1 - edge.boundary_v);
}

// Returns true if edge is collapsed
bool edge_collapse(V& vertices, F& indices, E& edges, F2E& face2edge, Q& quadrics,
                   QEMHeap& heap, idx ecol_target);

} // namespace Internal

} // namespace MeshSimpl

#endif // LIB_MESH_SIMPL_ECOL_H
