// swift-tools-version:5.0

// This file defines Swift package manager support for llbuild. See:
//  https://github.com/apple/swift-package-manager/tree/master/Documentation

import PackageDescription

let package = Package(
    name: "llbuild",
    platforms: [
        .macOS(.v10_10), .iOS(.v9),
    ],
    products: [
        .library(
            name: "libllbuild",
            targets: ["libllbuild"]),
        .library(
            name: "llbuildSwift",
            targets: ["llbuildSwift"]),
        .library(
            name: "llbuildSwiftDynamic",
            type: .dynamic,
            targets: ["llbuildSwift"]),
        .library(
            name: "llbuildAnalysis",
            targets: ["llbuildAnalysis"]),
    ],
    targets: [
        // MARK: Products

        /// The llbuild multitool (primarily for testing).
        .target(
            name: "llbuild",
            dependencies: ["llbuildCommands"],
            path: "products/llbuild"
        ),

        /// The custom build tool used by the Swift package manager (SwiftPM).
        ///
        /// SwiftPM has now switched to using llbuild's Swift bindings API to
        /// build, but this tool is still used for SwiftPM's bootstrapping. Once
        /// that step has been eliminated, this tool can be removed.
        .target(
            name: "swift-build-tool",
            dependencies: ["llbuildBuildSystem"],
            path: "products/swift-build-tool"
        ),

        /// The public llbuild C API.
        .target(
            name: "libllbuild",
            dependencies: ["llbuildCore", "llbuildBuildSystem"],
            path: "products/libllbuild"
        ),

        /// The public llbuild Swift API.
        .target(
            name: "llbuildSwift",
            dependencies: ["libllbuild"],
            path: "products/llbuildSwift",
            exclude: []
        ),

        // MARK: Components
        
        .target(
            name: "llbuildBasic",
            dependencies: ["llvmSupport"],
            path: "lib/Basic"
        ),
        .target(
            name: "llbuildCore",
            dependencies: ["llbuildBasic"],
            path: "lib/Core",
            linkerSettings: [.linkedLibrary("sqlite3")]
        ),
        .target(
            name: "llbuildBuildSystem",
            dependencies: ["llbuildCore"],
            path: "lib/BuildSystem"
        ),
        .target(
            name: "llbuildNinja",
            dependencies: ["llbuildBasic"],
            path: "lib/Ninja"
        ),
        .target(
            name: "llbuildCommands",
            dependencies: ["llbuildCore", "llbuildBuildSystem", "llbuildNinja"],
            path: "lib/Commands"
        ),

        // MARK: Analysis Components
        
        .target(
            name: "llbuildAnalysis",
            dependencies: ["llbuildSwift"],
            path: "lib/Analysis"
        ),
        
        // MARK: Test Targets

        .target(
            name: "llbuildBasicTests",
            dependencies: ["llbuildBasic", "gtestlib"],
            path: "unittests/Basic"),
        .target(
            name: "llbuildCoreTests",
            dependencies: ["llbuildCore", "gtestlib"],
            path: "unittests/Core"),
        .target(
            name: "llbuildBuildSystemTests",
            dependencies: ["llbuildBuildSystem", "gtestlib"],
            path: "unittests/BuildSystem"),
        .target(
            name: "llbuildNinjaTests",
            dependencies: ["llbuildNinja", "gtestlib"],
            path: "unittests/Ninja"),
        .testTarget(
            name: "llbuildSwiftTests",
            dependencies: ["llbuildSwift"],
            path: "unittests/Swift"),
        .testTarget(
            name: "AnalysisTests",
            dependencies: ["llbuildAnalysis"],
            path: "unittests/Analysis"),
        
        // MARK: GoogleTest

        .target(
            name: "gtestlib",
            path: "utils/unittest/googletest/src",
            exclude: [
                "gtest-death-test.cc",
                "gtest-filepath.cc",
                "gtest-port.cc",
                "gtest-printers.cc",
                "gtest-test-part.cc",
                "gtest-typed-test.cc",
                "gtest.cc",
            ]),
        
        // MARK: Ingested LLVM code.
        .target(
          name: "llvmDemangle",
          path: "lib/llvm/Demangle"
        ),

        .target(
            name: "llvmSupport",
            dependencies: ["llvmDemangle"],
            path: "lib/llvm/Support",
            linkerSettings: [.linkedLibrary("ncurses", .when(platforms: [.linux, .macOS]))]
        ),
    ],
    cxxLanguageStandard: .cxx14
)
