add_library(wsv
  wsv.hpp
  backend/postgresql.hpp backend/postgresql.cpp
  )

target_link_libraries(wsv
  pqxx
  )
