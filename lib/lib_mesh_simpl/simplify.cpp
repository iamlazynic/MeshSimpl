//
// Created by nickl on 1/8/19.
//

#include "simplify.h"
#include "ecol.h"
#include "pre_proc.h"
#include "post_proc.h"

namespace MeshSimpl
{

// Mesh simplification main method. Simplify given mesh until remaining number of vertices/faces
// is (1-strength) of the original. Returns output vertices and indices as in inputs.
// TODO: Ignoring the 4th and the following values (if exist) in vertices.
std::pair<V, F> simplify(const V& vertices, const F& indices, float strength)
{
    const auto nv_target = std::max(static_cast<size_t>(std::lround((1-strength)*vertices.size())),
                                    Internal::MIN_NR_VERTICES);
    auto out_vertices = vertices;
    auto out_indices = indices;

    if (nv_target == vertices.size())
        return {out_vertices, out_indices};

    auto quadrics = Internal::compute_quadrics(vertices, indices, true);

    auto edge_topo = Internal::construct_edges(indices, vertices.size());
    std::vector<Internal::Edge>& edges = edge_topo.first;
    std::vector<vec3i>& face2edge = edge_topo.second;

    Internal::init_edge_errors(vertices, quadrics, edges);

    // create priority queue on quadric error
    Internal::QEMHeap heap(edges);

    std::vector<bool> deleted_vertex(vertices.size(), false);
    std::vector<bool> deleted_face(indices.size(), false);

    // returns f,v,e of next neighbor face centered around v_center;
    // indexes v and e are local in face (0,1,2) in both input and output.
    const auto iter_next = [&](vec3i& fve)->void {
        const idx f = fve[0], v = fve[1], e = fve[2];
        assert(v != e);
        const auto& edge = edges[face2edge[f][v]];
        const idx f_idx_to_edge = fi_in_edge(edge, f);
        const idx of = edge.faces[1-f_idx_to_edge];
        assert(f != of);
        const idx ov_global = out_indices[f][e];
        const idx ov = Internal::vi_in_face(out_indices, of, ov_global);
        assert(face2edge[of][ov] != face2edge[f][e]);
        const idx oe = edge.idx_in_face[1-f_idx_to_edge];
        fve = {of, ov, oe};
    };

    // one run of a series of edge collapse that is supposed to decimate nv_decimate vertices and
    // returns true if it ends because this target is achieved instead of because of other reason
    const auto run = [&](size_t nv_decimate)->bool {
        size_t i = 0;
        while (!heap.empty() && i < nv_decimate) {
            // collapse an edge to remove 1 vertex and 2 faces in each iteration
            const idx e_collapsed = heap.top();

            const auto& edge = edges[e_collapsed];
            if (edge.error == std::numeric_limits<double>::max())
                break;
            assert(edge.vertices[0] != edge.vertices[1]);
            assert(edge.faces[0] != edge.faces[1]);

            if (edge.boundary_v == Internal::BOUNDARY_V::NONE) {
                if (collapse_interior_edge(out_vertices, out_indices, edges, face2edge, quadrics,
                                           deleted_vertex, deleted_face,
                                           iter_next, heap, e_collapsed)) {
                    ++i;
                }
                else {
                    heap.penalize(e_collapsed);
                }
            }
            else {
                // TODO: handle boundary cases
                assert(false);
            }
        }
        return i == nv_decimate;
    };

    size_t nv_second_run = (vertices.size()-nv_target) >> 8;
    if (nv_second_run >> 4 > 0) {
        bool first_run_complete = run(vertices.size()-nv_target-nv_second_run);
        if (first_run_complete) {
            quadrics = Internal::compute_quadrics(out_vertices, out_indices, deleted_face, true);
            heap.reset_edge_errors(out_vertices, quadrics);
            run(nv_second_run);
        }
    }
    else {
        run(vertices.size()-nv_target);
    }

    // edges are pointless from this point on, but need to fix vertices and indices
    Internal::compact_data(deleted_vertex, deleted_face, out_vertices, out_indices);
    return {out_vertices, out_indices};
}

}

