from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class cpputils2Recipe(ConanFile):
    name = "cpputils2"
    version = "1.0"
    #package_type = "library"

    # Optional metadata
    license = "MIT"
    author = "Yangosoft yangosoft@protonmail.com"
    url = "https://github.com/yangosoft/cpputils2"
    description = "Set of utils in C++23"
    topics = ("shared memory", "mutex", "futex", "memory", "shared")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "examples/*"

    #def config_options(self):
        #if self.settings.os == "Windows":
            #self.options.rm_safe("fPIC")

    #def configure(self):
        #if self.options.shared:
            #self.options.rm_safe("fPIC")

    #def layout(self):
    #    cmake_layout(self)
    
    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        #self.cpp_info.libs = ["cpputils2"]
        self.cpp_info.set_property("cmake_target_name", "CPPUTILS2::cpputils2")
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
        #self.cpp_info.set_property("cmake_find_mode", "none")

