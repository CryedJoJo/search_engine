#include "LRUCache.h"
#include "log4cpp.h"

LRUCache::LRUCache(size_t capacity)
    : _capacity(capacity)
{
}

LRUCache::LRUCache(const LRUCache &rhs)
    : _capacity(rhs._capacity)
    , _list(rhs._list)
{
	for(auto it = _list.begin(); it != _list.end(); ++it){
		_map[it->key] = it;
	}
}

LRUCache &LRUCache::operator=(const LRUCache &rhs)
{
	if(this != &rhs){
		_capacity = rhs._capacity;
		_list     = rhs._list;
		_map.clear();
		for(auto it = _list.begin(); it != _list.end(); ++it){
			_map[it->key] = it;
		}
	}
	return *this;
}

LRUCache::LRUCache(LRUCache &&rhs) noexcept
    : _capacity(rhs._capacity)
    , _list(std::move(rhs._list))
    , _map(std::move(rhs._map))
{
	rhs._capacity = 1000;
}

LRUCache &LRUCache::operator=(LRUCache &&rhs) noexcept
{
	if(this != &rhs){
		_capacity = rhs._capacity;
		_list     = std::move(rhs._list);
		_map      = std::move(rhs._map);
		rhs._capacity = 1000;
	}
	return *this;
}

optional<string> LRUCache::get(const string &key)
{
	lock_guard<mutex> lg(_mutex);

	auto it = _map.find(key);
	if(it == _map.end()){
		return std::nullopt;
	}

	_list.splice(_list.begin(), _list, it->second);
	return it->second->value;
}

void LRUCache::set(const string &key, const string &value)
{
	lock_guard<mutex> lg(_mutex);

	auto it = _map.find(key);
	if(it != _map.end()){
		it->second->value = value;
		_list.splice(_list.begin(), _list, it->second);
		return;
	}

	if(_list.size() >= _capacity){
		string evictKey = _list.back().key;
		_map.erase(evictKey);
		_list.pop_back();
	}

	_list.push_front({key, value});
	_map[key] = _list.begin();
}

vector<string> LRUCache::getKeys() const
{
	lock_guard<mutex> lg(_mutex);

	vector<string> keys;
	keys.reserve(_list.size());
	for(auto &node : _list){
		keys.push_back(node.key);
	}
	return keys;
}
