file(GLOB_RECURSE FILES RELATIVE "${INPUT_DIR}" "${INPUT_DIR}/*")

set(HPP_CONTENT "#pragma once\n#include <string_view>\n#include <map>\n\nnamespace start::assets {\n")

foreach(FILE ${FILES})
    # ファイル名から変数名を作れない文字を置換
    string(MAKE_C_IDENTIFIER "${FILE}" VAR_NAME)
    file(READ "${INPUT_DIR}/${FILE}" RAW_DATA HEX)
    string(REGEX MATCHALL ".." HEX_LIST "${RAW_DATA}")
    string(JOIN ", 0x" HEX_ARRAY ${HEX_LIST})

    string(APPEND HPP_CONTENT "    inline constexpr unsigned char ${VAR_NAME}[] = { 0x${HEX_ARRAY} };\n")
    
    # マップ登録用のリストを作っておく
    list(APPEND ASSET_MAP "{\"${FILE}\", {reinterpret_cast<const char*>(${VAR_NAME}), sizeof(${VAR_NAME})}}")
endforeach()

string(JOIN ",\n        " ASSET_MAP_STR ${ASSET_MAP})

# ここで namespace start::assets を閉じる！
string(APPEND HPP_CONTENT "\n    inline const std::map<std::string_view, std::string_view> all = {\n        ${ASSET_MAP_STR}\n    };\n}")

# 🚨 ここにあった余分な "}\n" を削除したよ！

file(WRITE "${OUTPUT_FILE}" "${HPP_CONTENT}")