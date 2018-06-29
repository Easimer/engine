#pragma once

#include "tree.h"
#include "adler32.h"

template<typename T1, typename T2>
class pair {
public:
	using first_type = T1;
	using second_type = T1;
	T1 first;
	T2 second;
};

struct itree_node {
public:
	virtual uint64_t hash() const noexcept = 0;
};

template<typename T>
uint64_t hash(const T& v) {
	return adler64().apply(&v, sizeof(v)).result();
}

template<>
uint64_t hash<std::string>(const std::string& v) {
	return adler64().apply(v.c_str(), v.size()).result();
}

template<typename Key_t, typename Value_t>
class map {
public:
	using key_type = Key_t;
	using mapped_type = Value_t;
	using value_type = pair<const Key_t, Value_t>;

	Value_t& operator[](const Key_t& key) {
		uint64_t key_hash = hash<Key_t>(key);
		std::optional<tree_node*> res = m_tree.find(key_hash);
		if (!res) {
			tree_node n;
			n.key = key_hash;
			n.value = Value_t();
			PRINT_DBG("Key " << key << " not found in tree, inserting with hash=" << n.key);
			return m_tree.insert(n).value;
		} else {
			return res.value()->value;
		}
	}

	struct tree_node : public itree_node {
		uint64_t key;
		Value_t value;

		uint64_t hash() const noexcept { return key; }

		operator uint64_t() const noexcept { return key; }
	};

//private:
	tree<tree_node> m_tree;
};


bool operator<(const itree_node& lhs, const itree_node& rhs) {
	return lhs.hash() < rhs.hash();
}

bool operator>(const itree_node& lhs, const itree_node& rhs) {
	return lhs.hash() > rhs.hash();
}

bool operator<(const itree_node& lhs, uint64_t rhs) {
	return lhs.hash() < rhs;
}

bool operator==(const itree_node& lhs, uint64_t rhs) {
	return lhs.hash() == rhs;
}
