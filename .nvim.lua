vim.diagnostic.disable()   -- Disable diagnostics

-- Switch between header/source with F4 (change between .c & .cpp)
vim.api.nvim_set_keymap('n', '<F4>', ':update<CR>:e %:p:s,.h$,.X123X,:s,\\.cpp$,\\.h,:s,.X123X$,.cpp,<CR>', { noremap = true, silent = true })
vim.api.nvim_set_keymap('i', '<F4>', '<ESC>:update<CR>:e %:p:s,.h$,.X123X,:s,\\.cpp$,\\.h,:s,.X123X$,.cpp,<CR>', { noremap = true, silent = true })
vim.api.nvim_set_keymap('v', '<F4>', '<ESC>:update<CR>:e %:p:s,.h$,.X123X,:s,\\.cpp$,\\.h,:s,.X123X$,.cpp,<CR>', { noremap = true, silent = true })

-- Switch between header/source and search for selected text/text under cursor with Shift+F4
vim.api.nvim_set_keymap('n', '<S-F4>', ":let @/ = expand('<cword>')<CR>:update<CR>:e %:p:s,.h$,.X123X,:s,\\.cpp$,\\.h,:s,.X123X$,.cpp,<CR>ggnzt", { noremap = true, silent = true })
vim.api.nvim_set_keymap('i', '<S-F4>', "<ESC>:let @/ = expand('<cword>')<CR>:update<CR>:e %:p:s,.h$,.X123X,:s,\\.cpp$,\\.h,:s,.X123X$,.cpp,<CR>ggnzt", { noremap = true, silent = true })
vim.api.nvim_set_keymap('v', '<S-F4>', [[y:let @/ = @0<CR>:update<CR>:e %:p:s,.h$,.X123X,:s,\\.cpp$,\\.h,:s,.X123X$,.cpp,<CR>ggnzt]], { noremap = true, silent = true })
