cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)


# Static linking CRT for MSVC
if (DEFINED ENV{VC_CRT_STATIC_LINK})
    foreach (flag_var
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if (${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif ()
    endforeach ()
endif ()


# vcpkg libraries
if (DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
    set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "")
endif ()

if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif ()


project(AcousticSimulator)

# standard compiler warnings
set(MSVC_WARNINGS
        /W4 # Baseline reasonable warnings
        /w14242 # 'identifier': conversion from 'type1' to 'type1', possible loss
        # of data
        /w14254 # 'operator': conversion from 'type1:field_bits' to
        # 'type2:field_bits', possible loss of data
        /w14263 # 'function': member function does not override any base class
        # virtual member function
        /w14265 # 'classname': class has virtual functions, but destructor is not
        # virtual instances of this class may not be destructed correctly
        /w14287 # 'operator': unsigned/negative constant mismatch
        /we4289 # nonstandard extension used: 'variable': loop control variable
        # declared in the for-loop is used outside the for-loop scope
        /w14296 # 'operator': expression is always 'boolean_value'
        /w14311 # 'variable': pointer truncation from 'type1' to 'type2'
        /w14545 # expression before comma evaluates to a function which is missing
        # an argument list
        /w14546 # function call before comma missing argument list
        /w14547 # 'operator': operator before comma has no effect; expected
        # operator with side-effect
        /w14549 # 'operator': operator before comma has no effect; did you intend
        # 'operator'?
        /w14555 # expression has no effect; expected expression with side- effect
        /w14619 # pragma warning: there is no warning number 'number'
        /w14640 # Enable warning on thread un-safe static member initialization
        /w14826 # Conversion from 'type1' to 'type_2' is sign-extended. This may
        # cause unexpected runtime behavior.
        /w14905 # wide string literal cast to 'LPSTR'
        /w14906 # string literal cast to 'LPWSTR'
        /w14928 # illegal copy-initialization; more than one user-defined
        # conversion has been implicitly applied
        /permissive- # standards conformance mode for MSVC compiler.
        )
# Treat warnings as errors
set(MSVC_WARNINGS ${MSVC_WARNINGS} /WX)
set(PROJECT_WARNINGS ${MSVC_WARNINGS})


# Files
add_subdirectory(ComputeEngine)