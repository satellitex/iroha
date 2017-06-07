###########################
#    flatbuffer_service   #
###########################
ADD_LIBRARY(flatbuffer_service STATIC
  flatbuffer_service.cpp
)
target_link_libraries(flatbuffer_service
  flatbuffers
  block_builder
  config_manager
)

###########################
#    protocol_to_string   #
###########################
ADD_LIBRARY(protocol_to_string STATIC
  protocol_to_string.cpp
)
target_link_libraries(protocol_to_string
  flatbuffers
)
