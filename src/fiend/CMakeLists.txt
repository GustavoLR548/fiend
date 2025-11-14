# Fiend game executable

# Source files
set(FIEND_SOURCES
    main.c
    ai.c
    astar.c
    collision.c
    console_funcs.c
    draw_level.c
    effect.c
    enemy_update.c
    intro.c
    inventory.c
    link.c
    los.c
    menu.c
    message.c
    missile.c
    notes.c
    npc_update.c
    particle.c
    player.c
    save_menu.c
    savegame.c
    soundplay.c
    trigger_cond.c
    trigger_event.c
    trigger_update.c
    update.c
    weapon.c
)

# Common source files from parent directory
set(COMMON_SOURCES
    ../bmp_array.c
    ../character.c
    ../console.c
    ../draw.c
    ../draw_polygon.c
    ../enemy.c
    ../fiend.c
    ../grafik4.c
    ../item.c
    ../lightmap.c
    ../mapio.c
    ../misc.c
    ../NPC.c
    ../object.c
    ../picdata.c
    ../rotate_sprite.c
    ../sound.c
    ../tile.c
    ../trigger.c
)

# Create executable
add_executable(fiend ${FIEND_SOURCES} ${COMMON_SOURCES})

# Link libraries
target_link_libraries(fiend 
    ${ALLEGRO_LIBRARIES}
    m  # Math library
)

# Link FMOD if available
if(FMOD_FOUND)
    target_link_libraries(fiend ${FMOD_LIBRARY})
endif()

# Set output properties
set_target_properties(fiend PROPERTIES
    OUTPUT_NAME "fiend"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)
