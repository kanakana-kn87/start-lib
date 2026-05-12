plugins {
    id("com.android.application") version "8.5.0-rc01" // ここにバージョンが必要！
    id("org.jetbrains.kotlin.android") version "1.9.22"
}

android {
    namespace = "framework.start.tspp"
    compileSdk = 34
    ndkVersion = "27.0.12077973"

    compileOptions {
        // Javaコンパイラのターゲットを21に固定
        sourceCompatibility = JavaVersion.VERSION_21
        targetCompatibility = JavaVersion.VERSION_21
    }

    // ソースの場所を「自分（srcフォルダ）自身」に設定
    sourceSets {
        getByName("main") {
            java.setSrcDirs(listOf("os/android"))
            manifest.srcFile("os/android/AndroidManifest.xml")
            res.setSrcDirs(listOf("os/android/res")) // 必要なら
        }
    }

    externalNativeBuild {
        cmake {
            path = file("../CMakeLists.txt") // 1つ上のCMakeListsを見に行く
        }
    }
    defaultConfig {
        minSdk = 21
        targetSdk = 34
        externalNativeBuild {
            cmake {
                // tomlplusplus-config.cmake があるフォルダを指定
                // ${project.rootDir} を使うと確実だよ！
                arguments += "-Dtomlplusplus_DIR=${project.rootDir}/build/android-debug/generators"
                arguments += "-DCMAKE_PREFIX_PATH=${project.rootDir}/build/android-debug/generators"
            }
        }
    }
}