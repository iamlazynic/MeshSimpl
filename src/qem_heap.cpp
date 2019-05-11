//
// Created by nickl on 1/8/19.
//

#include "qem_heap.hpp"
#include <cassert>
#include <limits>
#include "ecol.hpp"

namespace MeshSimpl {
namespace Internal {

QEMHeap::QEMHeap(E& edges, bool include_boundary)
    : keys(edges.size() + 1), edges(edges), handles(edges.size()), n(0) {
  for (idx i = 0; i < edges.size(); ++i)
    if (include_boundary || (edges[i].boundary_v != Edge::BOTH))
      keys[handles[i] = ++n] = i;

  keys.resize(n + 1);
  for (size_t k = n / 2; k >= 1; --k) sink(k);
  assert(is_min_heap());
}

void QEMHeap::pop() {
  exchange(1, n--);
  sink(1);
  keys.resize(n + 1);
}

void QEMHeap::fix(const Edge* ptr, double error_prev) {
  auto e = static_cast<idx>(ptr - edges.data());
  size_t k = handles[e];
  if (ptr->error > error_prev)
    sink(k);
  else
    swim(k);
}

void QEMHeap::penalize(idx e) {
  edges[e].error = std::numeric_limits<double>::max();
  sink(handles[e]);
}

void QEMHeap::erase(idx e) {
  const double error_prev = edges[e].error;
  size_t k = handles[e];
  assert(k < n + 1 && k >= 1);  // check existence in heap

  exchange(k, n--);
  if (edges[keys[k]].error > error_prev)
    sink(k);
  else
    swim(k);
  keys.resize(n + 1);
}

void QEMHeap::exchange(size_t i, size_t j) {
  std::swap(handles[keys[i]], handles[keys[j]]);
  std::swap(keys[i], keys[j]);
}

bool QEMHeap::is_min_heap(size_t k) const {
  if (k > n) return true;
  size_t left = 2 * k;
  size_t right = 2 * k + 1;
  assert(!(left <= n && greater(k, left)));
  assert(!(right <= n && greater(k, right)));
  return is_min_heap(left) && is_min_heap(right);
}

void QEMHeap::swim(size_t k) {
  while (k > 1 && greater(k / 2, k)) {
    exchange(k, k / 2);
    k = k / 2;
  }
}

void QEMHeap::sink(size_t k) {
  assert(k >= 1);
  while (2 * k <= n) {
    size_t j = 2 * k;
    if (j < n && greater(j, j + 1)) ++j;
    if (!greater(k, j)) break;
    exchange(k, j);
    k = j;
  }
}

}  // namespace Internal
}  // namespace MeshSimpl
