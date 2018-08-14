<?php
/**
 * Created by PhpStorm.
 * User: liujingfeng.a
 * Date: 2018/6/30
 * Time: 18:24
 */

namespace Tars\monitor\cache;


use Tars\monitor\contract\StoreCacheInterface;

class RedisStoreCache implements StoreCacheInterface
{
    protected $_redis = null;
    protected $redisStoreCachePrefix = 'redis_store_cache_';
    protected $ttl = 600;   //Remaining Time To Live, in seconds.

    public function __construct($config)
    {
        $redis = new \Redis();
        $redis->connect($config['host'], $config['port'], $config['timeout']);
        if (isset($config['password'])) {
            $redis->auth($config['password']);
        }
        if (isset($config['cachePrefix'])) {
            $this->redisStoreCachePrefix = $config['cachePrefix'];
        }
        if (isset($config['ttl'])) {
            $this->ttl = $config['ttl'];
        }

        $this->_redis = $redis;
    }

    /**
     * Retrieve an item from the cache by key.
     *
     * @param  string $key
     * @return mixed
     */
    public function get($key)
    {
        $key = $this->generateKey($key);
        return $this->_redis->hGetAll($key);
    }

    /**
     * Retrieve an filed value from the cache by key and field.
     *
     * @param  string $key
     * @param  string $field
     * @return mixed
     */
    public function getField($key, $field)
    {
        $key = $this->generateKey($key);
        return $this->_redis->hget($key, $field);
    }

    /**
     * Retrieve all key with value from the cache.
     *
     * @return mixed
     */
    public function getAll()
    {
        $ret = [];
        $keys = $this->_redis->keys($this->redisStoreCachePrefix . "*");
        foreach ($keys as $key) {
            $keyHashArray = explode($this->redisStoreCachePrefix, $key);
            $ret[$keyHashArray[1]] = $this->_redis->hGetAll($key);
        }
        return $ret;
    }

    /**
     * Set an item into cache.
     *
     * @param   string $key
     * @param   array $value
     * @return  mixed
     */
    public function set($key, $value)
    {
        $key = $this->generateKey($key);
        $this->_redis->hMset($key, $value);
        $this->_redis->expire($key, $this->ttl);
        return;
    }

    /**
     * Increment the int|float value of a key field by the given amount.
     *
     * @param   string $key
     * @param   string $field
     * @param   mixed $incrby
     * @return  mixed
     */
    public function incrField($key, $field, $incrby = 1)
    {
        $key = $this->generateKey($key);
        $this->_redis->hIncrBy($key, $field, $incrby);
        $this->_redis->expire($key, $this->ttl);
        return;
    }

    /**
     * Decrement the int|float value of a key field by the given amount.
     *
     * @param   string $key
     * @param   string $field
     * @param   mixed $decrby
     * @return  mixed
     */
    public function decrField($key, $field, $decrby = 1)
    {
        $key = $this->generateKey($key);
        $incrby = 0 - $decrby;
        return $this->_redis->hIncrBy($key, $field, $incrby);
    }

    /**
     * Removes a values from the stored by key.
     *
     * @param   string $key
     * @return  mixed
     */
    public function del($key)
    {
        $key = $this->generateKey($key);
        return $this->_redis->del($key);
    }

    /**
     * Verify if the key exists in the store.
     *
     * @param   string $key
     * @return  mixed
     */
    public function exist($key)
    {
        $key = $this->generateKey($key);
        return $this->_redis->exists($key);
    }

    private function generateKey($key)
    {
        return $this->redisStoreCachePrefix . $key;
    }
}