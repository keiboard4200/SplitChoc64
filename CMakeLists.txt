# SplitChoc64 RC13
# No source/API compatibility patching is used.
# 2765 uses Zephyr input events + ZMK's native input-split/input-listener path.

# RC15: local JOY OFF input processor.
target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/input_processor_discard.c)
