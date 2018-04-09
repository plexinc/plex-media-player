# We want OpenGL or OpenGLES2
find_package(OpenGL)
if(NOT OPENGL_FOUND)
  find_package(GLES2)
  if(NOT GLES2_FOUND)
    message(FATAL_ERROR "OpenGL or GLES2 is required")
  else(NOT GLES2_FOUND)
    set(OPENGL_LIBS ${GLES2_LIBRARY})
  endif(NOT GLES2_FOUND)
else(NOT OPENGL_FOUND)
  set(OPENGL_LIBS ${OPENGL_gl_LIBRARY})
endif(NOT OPENGL_FOUND)

# checks inimal mpv version, for reference :
# https://github.com/mpv-player/mpv/blob/master/DOCS/client-api-changes.rst
pkg_check_modules(MPV REQUIRED mpv>=1.23)
if (MPV_FOUND)
    include_directories(${MPV_INCLUDE_DIRS})
    link_directories(${MPV_LIBRARY_DIRS})
endif(MPV_FOUND)
