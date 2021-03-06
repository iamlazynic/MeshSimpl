//
// Created by nickl on 1/8/19.
//

#include <cassert>
#include <cmath>
#include <memory>
#include <utility>

#include "edge.hpp"
#include "qemheap.hpp"

namespace MeshSimpl {
namespace Internal {

QEMHeap::QEMHeap(Edges &edges)
    : keys(edges.size() + 1),
      edges(edges),
      handles(edges.size(), 0),
      n(0),
      removed(edges.size(), false) {
  for (idx e = 0; e < edges.size(); ++e) {
    keys[handles[e] = ++n] = e;
  }
  keys.resize(n + 1);
}

void QEMHeap::pop() {
  exchange(1, n--);
  sink(1);
  keys.resize(n + 1);
}

void QEMHeap::fix(const Edge *edge, double errorPrev) {
  size_t k = handles[id(edge)];
  assert(contains(edge));
  if (edge->error() > errorPrev)
    sink(k);
  else
    swim(k);
}

void QEMHeap::penalize(Edge *edge) {
  assert(contains(edge));
  edge->setErrorInfty();
  sink(handles[id(edge)]);
}

bool QEMHeap::contains(const Edge *edge) const { return !removed[id(edge)]; }

void QEMHeap::markRemoved(const Edge *edge) { markRemovedById(id(edge)); }

void QEMHeap::markRemovedById(idx e) { removed[e] = true; }

bool QEMHeap::greater(size_t i, size_t j) const {
  assert(!std::isnan(edges[keys[i]].error()));
  assert(!std::isnan(edges[keys[j]].error()));
  return edges[keys[i]].error() > edges[keys[j]].error();
}

void QEMHeap::exchange(size_t i, size_t j) {
  std::swap(handles[keys[i]], handles[keys[j]]);
  std::swap(keys[i], keys[j]);
}

bool QEMHeap::isMinHeap(size_t k) const {
  if (k > n) return true;
  size_t left = 2 * k;
  size_t right = 2 * k + 1;
  assert(!(left <= n && greater(k, left)));
  assert(!(right <= n && greater(k, right)));
  return isMinHeap(left) && isMinHeap(right);
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
