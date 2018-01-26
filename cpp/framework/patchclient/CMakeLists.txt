
complice_module("patchclient")

add_library(patch tars_patch.cpp)

add_dependencies(patch FRAMEWORK-JCE)
