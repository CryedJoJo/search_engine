#ifndef __LRUCACHE_H__
#define __LRUCACHE_H__

#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <mutex>
#include <optional>

using std::string;
using std::unordered_map;
using std::list;
using std::vector;
using std::mutex;
using std::lock_guard;
using std::optional;

class LRUCache {
public:
	explicit LRUCache(size_t capacity = 1000);
	~LRUCache() = default;

	LRUCache(const LRUCache &rhs);
	LRUCache &operator=(const LRUCache &rhs);
	LRUCache(LRUCache &&rhs) noexcept;
	LRUCache &operator=(LRUCache &&rhs) noexcept;

	optional<string> get(const string &key);
	void             set(const string &key, const string &value);
	vector<string>   getKeys() const;

	size_t size() const { return _map.size(); }
	bool   empty() const { return _map.empty(); }

private:
	struct CacheNode {
		string key;
		string value;
	};

	size_t                                           _capacity;
	list<CacheNode>                                  _list;
	unordered_map<string, list<CacheNode>::iterator> _map;
	mutable mutex                                    _mutex;
};

#endif
