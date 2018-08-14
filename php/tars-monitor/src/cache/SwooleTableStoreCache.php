<?php
namespace Tars\monitor\cache;

use Tars\monitor\contract\StoreCacheInterface;


class SwooleTableStoreCache implements StoreCacheInterface
{
    protected $_swooleTable = null;
    protected $_swooleTableKey = null;

    public function __construct($config)
    {
        $size = isset($config['size']) ? $config['size'] : 40960;
        $this->_swooleTable = new \swoole_table($size);
        $this->_swooleTable->column('count', \swoole_table::TYPE_INT, 4);
        $this->_swooleTable->column('timeoutCount', \swoole_table::TYPE_INT, 4);
        $this->_swooleTable->column('execCount', \swoole_table::TYPE_INT, 4);
        $this->_swooleTable->column('totalRspTime', \swoole_table::TYPE_INT, 4);
        $this->_swooleTable->column('maxRspTime', \swoole_table::TYPE_INT, 4);
        $this->_swooleTable->column('minRspTime', \swoole_table::TYPE_INT, 4);

        $this->_swooleTable->create();

        $this->_swooleTableKey = new \swoole_table(4096);
        $this->_swooleTableKey->column('key', \swoole_table::TYPE_STRING, 256);
        $this->_swooleTableKey->create();
    }

    /**
     * Retrieve an item from the cache by key.
     *
     * @param  string $key
     * @return mixed
     */
    public function get($key)
    {
        $hash = md5($key);
        return $this->_swooleTable->get($hash);
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
        $hash = md5($key);
        return $this->_swooleTable->get($hash, $field);
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
        $hash = md5($key);
        $this->_swooleTableKey->set($hash, ['key' => $key]);
        return $this->_swooleTable->set($hash, $value);
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
        $hash = md5($key);
        return $this->_swooleTable->incr($hash, $field, $incrby);
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
        $hash = md5($key);
        return $this->_swooleTable->decr($hash, $field, $decrby);
    }

    /**
     * Removes a values from the stored by key.
     *
     * @param   string $key
     * @return  mixed
     */
    public function del($key)
    {
        $hash = md5($key);
        $this->_swooleTableKey->del($hash);
        return $this->_swooleTable->del($hash);
    }

    /**
     * Verify if the key exists in the store.
     *
     * @param   string $key
     * @return  mixed
     */
    public function exist($key)
    {
        $hash = md5($key);
        return $this->_swooleTable->exist($hash);
    }

    /**
     * Retrieve all key with value from the cache.
     *
     * @return mixed
     */
    public function getAll()
    {
        $ret = [];
        foreach ($this->_swooleTable as $hash => $value) {
            if ($this->_swooleTableKey->exist($hash)) {
                $key = $this->_swooleTableKey->get($hash);
                $ret[$key['key']] = $value;
            }
        }
        return $ret;
    }
}