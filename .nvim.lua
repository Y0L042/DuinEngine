-- vim.diagnostic.disable()   -- Disable diagnostics
--
-- -- Switch between header/source with F4 (change between .c & .cpp)
-- vim.api.nvim_set_keymap('n', '<F4>', ':update<CR>:e %:p:s,.h$,.X123X,:s,\\.cpp$,\\.h,:s,.X123X$,.cpp,<CR>', { noremap = true, silent = true })
-- vim.api.nvim_set_keymap('i', '<F4>', '<ESC>:update<CR>:e %:p:s,.h$,.X123X,:s,\\.cpp$,\\.h,:s,.X123X$,.cpp,<CR>', { noremap = true, silent = true })
-- vim.api.nvim_set_keymap('v', '<F4>', '<ESC>:update<CR>:e %:p:s,.h$,.X123X,:s,\\.cpp$,\\.h,:s,.X123X$,.cpp,<CR>', { noremap = true, silent = true })
--
-- -- Switch between header/source and search for selected text/text under cursor with Shift+F4
-- vim.api.nvim_set_keymap('n', '<S-F4>', ":let @/ = expand('<cword>')<CR>:update<CR>:e %:p:s,.h$,.X123X,:s,\\.cpp$,\\.h,:s,.X123X$,.cpp,<CR>ggnzt", { noremap = true, silent = true })
-- vim.api.nvim_set_keymap('i', '<S-F4>', "<ESC>:let @/ = expand('<cword>')<CR>:update<CR>:e %:p:s,.h$,.X123X,:s,\\.cpp$,\\.h,:s,.X123X$,.cpp,<CR>ggnzt", { noremap = true, silent = true })
-- vim.api.nvim_set_keymap('v', '<S-F4>', [[y:let @/ = @0<CR>:update<CR>:e %:p:s,.h$,.X123X,:s,\\.cpp$,\\.h,:s,.X123X$,.cpp,<CR>ggnzt]], { noremap = true, silent = true })


-- Function to switch between header and source files
function switch_source_header()
    local filepath = vim.fn.expand('%:p')
    local filename = vim.fn.expand('%:t')
    local dir = vim.fn.expand('%:p:h')
    local base = vim.fn.expand('%:t:r')

    local header_extensions = { '.h', '.hpp' }
    local source_extensions = { '.c', '.cpp', '.cc', '.cxx' }

    local current_ext = vim.fn.expand('%:e')
    local target_files = {}

    if vim.tbl_contains(header_extensions, '.' .. current_ext) then
        for _, ext in ipairs(source_extensions) do
            table.insert(target_files, dir .. '/' .. base .. ext)
        end
    elseif vim.tbl_contains(source_extensions, '.' .. current_ext) then
        for _, ext in ipairs(header_extensions) do
            table.insert(target_files, dir .. '/' .. base .. ext)
        end
    else
        vim.notify("Not a recognized header/source file.", vim.log.levels.WARN)
        return
    end

    for _, file in ipairs(target_files) do
        if vim.fn.filereadable(file) == 1 then
            vim.cmd('edit ' .. vim.fn.fnameescape(file))
            return
        end
    end

    vim.notify("Corresponding file not found.", vim.log.levels.INFO)
end

-- Function to switch files and set search
local function switch_and_search()
    local mode = vim.fn.mode()
    if mode == 'v' or mode == 'V' or mode == '\22' then
        vim.cmd('noau normal! gv"zy')
        vim.fn.setreg('/', vim.fn.getreg('z'))
    else
        vim.fn.setreg('/', vim.fn.expand('<cword>'))
    end

    switch_source_header()

    -- Optional: Jump to the first search match
    -- vim.cmd('normal! ggnzt')
end

-- Key mappings for switching files

-- F4: Switch between header and source
vim.api.nvim_set_keymap('n', '<F4>', '<cmd>lua switch_source_header()<CR>', { noremap = true, silent = true })
vim.api.nvim_set_keymap('i', '<F4>', '<Esc><cmd>lua switch_source_header()<CR>', { noremap = true, silent = true })
vim.api.nvim_set_keymap('v', '<F4>', '<Esc><cmd>lua switch_source_header()<CR>', { noremap = true, silent = true })

-- Shift+F4: Switch and search
vim.api.nvim_set_keymap('n', '<S-F4>', '<cmd>lua switch_and_search()<CR>', { noremap = true, silent = true })
vim.api.nvim_set_keymap('i', '<S-F4>', '<Esc><cmd>lua switch_and_search()<CR>', { noremap = true, silent = true })
vim.api.nvim_set_keymap('v', '<S-F4>', '<Esc><cmd>lua switch_and_search()<CR>', { noremap = true, silent = true })
