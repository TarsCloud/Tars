<?php
namespace App\Core;

define('DIRWATCHER_CHANGED', IN_MODIFY | IN_CLOSE_WRITE | IN_MOVE | IN_CREATE | IN_DELETE );

/**
 * DirWatcher
 *
 * @author Charles Tang <charlestang AT foxmail DOT com>
 */
class DirWatcher {

    private $_callbacks = array();
    private $_directories = array();
    private $_inotify = null;

    public function __construct() {
        $this->_inotify = inotify_init();
    }


    public function addDirectory($path, $mask = DIRWATCHER_CHANGED) {
        $key = md5($path);
        if (!isset($this->_directories[$key])) {
            $wd = inotify_add_watch($this->_inotify, $path, $mask);
            $this->_directories[$key] = array(
                'wd' => $wd,
                'path' => $path,
                'mask' => $mask,
            );
        }
    }

    public function removeDirectory($path) {
        $key = md5($path);
        if (isset($this->_directories[$key])) {
            $wd = $this->_directories[$key]['wd'];
            if (inotify_rm_watch($this->_inotify, $wd)) {
                unset($this->_directories[$key]);
            }
        }
    }

    public function addDirectories($directories) {
        foreach ($directories as $dir) {
            if (!is_array($dir)) {
                $this->addDirectory($dir);
            } else {
                $this->addDirectory($dir['path'], $dir['mask']);
            }
        }
    }

    public function addCallback($callback, $params = array(), $priority = 9) {
        $key = md5(var_export($callback, true));
        if (!isset($this->_callbacks[$key])) {
            $this->_callbacks[$key] = array(
                'callable' => $callback,
                'params' => $params,
                'priority' => $priority,
            );

            usort($this->_callbacks, create_function('$a, $b', 'return $a["priority"] > $b["priority"];'));
        }
    }

    public function removeCallback($callback) {
        $key = md5(var_export($callback, true));
        if (isset($this->_callbacks[$key])) {
            unset($this->_callbacks[$key]);
        }
    }

    public function addCallbacks($callbacks) {
        foreach ($callbacks as $callable) {
            if (is_callable($callable)) {
                $callable = array(
                    'callable' => $callable,
                    'params' => array(),
                    'priority' => 9,
                );
            }

            $this->addCallback($callable['callable'], $callable['params'], $callable['priority']);
        }
    }

    public function startWatch() {
        while (TRUE) { //启动一个常驻进程，监视目录的变化，事件触发回调函数
            $event = inotify_read($this->_inotify);

            if (defined('DIRWATCHER_DEBUG') && DIRWATCHER_DEBUG) {
                error_log(vsprintf("[wd:%d][mask:%d][cookie:%s]%s", $event[0]));
            }

            foreach ($this->_callbacks as $callable) {
                call_user_func_array($callable['callable'], array_merge($event, $callable['params']));
            }
        }
    }

    public function stopWatch() {
        //没有实现，可以引入pcntl，优雅退出，退出前记得fclose($this->_inotify)
    }

}