# Qt5LinguistTools
find_package(Qt5LinguistTools REQUIRED)
# Manually set the *.ts files. 
list(
    APPEND
    PROJECT_TS
    ${PROJECT_TS}
    ${CMAKE_PROJECT_TS}
)
if(PROJECT_TS)
    option(UPDATE_TS_FILES "Update Qt *.ts files. " OFF)
    if(${UPDATE_TS_FILES})
        qt5_create_translation(
            PROJECT_QM
            ${CMAKE_SOURCE_DIR}
            ${CMAKE_BUILD_DIR}
            ${PROJECT_TS}
            OPTION
            -recursive
        )
    else()
        qt5_add_translation(
            PROJECT_QM
            ${PROJECT_TS}
        )
    endif()
    list(
        APPEND
        PROJECT_SRC
        ${PROJECT_TS}
        ${PROJECT_QM}
    )
    add_custom_target(
        ${PROJECT_NAME}_translator
        DEPENDS
        ${PROJECT_QM}
        SOURCES
        ${PROJECT_TS}
    )
    if(${MSVC})
        add_custom_command(
            TARGET
            ${PROJECT_NAME}_translator
            POST_BUILD
            COMMAND
            ${CMAKE_COMMAND} -E copy ${PROJECT_QM} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release
            COMMENT "Copy ${PROJECT_QM} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
        )
        add_custom_command(
            TARGET
            ${PROJECT_NAME}_translator
            POST_BUILD
            COMMAND
            ${CMAKE_COMMAND} -E copy ${PROJECT_QM} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
            COMMENT "Copy ${PROJECT_QM} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
        )
    else()
        add_custom_command(
            TARGET
            ${PROJECT_NAME}_translator
            POST_BUILD
            COMMAND
            ${CMAKE_COMMAND} -E copy ${PROJECT_QM} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMENT "Copy ${PROJECT_QM} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        )
    endif()
    install(
        FILES
        ${PROJECT_QM}
        DESTINATION
        bin
    )
    if(TARGET ${PROJECT_NAME})
        add_dependencies(
            ${PROJECT_NAME}
            ${PROJECT_NAME}_translator
        )
    elseif(TARGET ${PROJECT_NAME}_main)
        add_dependencies(
            ${PROJECT_NAME}_main
            ${PROJECT_NAME}_translator
        )
    endif()
endif()