var TarsError = {
    SUCCESS : 0,
    SERVER  : {
        DECODE_ERROR        : -1,
        ENCODE_ERROR        : -2,
        FUNC_NOT_FOUND      : -3,
        SERVANT_NOT_FOUND   : -4,
        QUEUE_TIMEOUT       : -6,
        TARSSERVEROVERLOAD   : -9,
        TARSADAPTERNULL      : -10
    },
    CLIENT : {
        ADAPTER_NOT_FOUND   : -10000,
        DECODE_ERROR        : -11000,
        CONN_CLOSED         : -12001,
        COON_ERROR          : -12002,
        REQUEST_TIMEOUT     : -13001
    }
};

module.exports.TarsError = TarsError;
