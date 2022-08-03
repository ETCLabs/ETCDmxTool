# libGadget

This repository contains a Windows DLL and headers for interfacing with ETC
Gadget products.
[Gadget II](https://www.etcconnect.com/Products/Networking/Gadget-II/Features.aspx)
is a 2-port USB-to-DMX512 converter sold by [ETC](https://www.etcconnect.com).
It supports DMX, RDM and "Raw Mode" - sending and receiving arbitrary serial
data on the DMX bus. All of these features can be accessed through this DLL
interface. Legacy one-port Gadgets are also supported.

32- and 64-bit DLLs are provided. The DLLs are built with the MSVC 2017
toolchain. Currently, non-Windows platforms are not supported.

To include libGadget in a CMake project, you can use `find_package()`:

```cmake
if(CMAKE_SIZEOF_VOID_P STREQUAL 4)
  list(APPEND CMAKE_PREFIX_PATH ${PATH_TO_LIBGADGET}/libGadget/Win32)
else()
  list(APPEND CMAKE_PREFIX_PATH ${PATH_TO_LIBGADGET}/libGadget/x64)
endif()

find_package(GadgetDLL 2.1.0 REQUIRED)
```

Then link the export library:

```cmake
target_link_libraries(YourApp PRIVATE GadgetDLL::GadgetDLL)
```

Additionally, it's convenient to add a post-build step to copy the DLL to the
directory containing your executable:
```cmake
get_target_property(GADGET_DLL_LOCATION GadgetDLL::GadgetDLL IMPORTED_LOCATION_RELEASE)
add_custom_command(
  TARGET YourApp
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${GADGET_DLL_LOCATION} $<TARGET_FILE_DIR:YourApp>
  COMMENT "Copying Gadget DLL to executable directory..."
)
```

To include libGadget in a non-CMake project, you can add the relevant
directories manually. The export library is located in lib/, the DLL is in bin/,
and the headers are in include/.
