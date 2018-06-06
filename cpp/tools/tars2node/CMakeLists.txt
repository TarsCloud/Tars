# version
set(PARSER_VERSION "${TARS_VERSION}")
set(GENERATOR_VERSION "20180606")

# namespace
set(IDL_NAMESPACE Tars)
string(TOLOWER ${IDL_NAMESPACE} TC_NAMESPACE)
set(GRAMMAR_NAME Grammar)

# idl
set(RPC_MODULE_PATH "@tars/rpc")
set(STREAM_MODULE_PATH "@tars/stream")
set(IDL_TYPE "Tars")

# flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O2 -Wall")

# define
add_definitions(-DPARSER_VERSION="${PARSER_VERSION}")
add_definitions(-DGENERATOR_VERSION="${GENERATOR_VERSION}")
add_definitions(-DTC_NAMESPACE=${TC_NAMESPACE})
add_definitions(-DIDL_NAMESPACE=${IDL_NAMESPACE})
add_definitions(-DGRAMMAR_NAME=${GRAMMAR_NAME})
add_definitions(-DRPC_MODULE_PATH="${RPC_MODULE_PATH}")
add_definitions(-DSTREAM_MODULE_PATH="${STREAM_MODULE_PATH}")
add_definitions(-DEXECUTE_FILENAME="${PROJECT_NAME}")
add_definitions(-DIDL_TYPE="${IDL_TYPE}")


tars2language("tars2node")
