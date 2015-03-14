project(gvm)

cmake_minimum_required (VERSION 2.8.7 FATAL_ERROR)


find_package (Boost 1.46.1 REQUIRED COMPONENTS system filesystem thread)

set (Boost_NO_SYSTEM_PATHS ON)
set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME OFF)
set (BOOST_ALL_DYN_LINK OFF)

if (Boost_FOUND)
  # From the offical documentation:
  # Add include directories to the build. [...] If the SYSTEM option is given,
  # the compiler will be told the directories are meant as system include
  # directories on some platforms (signalling this setting might achieve effects
  # such as the compiler skipping warnings [...])."
  include_directories (SYSTEM ${Boost_INCLUDE_DIR})
 
    # From the offical documentation:
    # "Specify directories in which the linker will look for libraries. [...] Note
    # that this command is rarely necessary. Library locations returned by
    # find_package() and find_library() are absolute paths. Pass these absolute
    # library file paths directly to the target_link_libraries() command. CMake
    # will ensure the linker finds them."
    #link_directories (${Boost_LIBRARY_DIRS})
endif ()

add_executable(gvm Tokenizer.cpp TokenDispatcher.cpp Token.cpp Tag.cpp Scheduler.cpp parser.cpp Natives.cpp MatchingUnit.cpp Instruction.cpp IMemory.cpp GVM.cpp Datum.cpp Create_Structure_stmt.cpp Core.cpp Context.cpp )

target_link_libraries (${PROJECT_NAME} ${Boost_LIBRARIES})