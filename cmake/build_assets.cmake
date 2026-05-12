# cmake/build_assets.cmake

function(setup_assets_packing TARGET_NAME INPUT_DIR OUTPUT_FILE)
    file(GLOB_RECURSE ASSET_FILES "${INPUT_DIR}/*")
    
    # この関数が定義されているディレクトリ（cmake/）を取得
    set(CURRENT_CMAKE_MODULE_DIR "${CMAKE_CURRENT_FUNCTION_LIST_DIR}")
    
    add_custom_command(
        OUTPUT "${OUTPUT_FILE}"
        COMMAND ${CMAKE_COMMAND} 
            -DINPUT_DIR="${INPUT_DIR}" 
            -DOUTPUT_FILE="${OUTPUT_FILE}" 
            # 🟢 修正：関数がある場所の隣にある pack_assets.cmake を確実に指す
            -P "${CURRENT_CMAKE_MODULE_DIR}/pack_assets.cmake"
        DEPENDS ${ASSET_FILES}
        COMMENT "start.ts++: assets フォルダをパッキング中...📦✨"
    )
    
    target_sources(${TARGET_NAME} PRIVATE "${OUTPUT_FILE}")
endfunction()