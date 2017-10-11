from conans import ConanFile, CMake
import os

class PlexMediaPlayer(ConanFile):
  settings = "os", "compiler", "build_type", "arch"
  options = {"include_desktop": [True, False]}
  default_options = "include_desktop=True"
  generators = "cmake"

  def requirements(self):
    self.requires("web-client-tv2/3.24.0-c4fa2535@plex/public")

    if self.options.include_desktop:
      self.requires("web-client-desktop/3.21.2-8b92636@plex/public")
