SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

###########################
#      bm_validation      #
###########################

add_executable(bm_validation
  bm_validation.cpp
)

target_link_libraries(bm_validation
  flatbuffer_service
  flatbuffers
  hash
  signature
  base64
)


###########################
#      bm_connection      #
###########################

#add_executable(bm_connection
#  bm_connection.cpp
#)

#target_link_libraries(bm_connection
#  flatbuffer_service
#  flatbuffers
#  hash
#  signature
#  base64
#  config_manager
#)


###########################
#   bm_validation_memory  #
###########################

add_executable(bm_validation_memory
  bm_validation_memory.cpp
)

target_link_libraries(bm_validation_memory
  flatbuffer_service
  flatbuffers
  hash
  signature
  base64
  timer
  json
)
