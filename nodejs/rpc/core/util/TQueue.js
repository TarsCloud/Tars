var TQueue = function () {
    this._queue = {};
};
module.exports.TQueue = TQueue;

// 获取内部的队列
TQueue.prototype.__defineGetter__("queue", function () { return this._queue; });

// 内部队列的遍历方法
TQueue.prototype.forEach = function ($cbFunc) {
    for (var key in this._queue) {
        if ($cbFunc.call(this, key, this._queue[key], this._queue) === false) break;
    }
};

// 根据唯一ID将数据插入队列中
// 如果数据重复，则覆盖老的数据
TQueue.prototype.push = function ($iUniID, $message) {
    this._queue[$iUniID] = $message;
};

// 根据唯一ID将数据从队列中取出，同时根据bErase决定是否从队列中删除
// 如果存在则返回相应数据，如果不存在返回undefined
TQueue.prototype.pop  = function ($iUniID, $bErase) {
    var temp = this._queue[$iUniID];
    if ($bErase && $bErase === true) {
        delete this._queue[$iUniID];
    }
    return temp;
};

// 根据唯一ID将数据从队列中删除
TQueue.prototype.erase = function ($iUniID) {
    delete this._queue[$iUniID];
};

//清空所有数据
TQueue.prototype.clear = function () {
    this._queue = {};
};
