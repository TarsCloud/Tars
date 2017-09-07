PHP_ARG_ENABLE(phptars, whether to enable phptars support,
Make sure that the comment is aligned:
[  --enable-phptars           Enable phptars support])

if test "$PHP_PHPTARS" != "no"; then

  PHP_NEW_EXTENSION(phptars, tupapi.c ttars.c tup_c.c tars_c.c, $ext_shared)
fi
