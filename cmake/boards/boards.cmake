
function(BOARD_POST_PROCESS target)
    cmake_language(EVAL CODE "
        cmake_language(DEFER CALL \"${__BOARD_POST_PROCESS}\" \"${target}\" )
        ")
endfunction()