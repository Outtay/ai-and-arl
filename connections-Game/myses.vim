let SessionLoad = 1
let s:so_save = &so | let s:siso_save = &siso | set so=0 siso=0
let v:this_session=expand("<sfile>:p")
silent only
cd ~/Documents/git/github/ai-and-arl/connections-Game
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
set shortmess=aoO
badd +8 src/main.cpp
badd +1 term://.//23726:/bin/bash
badd +1 Board.h
badd +1 Board.cpp
badd +1 src/Board.h
badd +1 term://.//23733:/bin/bash
badd +0 src/Board.cpp
argglobal
silent! argdel *
$argadd src/main.cpp
set stal=2
edit src/main.cpp
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
wincmd t
set winminheight=1 winminwidth=1 winheight=1 winwidth=1
exe 'vert 1resize ' . ((&columns * 118 + 119) / 238)
exe 'vert 2resize ' . ((&columns * 119 + 119) / 238)
argglobal
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let s:l = 36 - ((20 * winheight(0) + 27) / 55)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
36
normal! 013|
wincmd w
argglobal
if bufexists('src/Board.h') | buffer src/Board.h | else | edit src/Board.h | endif
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let s:l = 67 - ((15 * winheight(0) + 27) / 55)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
67
normal! 047|
wincmd w
exe 'vert 1resize ' . ((&columns * 118 + 119) / 238)
exe 'vert 2resize ' . ((&columns * 119 + 119) / 238)
tabnew
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
wincmd t
set winminheight=1 winminwidth=1 winheight=1 winwidth=1
exe 'vert 1resize ' . ((&columns * 118 + 119) / 238)
exe 'vert 2resize ' . ((&columns * 119 + 119) / 238)
argglobal
if bufexists('term://.//23726:/bin/bash') | buffer term://.//23726:/bin/bash | else | edit term://.//23726:/bin/bash | endif
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 1904 - ((54 * winheight(0) + 27) / 55)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
1904
normal! 072|
wincmd w
argglobal
if bufexists('term://.//23733:/bin/bash') | buffer term://.//23733:/bin/bash | else | edit term://.//23733:/bin/bash | endif
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 25 - ((24 * winheight(0) + 27) / 55)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
25
normal! 0
wincmd w
exe 'vert 1resize ' . ((&columns * 118 + 119) / 238)
exe 'vert 2resize ' . ((&columns * 119 + 119) / 238)
tabedit src/Board.h
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
wincmd t
set winminheight=1 winminwidth=1 winheight=1 winwidth=1
exe 'vert 1resize ' . ((&columns * 118 + 119) / 238)
exe 'vert 2resize ' . ((&columns * 119 + 119) / 238)
argglobal
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let s:l = 85 - ((0 * winheight(0) + 27) / 55)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
85
normal! 09|
wincmd w
argglobal
if bufexists('src/Board.cpp') | buffer src/Board.cpp | else | edit src/Board.cpp | endif
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let s:l = 282 - ((44 * winheight(0) + 27) / 55)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
282
normal! 0
wincmd w
2wincmd w
exe 'vert 1resize ' . ((&columns * 118 + 119) / 238)
exe 'vert 2resize ' . ((&columns * 119 + 119) / 238)
tabnext 3
set stal=1
if exists('s:wipebuf') && getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20 winminheight=1 winminwidth=1 shortmess=filnxtToOc
let s:sx = expand("<sfile>:p:r")."x.vim"
if file_readable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &so = s:so_save | let &siso = s:siso_save
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
