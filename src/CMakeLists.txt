include_directories(../deps/imgui ../deps/imgui/backends/ ../deps/Bento4/Source/C++/Core
        ../deps/Bento4/Source/C++/MetaData
        ../deps/Bento4/Source/C++/Codecs
        ../deps/Bento4/Source/C++/Crypto
        ../deps/imgui_club/imgui_memory_editor)
aux_source_directory(. SOURCE_FILES)
set(SOURCE_FILES ${SOURCE_FILES}
        ../deps/imgui/imgui.cpp
        ../deps/imgui/imgui_draw.cpp
        ../deps/imgui/imgui_demo.cpp
        ../deps/imgui/imgui_widgets.cpp
        ../deps/imgui/imgui_tables.cpp
        ../deps/imgui/backends/imgui_impl_dx10.cpp
        ../deps/imgui/backends/imgui_impl_win32.cpp
        )
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} D3D10 ap4)
target_sources(${PROJECT_NAME} PRIVATE ../data/MP4Viewer.rc)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION "bin" COMPONENT applications)