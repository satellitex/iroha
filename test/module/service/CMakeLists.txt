###########################
# protocol_to_string_test #
###########################
add_executable(protocol_to_string_test
  protocol_to_string_test.cpp
)
target_link_libraries(protocol_to_string_test
  gtest
  protocol_to_string
  random_generator
)
add_test(
  NAME protocol_to_string_test
  COMMAND $<TARGET_FILE:protocol_to_string_test>
)
