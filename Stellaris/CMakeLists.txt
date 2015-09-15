PROJECT(Levitor)

MACRO(set_cross_compiling prefix)
  # specify the cross compiler
  SET(CPREF "/usr/bin/arm-none-eabi-")

  # Should be left untouched
  SET(CMAKE_C_COMPILER   "${CPREF}gcc")
  SET(CMAKE_CXX_COMPILER   "${CPREF}g++")
  SET(AS "${CPREF}gcc -x -assembler-with-cpp")
  SET(AR "${CPREF}ar")  
  SET(LD "${CPREF}ld")  
  SET(NM "${CPREF}nm")
ENDMACRO(set_cross_compiling)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake")
SET(BINARY_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")

MACRO(stellaris_exe linker_script)
  FIND_PACKAGE(Stellarisware REQUIRED)
  set_cross_compiling("arm-none-eabi-")
  SET(PART "LM4F120H5QR")
  SET(CPU "-mcpu=cortex-m4")
  SET(FPU "-mfpu=fpv4-sp-d16 -mfloat-abi=softfp")
  SET(A_FLAGS "-Wa,-mthumb,-mcpu=cortex-m4,-mfpu=fpv4-sp-d16,-mfloat-abi=softfp,-MD")
  SET(CMAKE_C_FLAGS "-mthumb ${CPU} ${FPU} -g -ffunction-sections -fdata-sections -MD -std=c99 -Wall -pedantic -DPART_${PART} -DTARGET_IS_BLIZZARD_RA1 -Wall -O0"  )
  SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-exceptions")
  set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
  set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

#Linker script for stellaris
  SET(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/${linker_script}")
  SET(CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT} --entry ResetISR -Wl,--gc-sections")
ENDMACRO(stellaris_exe)

FUNCTION(configure_for_stellaris tname )
  SET(target_name ${ARGV0})
  LIST(REMOVE_AT ARGV 0)
  ADD_EXECUTABLE(${target_name} ${ARGV})
  SET_TARGET_PROPERTIES(${target_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  SET_TARGET_PROPERTIES(${target_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  INCLUDE_DIRECTORIES(${target_name}  ${Stellarisware_INCLUDE_DIRS})
  TARGET_LINK_LIBRARIES(${target_name} ${Stellarisware_LIBRARIES})
  produce_flash(${target_name})
ENDFUNCTION(configure_for_stellaris)

MACRO(produce_flash exec_name)
  GET_TARGET_PROPERTY(miopercorsodelbin ${exec_name} RUNTIME_OUTPUT_DIRECTORY)
  ADD_CUSTOM_COMMAND(TARGET ${exec_name}
    POST_BUILD
    COMMAND ${CPREF}objcopy -O binary "${miopercorsodelbin}/${exec_name}" "${miopercorsodelbin}/${exec_name}.bin"
    #   COMMAND rm ${exec_name}
    )

ENDMACRO(produce_flash)

INCLUDE_DIRECTORIES(".")
add_subdirectory(ConstantField/)
