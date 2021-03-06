/**
 * @file subgraph.cpp
 * Test script for viewing a subgraph from our Graph
 *
 * @brief Reads in two files specified on the command line.
 * First file: 3D Points (one per line) defined by three doubles
 * Second file: Tetrahedra (one per line) defined by 4 indices into the point
 * list
 */

#include <fstream>
#include <iterator>
#include <stdlib.h> /* srand, rand */

#include "CME212/SFML_Viewer.hpp"
#include "CME212/Util.hpp"

#include "Graph.hpp"

/** An iterator that skips over elements of another iterator based on whether
 * those elements satisfy a predicate.
 *
 * Given an iterator range [@a first, @a last) and a predicate @a pred,
 * this iterator models a filtered range such that all i with
 * @a first <= i < @a last and @a pred(*i) appear in order of the original range.
 */
template <typename Pred, typename It>
class filter_iterator : private equality_comparable<filter_iterator<Pred,It>>
{
 public:
  // Get all of the iterator traits and make them our own
  using value_type        = typename std::iterator_traits<It>::value_type;
  using pointer           = typename std::iterator_traits<It>::pointer;
  using reference         = typename std::iterator_traits<It>::reference;
  using difference_type   = typename std::iterator_traits<It>::difference_type;
  using iterator_category = typename std::input_iterator_tag;

  // Constructor
  filter_iterator(const Pred& p, const It& first, const It& last)
      : p_(p), it_(first), end_(last) {
    // HW1 #4: YOUR CODE HERE
        find_next_true(); 
  }

  // HW1 #4: YOUR CODE HERE
  // Supply definitions AND SPECIFICATIONS for:
  
  /** 
  * @return returns the value_type that the filter_iterator points to
  */
  value_type operator*() const{
    return *it_;
  }

  /** This changes the filter_iterator to point to the next true object
   * @return updates filter_iterator to point to next object that 
   *         satisfied the predicated unless the filter_iterator already points 
   *          to the nullptr
   */
  filter_iterator& operator++(){
    ++it_;
    find_next_true();
    return *this;
  }

  /**
  * @param rit An object of const filter_iterator
  * @return bool is true if @a rit points to the nullptr
  */
  bool operator==(const filter_iterator& rit) const{
    //return (it_ == rit.it_)  && (end_ == rit.end_);
    return it_ == rit.end_;
  }

 private:
  Pred p_;
  It it_;
  It end_;

  void find_next_true() {
    while(it_ != end_ && !p_(*it_)) ++it_;
  }
};

/** Helper function for constructing filter_iterators. This deduces the type of
 * the predicate function and the iterator so the user doesn't have to write it.
 * This also allows the use of lambda functions as predicates.
 *
 * Usage:
 * // Construct an iterator that filters odd values out and keeps even values.
 * std::vector<int> a = ...;
 * auto it = make_filtered(a.begin(), a.end(), [](int k) {return k % 2 == 0;});
 */
template <typename Pred, typename Iter>
filter_iterator<Pred,Iter> make_filtered(const Iter& it, const Iter& end,
                                         const Pred& p) {
  return filter_iterator<Pred,Iter>(p, it, end);
}

// HW1 #4: YOUR CODE HERE
// Specify and write an interesting predicate on the nodes.
// Explain what your predicate is intended to do and test it.
// If you'd like you may create new nodes and tets files.

/** Return true is a node is a specified distance away from a specified point
 * @param n An Node object 
 * @return bool which is true only if @n.position() is a specified distance away from a specified point
 * @pre @a n has valid position, i.e. @a n.position() is defined, is nonempty 
 */
struct InterestingPredicate {
  template <typename NODE>
  bool operator()(const NODE& n) {
    Point p(0.1,0.1,0.1);
    double distance_from_point = norm_2(n.position() - p);
    double distance_allowed = 1.1;
    return (distance_from_point < distance_allowed);
  }
};

/** Test predicate for HW1 #4 */
struct SlicePredicate {
  template <typename NODE>
  bool operator()(const NODE& n) {
    return n.position().x < 0;
  }
};

int main(int argc, char** argv)
{
  // Check arguments
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " NODES_FILE TETS_FILE\n";
    exit(1);
  }

  // Define our types
  using GraphType = Graph<int>;
  using NodeType  = typename GraphType::node_type;

  // Construct a Graph
  GraphType graph;
  std::vector<NodeType> nodes;

  // Create a nodes_file from the first input argument
  std::ifstream nodes_file(argv[1]);
  // Interpret each line of the nodes_file as a 3D Point and add to the Graph
  Point p;
  while (CME212::getline_parsed(nodes_file, p))
    nodes.push_back(graph.add_node(p));

  // Create a tets_file from the second input argument
  std::ifstream tets_file(argv[2]);
  // Interpret each line of the tets_file as four ints which refer to nodes
  std::array<int,4> t;
  while (CME212::getline_parsed(tets_file, t))
    for (unsigned i = 1; i < t.size(); ++i)
      for (unsigned j = 0; j < i; ++j)
        graph.add_edge(nodes[t[i]], nodes[t[j]]);

  // Print out the stats
  std::cout << graph.num_nodes() << " " << graph.num_edges() << std::endl;

  // Launch the SFML_Viewer
  CME212::SFML_Viewer viewer;

  // HW1 #4: YOUR CODE HERE
  // Use the filter_iterator to plot an induced subgraph.
  InterestingPredicate slice;

  auto filter_start = make_filtered(graph.node_begin(), graph.node_end(), slice);
  auto filter_end = make_filtered(graph.node_end(), graph.node_end(), slice);

  auto node_map = viewer.empty_node_map(graph);
  viewer.add_nodes( filter_start , filter_end, node_map );
  viewer.add_edges( graph.edge_begin() , graph.edge_end(), node_map );

  // Center the view and enter the event loop for interactivity
  viewer.center_view();
  viewer.event_loop();

  return 0;
}
