ADD_LIBRARY(block_builder STATIC
    block_builder.cpp
)
target_link_libraries(block_builder
    datetime
    flatbuffers
    flatbuffer_service
    base64
    signature
    config_manager
)
