cmake_minimum_required(VERSION 3.10)

project(projetIA41)
set(CMAKE_CXX_STANDARD 17)

# --- Fichiers Sources ---
set(SOURCES
        main.cpp
        GameWindow.cpp
        PrologBridge.cpp
)

add_executable(projetIA41 ${SOURCES})

# ============================================================
#                     SWI-PROLOG
# ============================================================

set(SWI_HOME "C:/Program Files/swipl")

target_include_directories(projetIA41 PRIVATE "${SWI_HOME}/include")
target_link_libraries(projetIA41 PRIVATE "${SWI_HOME}/bin/libswipl.lib")


# ============================================================
#                     SFML LOCAL (dans le projet)
# ============================================================

set(SFML_HOME "${CMAKE_SOURCE_DIR}/../external/SFML-2.5.1")

target_include_directories(projetIA41 PRIVATE "${SFML_HOME}/include")
target_link_directories(projetIA41 PRIVATE "${SFML_HOME}/lib")

target_link_libraries(projetIA41 PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
)

# Copier DLL sur Windows
if(WIN32)
    add_custom_command(TARGET projetIA41 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${SFML_HOME}/bin"
            $<TARGET_FILE_DIR:projetIA41>
    )

    add_custom_command(TARGET projetIA41 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${SWI_HOME}/bin"
            $<TARGET_FILE_DIR:projetIA41>
    )
endif()
