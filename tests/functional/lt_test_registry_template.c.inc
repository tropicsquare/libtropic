// TEMPLATE FOR CMAKE AUTOMATIC GENERATOR
// This file will be automatically populated depending on tests
// defined in LIBTROPIC_TEST_LIST.

#ifdef LT_BUILD_TESTS
  #ifdef LT_CMAKE_DUMMY_TEST // This does nothing, only a dummy first branch for following elifs generated by CMake.
  @LIBTROPIC_TEST_FUNCTIONS_CONTENT@
  #else
    #error "No test selected! Check if macros are correctly defined."
  #endif
#endif