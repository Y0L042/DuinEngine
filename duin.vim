let SessionLoad = 1
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
silent only
silent tabonly
cd C:/Projects/CPP_Projects/Duin
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
let s:shortmess_save = &shortmess
if &shortmess =~ 'A'
  set shortmess=aoOA
else
  set shortmess=aoO
endif
badd +11 Duin/src/Duin/Physics/CharacterBody.h
badd +6 Duin/src/Duin/Physics/impl/physx/PhysX_Includes.h
badd +11 Duin/src/Duin/Physics/impl/physx/PhysX_PhysicsServer.cpp
badd +16 Duin/src/Duin/Physics/PhysicsServer.h
badd +28 Duin/src/Duin/Physics/PhysicsServer.cpp
badd +9 Duin/src/Duin/Physics/impl/physx/PhysX_PhysicsBody.h
badd +3 Duin/src/Duin/Physics/PhysicsBody.h
badd +8 Duin/src/Duin/Physics/impl/physx/PhysX_CollisionObject.h
badd +24 Duin/src/Duin/Physics/CollisionObject.h
badd +1 Duin/src/Duin/Physics/impl/physx/PhysX_CollisionObject.cpp
badd +3 Duin/src/Duin/Physics/impl/physx/PhysX_PhysicsBody.cpp
badd +1 Duin/src/Duin/Physics/impl/physx/PhysX_CharacterBody.h
badd +7 Duin/src/Duin/Physics/impl/physx/PhysX_CharacterBody.cpp
badd +125 Duin/src/Duin/Physics/PhysicsCharacterBody3D.h
badd +162 Duin/src/Duin/Physics/PhysicsCharacterBody3D.cpp
badd +15 Duin/src/Duin/Physics/impl/physx/PhysX_ControllerHitReport.h
badd +42 Duin/src/Duin/Physics/impl/physx/PhysX_ControllerHitReport.cpp
badd +85 ExampleProjects/DuinFPS/src/GameStates/StateGameLoop.cpp
badd +14 Duin/src/Duin.h
badd +29 Duin/src/Duin/Physics/impl/physx/PhysX_PhysicsServer.h
badd +1 Duin/src/Duin/Physics/CharacterBody.cpp
badd +396 Duin/src/Duin/ECS/ECSManager.cpp
badd +81 Duin/src/Duin/ECS/ECSManager.h
badd +1 Duin/src/Duin/Physics/PhysicsIncludes.h
badd +1 Duin/src/Duin/Physics/PhysicsMaterial.h
badd +21 Duin/src/Duin/Physics/CollisionObject.cpp
badd +15 Duin/src/Duin/Physics/StaticBody.h
badd +28 Duin/src/Duin/Physics/StaticBody.cpp
badd +3 Duin/src/Duin/Physics/impl/physx/PhysX_StaticBody.h
badd +1 Duin/src/Duin/Physics/impl/physx/PhysX_StaticBody.cpp
badd +2 ExampleProjects/DuinFPS/src/PlayerStates/PlayerStateInAir.h
badd +1 ExampleProjects/DuinFPS/src/PlayerStates/PlayerStateInAirIdle.cpp
badd +56 ExampleProjects/DuinFPS/src/PlayerStates/PlayerStateInAirStrafe.cpp
badd +58 ExampleProjects/DuinFPS/src/PlayerStates/PlayerStateOnGround.cpp
badd +735 Duin/vendor/PhysX/physx/include/PxPhysics.h
badd +5 Duin/src/Duin/Physics/CollisionMeta.h
badd +20 Duin/src/Duin/Physics/CollisionGeometry.h
badd +0 C:/Projects/CPP_Projects/Duin
argglobal
%argdel
$argadd C:/Projects/CPP_Projects/Duin
edit Duin/src/Duin/Physics/impl/physx/PhysX_StaticBody.cpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe 'vert 1resize ' . ((&columns * 104 + 104) / 209)
exe 'vert 2resize ' . ((&columns * 104 + 104) / 209)
argglobal
balt Duin/src/Duin/Physics/impl/physx/PhysX_StaticBody.h
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 7 - ((6 * winheight(0) + 25) / 50)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 7
normal! 0
lcd C:/Projects/CPP_Projects/Duin
wincmd w
argglobal
if bufexists(fnamemodify("C:/Projects/CPP_Projects/Duin/Duin/src/Duin/Physics/CollisionGeometry.h", ":p")) | buffer C:/Projects/CPP_Projects/Duin/Duin/src/Duin/Physics/CollisionGeometry.h | else | edit C:/Projects/CPP_Projects/Duin/Duin/src/Duin/Physics/CollisionGeometry.h | endif
if &buftype ==# 'terminal'
  silent file C:/Projects/CPP_Projects/Duin/Duin/src/Duin/Physics/CollisionGeometry.h
endif
balt C:/Projects/CPP_Projects/Duin/Duin/src/Duin/Physics/impl/physx/PhysX_StaticBody.cpp
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 20 - ((19 * winheight(0) + 25) / 50)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 20
normal! 0
lcd C:/Projects/CPP_Projects/Duin
wincmd w
2wincmd w
exe 'vert 1resize ' . ((&columns * 104 + 104) / 209)
exe 'vert 2resize ' . ((&columns * 104 + 104) / 209)
tabnext 1
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0 && getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20
let &shortmess = s:shortmess_save
let &winminheight = s:save_winminheight
let &winminwidth = s:save_winminwidth
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
set hlsearch
nohlsearch
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
