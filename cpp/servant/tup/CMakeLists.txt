set(OUT_DEPENDS_LIST)

complice_tars(OUT_DEPENDS_LIST "")

add_custom_target(TUP-TARS ALL DEPENDS ${OUT_DEPENDS_LIST})
