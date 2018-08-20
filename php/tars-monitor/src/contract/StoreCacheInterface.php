<?php

namespace Tars\monitor\contract;

interface StoreCacheInterface
{
    public function __construct($config);

    /**
     * Retrieve an item from the cache by key.
     *
     * @param  string $key
     * @return mixed
     */
    public function get($key);

    /**
     * Retrieve an filed value from the cache by key and field.
     *
     * @param  string $key
     * @param  string $field
     * @return mixed
     */
    public function getField($key, $field);

    /**
     * Retrieve all key with value from the cache.
     *
     * @return mixed
     */
    public function getAll();

    /**
     * Set an item into cache.
     *
     * @param   string $key
     * @param   array $value
     * @return  mixed
     */
    public function set($key, $value);

    /**
     * Increment the int|float value of a key field by the given amount.
     *
     * @param   string $key
     * @param   string $field
     * @param   mixed $incrby
     * @return  mixed
     */
    public function incrField($key, $field, $incrby = 1);

    /**
     * Decrement the int|float value of a key field by the given amount.
     *
     * @param   string $key
     * @param   string $field
     * @param   mixed $decrby
     * @return  mixed
     */
    public function decrField($key, $field, $decrby = 1);

    /**
     * Removes a values from the stored by key.
     *
     * @param   string $key
     * @return  mixed
     */
    public function del($key);

    /**
     * Verify if the key exists in the store.
     *
     * @param   string $key
     * @return  mixed
     */
    public function exist($key);
}