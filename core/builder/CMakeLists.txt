

ADD_LIBRARY(builder STATIC
    transaction_builder.cpp
    action_builder.cpp
    account_action_builder.cpp

)
target_link_libraries(
    flatbuffers
)
