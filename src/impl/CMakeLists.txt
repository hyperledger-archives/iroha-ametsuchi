set(ametsuchi_SOURCES
    ametsuchi.cpp
    )

add_library(ametsuchi
    ${block_store_SOURCES}
    ${index_SOURCES}
    ${wsv_SOURCES}
    ${ametsuchi_SOURCES}
    )

target_link_libraries(ametsuchi
  block_store
  index
  wsv
  )
