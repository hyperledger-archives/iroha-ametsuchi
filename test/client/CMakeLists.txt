AddTest(sample_app_test sample_app_test.cc)
target_link_libraries(sample_app_test
  ametsuchiclient)

#TODO uncomment when block processing is implemented
#add_executable(integration_test integration_test.cc)
#target_link_libraries(integration_test gtest ametsuchiclient)
#StrictMode(integration_test)
