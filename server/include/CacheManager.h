#ifndef __CACHEMANAGER_H__
#define __CACHEMANAGER_H__

#include "LRUCache.h"

#include <memory>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

class CacheManager {
public:
	static CacheManager *getInstance();
	static void          destroy();

	void onNewConnection(const TcpConnectionPtr &con);
	void onClose(const TcpConnectionPtr &con);

	optional<string> get(const TcpConnectionPtr &con, const string &key);
	void             set(const TcpConnectionPtr &con, const string &key, const string &value);

	void startSync();
	void stopSync();

private:
	CacheManager();
	~CacheManager();
	CacheManager(const CacheManager &)            = delete;
	CacheManager &operator=(const CacheManager &) = delete;

	void syncLoop();
	void syncAll();

	struct CachePair {
		// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
		// BUG: unique_ptr 无法在 syncAll 中共享所有权，导致悬挂指针
		// std::unique_ptr<LRUCache> cache1;
		// std::unique_ptr<LRUCache> cache2;
		// FIX: 改用 shared_ptr，syncAll 可延长缓存对象的生命周期
		// ————————————————————————————————————————————————————————————————————————bug 时间：2026:6:3
		std::shared_ptr<LRUCache> cache1;
		std::shared_ptr<LRUCache> cache2;
		LRUCache		         *active;
		LRUCache		         *sync;
	};

	static CacheManager *_instance;

	std::unordered_map<TcpConnectionPtr, CachePair> _caches;
	mutable std::mutex                              _mapMutex;

	std::thread                           _syncThread;
	std::atomic<bool>                     _running{false};
	std::chrono::steady_clock::time_point _lastDebugLogTime;
};

#endif
