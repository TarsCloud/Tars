PHP_ARG_ENABLE(phptars, whether to enable phptars support,
Make sure that the comment is aligned:
[  --enable-phptars           Enable phptars support])

if test "$PHP_PHPTARS" != "no"; then

  PHP_NEW_EXTENSION(phptars, tupapi.c ttars.c tup_c.c tars_c.c, $ext_shared)
fi

if test -z "$PHP_DEBUG"; then
    AC_ARG_ENABLE(debug,
    [ --enable-debug      compile with debugging symbols],[
    PHP_DEBUG=$enableval
    ],[ PHP_DEBUG=no
    ])
fi

PHP_ARG_ENABLE(phptars-gcov, whether to enable php gcov,
[  --enable-gcov   To enable code coverage reporting], no, no)

if test "$PHP_PHPTARS_GCOV" != "no"; then
    CFLAGS="$CFLAGS -O0 -fprofile-arcs -ftest-coverage"
    CXXFLAGS="$CXXFLAGS -O0 -fprofile-arcs -ftest-coverage"
fi