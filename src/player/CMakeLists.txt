add_sources(PlayerComponent.cpp PlayerComponent.h)
add_sources(PlayerQuickItem.cpp PlayerQuickItem.h)
add_sources(CodecsComponent.cpp CodecsComponent.h)
add_sources(OpenGLDetect.cpp OpenGLDetect.h)

if (AML_FOUND)
  add_subdirectory(aml)
endif(AML_FOUND)
