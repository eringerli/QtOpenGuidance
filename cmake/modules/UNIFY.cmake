
macro(addToUnifyGroupAndSources srcs group)
  set_source_files_properties(${srcs} PROPERTIES UNITY_GROUP ${group})
  set(SOURCES ${SOURCES} ${srcs})
endmacro()
