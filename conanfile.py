from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.cmake import CMakeDeps, CMakeToolchain, cmake_layout


class AruspixConan(ConanFile):
    name = "aruspix"
    settings = "os", "compiler", "build_type", "arch"
    generators = "VirtualBuildEnv"

    def requirements(self):
        # The IM toolkit is built from a local recipe (recipes/im).
        # Run `conan export recipes/im` once before `conan install`.
        # IM is being phased out in favor of OpenCV; see karsten/opencv branch.
        self.requires("im/3.15")
        self.requires("opencv/[~4.10]")
        self.requires("wxwidgets/[~3.2]")
        self.requires("libxml2/[>=2.10 <3]")
        self.requires("doctest/[~2.4]")
        # OpenCV pins exact versions for several lower-level deps that
        # other parts of the graph resolve via open ranges. Force the
        # whole graph onto OpenCV's pinned versions to avoid conflicts.
        self.requires("libjpeg/9e", override=True)
        self.requires("libtiff/4.6.0", override=True)

    def configure(self):
        # IM's upstream Windows prebuilt im_fftw3.lib references
        # __imp_fftw_* symbols, i.e. it expects FFTW as a DLL with the
        # import-stub naming convention. Force the shared build of fftw
        # on Windows so those symbols resolve.
        if self.settings.os == "Windows":
            self.options["fftw/*"].shared = True
        # We use OpenCV for image processing, not video. Disabling ffmpeg
        # avoids a CMake-config bug in OpenCV's videoio module that
        # otherwise makes the build fail.
        self.options["opencv/*"].with_ffmpeg = False

    def validate(self):
        # Windows is 64-bit only. Guard against x86 vcvars shells or a
        # stale profile silently producing a 32-bit build that then fails
        # to link against x64 dependencies.
        if self.settings.os == "Windows" and str(self.settings.arch) != "x86_64":
            raise ConanInvalidConfiguration(
                f"Windows builds require arch=x86_64 (got {self.settings.arch}). "
                "Open the x64 Native Tools Command Prompt, or pass "
                "-s arch=x86_64 -s:b arch=x86_64 to `conan install`."
            )

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()
