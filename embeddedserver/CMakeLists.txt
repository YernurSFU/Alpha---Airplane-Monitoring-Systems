add_subdirectory(c)
file(GLOB EMBEDDED_SERVER_NODE_FILES node/*)
file(COPY ${EMBEDDED_SERVER_NODE_FILES} DESTINATION ${CMAKE_BINARY_DIR}/dst/embeddedserver/nodeserverapp)
