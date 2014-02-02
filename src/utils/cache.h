/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

namespace CUBE {

template<typename K, typename V>
class Cache
{
private:
	std::map<typename K, std::weak_ptr<typename V>> cache;
public:
	std::shared_ptr<typename V> Get(const K& key)
	{
		auto it = cache.find(key);
		if(it == cache.end())
			return std::shared_ptr<V>();

		if(it->second.expired()) {
			cache.erase(it);
			return std::shared_ptr<V>();
		}
		return it->second.lock();
	}

	std::shared_ptr<typename V> Insert(const K& key, V* object)
	{
		std::shared_ptr<V> ptr(object);
		
		auto result = cache.insert({key, ptr});
		assert(result.second);
		return ptr;
	}

	void Flush()
	{
		for(auto it=cache.begin(); it!=cache.end();) {
			if(it->second.expired())
				cache.erase(it);
			else
				++it;
		}
	}
};

} // CUBE