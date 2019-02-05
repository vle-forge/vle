function(vle_declare_test test_name sources)
    add_executable(${test_name} ${sources})

    target_include_directories(${test_name}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR})

    target_compile_definitions(${test_name}
        PRIVATE
        $<$<BOOL:${WITH_FULL_OPTIMIZATION}>:VLE_FULL_OPTIMIZATION>
        $<$<NOT:$<BOOL:${WITH_DEBUG}>>:VLE_DISABLE_DEBUG>
        $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
        $<$<CXX_COMPILER_ID:MSVC>:_SCL_SECURE_NO_WARNINGS>
        VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        VERSION_MINOR=${PROJECT_VERSION_MINOR}
        VERSION_PATCH=${PROJECT_VERSION_PATCH})

    set_target_properties(${test_name}
        PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON)

    target_link_libraries(${test_name}
        PRIVATE
        libvle
        threads
        Boost::boost
        EXPAT::EXPAT
        $<$<PLATFORM_ID:Linux>:dl>)

    add_test(${test_name} ${test_name})
endfunction()


add_subdirectory(devs)
add_subdirectory(manager)
add_subdirectory(utils)
add_subdirectory(value)
add_subdirectory(vpz)

if (VLE_HAVE_GVLE)
    add_subdirectory(gvle)
endif ()
