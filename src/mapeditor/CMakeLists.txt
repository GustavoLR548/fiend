# Map editor executable

# Editor-specific source files
set(MAPEDITOR_SOURCES
    mapedit.c
    console_funcs.c
    d_area.c
    d_brush.c
    d_enemy.c
    d_global_trigger.c
    d_item.c
    d_light.c
    d_link.c
    d_lookat_area.c
    d_misc.c
    d_npc.c
    d_object.c
    d_pathnode.c
    d_sound.c
    d_trigger.c
    mapdraw.c
    menu_edit.c
    menu_file.c
    menu_misc.c
    menu_options.c
)

# Common source files from parent directory
set(COMMON_SOURCES
    ../audio.c
    ../bmp_array.c
    ../character.c
    ../console.c
    ../draw.c
    ../enemy.c
    ../fiend.c
    ../grafik4.c
    ../item.c
    ../lightmap.c
    ../mapio.c
    ../NPC.c
    ../object.c
    ../picdata.c
    ../rotate_sprite.c
    ../sound.c
    ../tile.c
    ../trigger.c
)

# Create executable
add_executable(mapeditor ${MAPEDITOR_SOURCES} ${COMMON_SOURCES})

# Include miniaudio vendor directory
target_include_directories(mapeditor PRIVATE 
    ${CMAKE_SOURCE_DIR}/vendor
)

# Link libraries
target_link_libraries(mapeditor 
    ${ALLEGRO_LIBRARIES}
    m       # Math library
    pthread # Required by miniaudio
    dl      # Required by miniaudio for dlopen
)

# Set output properties
set_target_properties(mapeditor PROPERTIES
    OUTPUT_NAME "mapeditor"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)
