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
        self.requires("im/3.15")
        self.requires("wxwidgets/[~3.2]")
        self.requires("libxml2/[>=2.10 <3]")
        self.requires("doctest/[~2.4]")

    def configure(self):
        # IM's upstream Windows prebuilt im_fftw3.lib references
        # __imp_fftw_* symbols, i.e. it expects FFTW as a DLL with the
        # import-stub naming convention. Force the shared build of fftw
        # on Windows so those symbols resolve.
        if self.settings.os == "Windows":
            self.options["fftw/*"].shared = True

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
