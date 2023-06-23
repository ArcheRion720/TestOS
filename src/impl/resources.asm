global consolas_font
global test_elf_file
section .data

consolas_font:
    incbin "res/consolas.psf"

test_elf_file:
    incbin "res/file.elf"
