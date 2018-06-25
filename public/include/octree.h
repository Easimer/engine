#pragma once

#include <array>
#include <vector>
#include <memory>
#include <limits>

#include <math/vector.h>

namespace octree {

	// X_Y_Z
	enum child_index {
		left_bottom_backward	= 0,
		left_bottom_forward		= 1,
		left_bottom_max,
		left_top_backward		= 2,
		left_top_forward		= 3,
		left_top_max,
		left_max,
		right_bottom_backward	= 4,
		right_bottom_forward	= 5,
		right_bottom_max,
		right_top_backward		= 6,
		right_top_forward		= 7,
		right_top_max,
		right_max,
		child_index_max			= 8
	};

	template<typename T, size_t max_depth>
	struct node;
	template<typename T, size_t max_depth>
	class tree;

	template<typename T, size_t max_depth>
	struct node {
	public:
		node(size_t current_depth) : m_level(current_depth), m_nodes{ nullptr } {}

		// An octree node is subdivisible on `index` index if:
		// - It's depth hasn't reached a maximum value and
		// - The node on `index` hasn't been allocated yet
		bool divisible(child_index index) const noexcept {
			return m_level < (max_depth - 1) && m_nodes[index] == nullptr;
		}

		node<T, max_depth>* subdivide(tree<T, max_depth>& root, child_index index) {
			if (!divisible(index))
				return nullptr;
			m_nodes[index] = root.allocate_node(*this);
			return m_nodes[index];
		}

		void subdivide(tree<T, max_depth>& root) {
			if (!divisible(index))
				return;
			for (size_t i = 0; i < (size_t)child_index_max; i++) {
				m_nodes[i] = root.allocate_node(*this);
			}
		}

		bool max_depth_reached() const noexcept {
			return m_level == max_depth - 1;
		}

		size_t level() const noexcept {
			return m_level;
		}

		void push_back(const T& data) {
			m_elements.push_back(data);
		}

		friend tree;

	private:
		std::vector<T> m_elements;
		std::array<octree::node<T, max_depth>*, child_index_max> m_nodes;
		// Current level (cannot be max_depth)
		size_t m_level;
	};

	template<typename T, size_t max_depth>
	class tree {
	public:
		tree(float min = std::numeric_limits<float>::lowest(), float max = std::numeric_limits<float>::max()) : m_min_coord(min), m_max_coord(max) {
			m_node_pool.reserve((size_t)child_index_max);
			for (size_t i = 0; i < (size_t)child_index_max; i++) {
				m_node_pool.push_back(node<T, max_depth>(0));
				m_nodes[i] = &(m_node_pool.back());
			}
		}

		octree::node<T, max_depth>* allocate_node(const node<T, max_depth>& requester) {
			node<T, max_depth> new_node(requester.level() + 1);
			m_node_pool.push_back(new_node);
			return &(m_node_pool.back());
		}

		child_index next_node_index(const math::vector3<float>& coord, const math::vector3<float>& origin) {
			child_index index;
			if (coord[0] >= origin[0])
				if (coord[1] >= origin[1])
					if (coord[2] >= origin[2])
						index = right_top_forward;
					else
						index = right_top_backward;
				else
					if (coord[2] >= origin[2])
						index = right_bottom_forward;
					else
						index = right_bottom_backward;
			else
				if (coord[1] >= origin[1])
					if (coord[2] >= origin[2])
						index = left_top_forward;
					else
						index = left_top_backward;
				else
					if (coord[2] >= origin[2])
						index = left_bottom_forward;
					else
						index = left_bottom_backward;
			return index;
		}

		void insert(const math::vector3<float>& coord, const T& data) {
			vec3 origin(0, 0, 0);
			child_index index = next_node_index(coord, origin);
			PRINT_DBG("Next node index: " << index);
			node<T, max_depth>* node = m_nodes[index];			
			
			PRINT_DBG(m_max_coord);

			float step_pos = m_max_coord / 2.0f;
			float step_neg = m_min_coord / 2.0f;
			
			while (!node->max_depth_reached()) {
				origin[0] += coord[0] >= origin[0] ? step_pos : step_neg;
				origin[1] += coord[1] >= origin[1] ? step_pos : step_neg;
				origin[2] += coord[2] >= origin[2] ? step_pos : step_neg;

				step_pos = step_pos / 2.0f;
				step_neg = step_neg / 2.0f;

				PRINT_DBG(origin);

				index = next_node_index(coord, origin);
				PRINT_DBG("Next node index: " << index);

				if (node->divisible(index)) {
					node = node->subdivide(*this, index);
				}
			}
			node->push_back(data);
		}

		std::vector<const T&> in_radius(const math::vector3<float>& origin, float radius) const noexcept {
			std::vector<const T&> ret;

			return ret;
		}
		
	private:
		// Nodes connected directly to the root
		std::array<octree::node<T, max_depth>*, child_index_max> m_nodes;
		// Node pool
		std::vector<octree::node<T, max_depth>> m_node_pool;

		float m_min_coord;
		float m_max_coord;
	};
}
