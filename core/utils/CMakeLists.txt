###########################
#        expected         #
###########################
ADD_LIBRARY(expected STATIC
  expected.cpp
  exception.cpp
)

###########################
#        datetime         #
###########################
ADD_LIBRARY(datetime STATIC datetime.cpp)

###########################
#         logger          #
###########################
ADD_LIBRARY(logger STATIC logger.cpp)
target_link_libraries(logger
  datetime
  spdlog
)

###########################
#        ip_tools         #
###########################
add_library(ip_tools STATIC ip_tools.cpp)
target_link_libraries(ip_tools
  logger
)

###########################
#     random_generator    #
###########################
add_library(random_generator STATIC random_generator.cpp)
target_link_libraries(random_generator
  hash
  base64
  signature
  flatbuffers
)
