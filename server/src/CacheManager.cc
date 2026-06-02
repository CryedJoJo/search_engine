#include "CacheManager.h"
#include "TcpConnection.h"
#include "log4cpp.h"

#include <algorithm>

CacheManager *CacheManager::_instance = nullptr;

CacheManager *CacheManager::getInstance()
{
	if(nullptr == _instance){
		_instance = new CacheManager();
	}
	return _instance;
}

void CacheManager::destroy()
{
	if(nullptr != _instance){
		delete _instance;
		_instance = nullptr;
	}
}

CacheManager::CacheManager()
    : _lastDebugLogTime(std::chrono::steady_clock::now())
{
}

CacheManager::~CacheManager()
{
	stopSync();
}

void CacheManager::onNewConnection(const TcpConnectionPtr &con)
{
	lock_guard<mutex> lg(_mapMutex);

	CachePair pair;
	pair.cache1 = std::make_unique<LRUCache>(1000);
	pair.cache2 = std::make_unique<LRUCache>(1000);
	pair.active = pair.cache1.get();
	pair.sync   = pair.cache2.get();

	_caches[con] = std::move(pair);
	INFO("CacheManager registered connection, total caches: {}", _caches.size());
}

void CacheManager::onClose(const TcpConnectionPtr &con)
{
	lock_guard<mutex> lg(_mapMutex);

	_caches.erase(con);
	INFO("CacheManager unregistered connection, remaining caches: {}", _caches.size());
}

optional<string> CacheManager::get(const TcpConnectionPtr &con, const string &key)
{
	lock_guard<mutex> lg(_mapMutex);

	auto it = _caches.find(con);
	if(it == _caches.end()){
		return std::nullopt;
	}
	return it->second.active->get(key);
}

void CacheManager::set(const TcpConnectionPtr &con, const string &key, const string &value)
{
	lock_guard<mutex> lg(_mapMutex);

	auto it = _caches.find(con);
	if(it != _caches.end()){
		it->second.active->set(key, value);
	}
}

void CacheManager::startSync()
{
	if(_running.load()){
		return;
	}
	_running.store(true);
	_syncThread = std::thread(&CacheManager::syncLoop, this);
	INFO("CacheManager sync thread started");
}

void CacheManager::stopSync()
{
	if(!_running.load()){
		return;
	}
	_running.store(false);
	if(_syncThread.joinable()){
		_syncThread.join();
	}
	INFO("CacheManager sync thread stopped");
}

void CacheManager::syncLoop()
{
	while(_running.load()){
		std::this_thread::sleep_for(std::chrono::seconds(30));
		if(!_running.load()){
			break;
		}
		syncAll();
	}
}

void CacheManager::syncAll()
{
	vector<LRUCache *> syncCaches;

	{
		lock_guard<mutex> lg(_mapMutex);

		syncCaches.reserve(_caches.size());
		for(auto &pair : _caches){
			std::swap(pair.second.active, pair.second.sync);
			syncCaches.push_back(pair.second.sync);
		}
	}

	if(syncCaches.empty()){
		return;
	}

	WARN("CacheManager sync start, {} caches to sync", syncCaches.size());

	// 选第一个 cache 为锚点
	LRUCache *target = syncCaches[0];

	// === 第一趟：将所有非锚点 cache 的数据通过 get/set 合并到锚点 ===
	for(size_t i = 1; i < syncCaches.size(); ++i){
		auto keys = syncCaches[i]->getKeys();
		for(auto &key : keys){
			auto val = syncCaches[i]->get(key);
			if(val){
				target->set(key, *val);
			}
		}
	}

	// === 第二趟：以锚点中的 key 为准，用 get/set 广播到所有其他 cache ===
	auto targetKeys = target->getKeys();
	for(size_t i = 1; i < syncCaches.size(); ++i){
		for(auto &key : targetKeys){
			auto val = target->get(key);
			if(val){
				syncCaches[i]->set(key, *val);
			}
		}
	}

	INFO("CacheManager sync done, {} entries in target, {} caches updated",
	     target->size(), syncCaches.size());

	// 流控 debug 日志：每 60s 打印一次各连接 cache 中的 key
	auto now = std::chrono::steady_clock::now();
	if(now - _lastDebugLogTime >= std::chrono::seconds(60)){
		_lastDebugLogTime = now;

		lock_guard<mutex> lg(_mapMutex);
		for(auto &entry : _caches){
			string connInfo = entry.first->toString();
			auto activeKeys = entry.second.active->getKeys();
			auto syncKeys   = entry.second.sync->getKeys();

			string activeKeyStr, syncKeyStr;
			for(auto &key : activeKeys){
				if(!activeKeyStr.empty()) activeKeyStr += ", ";
				activeKeyStr += key;
			}
			for(auto &key : syncKeys){
				if(!syncKeyStr.empty()) syncKeyStr += ", ";
				syncKeyStr += key;
			}

			DEBUG("[conn {}] active[{}]: {}", connInfo, activeKeys.size(), activeKeyStr);
			DEBUG("[conn {}] sync[{}]:   {}", connInfo, syncKeys.size(), syncKeyStr);
		}
	}
}
