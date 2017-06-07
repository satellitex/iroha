ADD_LIBRARY(flatbuffer_service STATIC
    flatbuffer_service.cpp
)
target_link_libraries(flatbuffer_service
    flatbuffers
    block_builder
    config_manager
)
