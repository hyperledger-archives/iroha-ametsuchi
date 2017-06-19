set(wsv_SOURCES
    backend/postgresql.cpp
    )

add_library(wsv
  ${wsv_SOURCES}
  )

target_link_libraries(wsv
  pqxx
  )
