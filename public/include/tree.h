#pragma once

#include <vector>

template<typename T>
struct node;

template<typename T>
class tree;

template<typename T>
struct node {
	node() : left(0), right(0), parent(0), black(false) {}
	size_t left;
	size_t right;
	size_t parent;
	bool black;
	T value;
};

template<typename T>
class tree {
public:
	tree() : m_root(0) {
		m_node_pool.push_back(node<T>());
	}

	void insert(const T& v) {
		auto node = allocate_node();
		value(node, v);
		
		size_t cursor = m_root;

		if (cursor == invalid_index()) {
			m_root = node;
			//printf("Inserting on root\n");
		}

		while (cursor != invalid_index()) {
			T cv = value(cursor);
			if (v < cv) {
				auto cursor_left = left_child(cursor);
				if (cursor_left == invalid_index()) {
					left_child(cursor, node);
					parent(node, cursor);
					//printf("Inserting on left\n");
					break;
				} else {
					cursor = cursor_left;
					//printf("Recursing on left\n");
					continue;
				}
			} else if (v > cv) {
				auto cursor_right = right_child(cursor);
				if (cursor_right == invalid_index()) {
					right_child(cursor, node);
					parent(node, cursor);
					//printf("Inserting on right\n");
					break;
				} else {
					cursor = cursor_right;
					//printf("Recursing on right\n");
					continue;
				}
			} else {
				break;
			}
		}

		repair(node);
	}

	const size_t root() const noexcept {
		return m_root;
	}

	T value(size_t index) const {
		return m_node_pool[index].value;
	}

	void value(size_t index, T value) {
		m_node_pool[index].value = value;
	}

	bool black(size_t index) const {
		return m_node_pool[index].black;
	}

	size_t left_child(size_t index) const {
		return m_node_pool[index].left;
	}

	size_t right_child(size_t index) const {
		return m_node_pool[index].right;
	}

	size_t parent(size_t index) const {
		return m_node_pool[index].parent;
	}

protected:
	size_t allocate_node() {
		m_node_pool.push_back(node<T>());
		return m_node_pool.size() - 1;
	}

	void repair(size_t node);

	void rotate_left(size_t node);
	void rotate_right(size_t node);

	void black(size_t index, bool black) {
		m_node_pool[index].black = black;
	}

	void left_child(size_t node, size_t left) {
		m_node_pool[node].left = left;
	}
	

	void right_child(size_t node, size_t right) {
		m_node_pool[node].right = right;
	}	

	void parent(size_t node, size_t parent) {
		m_node_pool[node].parent = parent;
	}

	size_t grandparent(size_t index) const {
		auto p = parent(index);
		if (p != invalid_index())
			return parent(p);
		return invalid_index();
	}

	size_t sibling(size_t index) const {
		auto p = parent(index);
		if (p != invalid_index()) {
			if (index == left_child(p))
				return right_child(p);
			else
				return left_child(p);
		}
		return invalid_index();
	}

	size_t uncle(size_t index) const {
		auto p = parent(index);
		if (p != invalid_index()) {
			auto g = grandparent(p);
			if (g != invalid_index()) {
				return sibling(p);
			}
		}
		return invalid_index();
	}

	size_t invalid_index() const noexcept {
		return 0;
	}

private:
	std::vector<node<T>> m_node_pool;
	size_t m_root;
};

template<typename T>
void tree<T>::repair(size_t n) {
	while (m_root != n && !black(parent(n))) {
		auto p = parent(n);
		auto g = grandparent(n);

		if (p == invalid_index() || g == invalid_index()) {
			printf("no parent\n");
			break;
		}

		if (left_child(g) == p) {
			auto uncle = right_child(g);
			if (!black(uncle)) {
				black(p, true);
				black(uncle, true);
				black(g, false);
				n = g;
			} else {
				if (right_child(p) == n) {
					n = p;
					rotate_left(n);
					p = parent(n);
					g = grandparent(n);
				}
				black(p, true);
				black(g, false);
				rotate_right(g);
			}
		} else {
			auto uncle = left_child(g);
			if (!black(uncle)) {
				black(p, true);
				black(uncle, true);
				black(g, false);
				n = g;
			} else {
				if (left_child(p) == n) {
					n = p;
					rotate_right(n);
					p = parent(n);
					g = grandparent(n);
				}
				black(p, true);
				black(g, false);
				rotate_left(g);
			}
		}
	}
	black(m_root, true);
}

template<typename T>
void tree<T>::rotate_left(size_t n) {
	auto rchild = right_child(n);
	right_child(n, left_child(rchild));
	if (left_child(rchild) != invalid_index()) {
		parent(left_child(rchild), parent(n));
	}
	if (n != m_root) {
		if (left_child(parent(n)) == n) {
			left_child(parent(n), rchild);
		} else {
			right_child(parent(n), rchild);
		}
	} else {
		m_root = rchild;
	}
	left_child(rchild, n);
	if (n != invalid_index()) {
		parent(n, rchild);
	}
}

template<typename T>
void tree<T>::rotate_right(size_t n) {
	auto lchild = left_child(n);
	left_child(n, right_child(lchild));
	if (right_child(lchild) != invalid_index()) {
		parent(right_child(lchild), parent(n));
	}
	if (n != m_root) {
		if (right_child(parent(n)) == n) {
			right_child(parent(n), lchild);
		} else {
			left_child(parent(n), lchild);
		}
	} else {
		m_root = lchild;
	}
	right_child(lchild, n);
	if (n != invalid_index()) {
		parent(n, lchild);
	}
}
