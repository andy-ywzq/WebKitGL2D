list(APPEND JavaScriptCore_SOURCES
    jit/ExecutableAllocatorFixedVMPool.cpp
    jit/ExecutableAllocator.cpp
)

if (APPLE)
    # GENERATOR: disassembler
    add_custom_command(
        OUTPUT ${DERIVED_SOURCES_JAVASCRIPTCORE_DIR}/udis86_itab.h
        MAIN_DEPENDENCY ${JAVASCRIPTCORE_DIR}/disassembler/udis86/itab.py
        DEPENDS ${JAVASCRIPTCORE_DIR}/disassembler/udis86/optable.xml ${JAVASCRIPTCORE_DIR}/disassembler/udis86/ud_opcode.py ${JAVASCRIPTCORE_DIR}/disassembler/udis86/ud_optable.py
        COMMAND ${PYTHON_EXECUTABLE}
        ${JAVASCRIPTCORE_DIR}/disassembler/udis86/itab.py
        ${JAVASCRIPTCORE_DIR}/disassembler/udis86/optable.xml --outputDir ${DERIVED_SOURCES_JAVASCRIPTCORE_DIR}
        VERBATIM)
    ADD_SOURCE_DEPENDENCIES(${JAVASCRIPTCORE_DIR}/disassembler/UDis86Disassembler.cpp ${DERIVED_SOURCES_JAVASCRIPTCORE_DIR}/udis86_itab.h)
    list(APPEND JavaScriptCore_SOURCES
        disassembler/UDis86Disassembler.cpp
        disassembler/udis86/udis86.c
        disassembler/udis86/udis86_decode.c
        disassembler/udis86/udis86_input.c
        disassembler/udis86/udis86_itab_holder.c
        disassembler/udis86/udis86_syn-intel.c
        disassembler/udis86/udis86_syn-att.c
        disassembler/udis86/udis86_syn.c
    )
    list(APPEND JavaScriptCore_INCLUDE_DIRECTORIES
        ${JAVASCRIPTCORE_DIR}/disassembler/udis86
    )
endif ()

list(APPEND JavaScriptCore_LIBRARIES
    ${ICU_I18N_LIBRARIES}
)

list(APPEND JavaScriptCore_INCLUDE_DIRECTORIES
    ${ICU_INCLUDE_DIRS}
    ${WTF_DIR}/wtf/gobject
)
