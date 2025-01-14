// Copyright 2020, MIT
// Authors: Xuhao Chen <cxh@mit.edu>
#include "graph.h"

void TCSolver(Graph &g, uint64_t &total, int n_gpu, int chunk_size);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <graph> [oriented(0)] [num_gpu(1)] [chunk_size(1024)] [adj_sorted(1)]\n";
    std::cout << "Example: " << argv[0] << " /graph_inputs/mico/graph\n";
    exit(1);
  }

  std::cout << "Triangle Counting: assuming the neighbor lists are sorted.\n";
  int oriented = 0;
  if (argc > 2) oriented = atoi(argv[2]);
  bool need_orientation = true;
  if (oriented) need_orientation = false;
  Graph g(argv[1], need_orientation);

  int n_devices = 1;
  int chunk_size = 1024;
  int adj_sorted = 1;
  if (argc > 3) n_devices = atoi(argv[3]);
  if (argc > 4) chunk_size = atoi(argv[4]);
  g.print_meta_data();
  if (argc > 5) adj_sorted = atoi(argv[5]);
  if (!adj_sorted) g.sort_neighbors();
  uint64_t total = 0;
  TCSolver(g, total, n_devices, chunk_size);
  std::cout << "total_num_triangles = " << total << "\n";
  return 0;
}

