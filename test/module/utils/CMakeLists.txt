###########################
#      datetime test      #
###########################
add_executable(datetime_test
    datetime_test.cpp
)
target_link_libraries(datetime_test
    gtest
    datetime
)
add_test(
    NAME datetime_test
    COMMAND $<TARGET_FILE:datetime_test>
)
