.data
suma dd 0
zero REAL4 0.0
minRe REAL4 -1.6
maxRe REAL4 1.6
minIm REAL4 -1.2
maxIm REAL4 1.2
SZEROKOSC dd 1920
WYSOKOSC dd 1440
wspRe REAL4 0.0
wspIm REAL4 0.0
wiersze dd 0
offsetwierszy dd 0
;zmienne
iteracja db ?
re REAL4 ?
im REAL4 ?
modul2 REAL4 ?
starere REAL4 ?
.code
;funkcja asemblerowa
FraktalJulii proc
;obliczenie wsp im i re
fld maxRe
fsub minRe
fidiv SZEROKOSC
fstp wspRe
fld maxIm
fsub minIm
fidiv WYSOKOSC
fstp wspIm
; za砤dowanie adresu tablicy z floatami re im
mov RSI, RCX
movss xmm4, REAL4 PTR[RSI]
shufps xmm4, xmm4, 00000000b
add RSI, 4
movss xmm5, REAL4 PTR[RSI]
shufps xmm5, xmm5, 00000000b
mov RSI, RDX
;obliczenie wierszy
mov EBX, SZEROKOSC
mov EAX, R8d
cdq
div EBX
mov wiersze, EAX
mov EBX, SZEROKOSC
mov EAX, R9d
cdq
div EBX
mov offsetwierszy, EAX
mov EAX, offsetwierszy
add EAX, wiersze
mov suma, EAX
; licznik i z C do EDX (rzadziej zmieniany) - j z C do ECX
mov EDX, wiersze ; i

mov ECX, 0 ; j
mov iteracja, 0

; re = minRe + j*wspRe - xmm6 im = minIm + i*wspIm - xmm7
el0:
pinsrd xmm6, ECX, 00000000b
inc ECX
cmp ECX, SZEROKOSC
jne el1
xor ECX, ECX
inc EDX
cmp EDX, suma ; mo縠 nie pykn规
je koniec
el1:
pinsrd xmm7, EDX, 00000000b
mov EAX, EDX
sub EAX, wiersze
imul EAX, SZEROKOSC
add EAX, ECX
pinsrd xmm0, EAX, 00000000b
pinsrd xmm6, ECX, 00000001b
inc ECX
cmp ECX, SZEROKOSC
jne el2
xor ECX, ECX
inc EDX
cmp EDX, suma ; mo縠 nie pykn规
je koniec
el2:
pinsrd xmm7, EDX, 00000001b
mov EAX, EDX
sub EAX, wiersze
imul EAX, SZEROKOSC
add EAX, ECX
pinsrd xmm0, EAX, 00000001b
pinsrd xmm6, ECX, 00000010b
inc ECX
cmp ECX, SZEROKOSC
jne el3
xor ECX, ECX
inc EDX
cmp EDX, suma ; mo縠 nie pykn规
je koniec
el3:
pinsrd xmm7, EDX, 00000010b
mov EAX, EDX
sub EAX, wiersze
imul EAX, SZEROKOSC
add EAX, ECX
pinsrd xmm0, EAX, 00000010b
pinsrd xmm6, ECX, 00000011b
inc ECX
cmp ECX, SZEROKOSC
jne petla
xor ECX, ECX
inc EDX
cmp EDX, suma ; mo縠 nie pykn规
je koniec

petla:
pinsrd xmm7, EDX, 00000011b
mov EAX, EDX
sub EAX, wiersze
imul EAX, SZEROKOSC
add EAX, ECX
pinsrd xmm0, EAX, 00000011b
cvtdq2ps xmm6, xmm6
cvtdq2ps xmm7, xmm7

movss xmm2, wspRe
shufps xmm2, xmm2, 00000000b
mulps xmm6, xmm2
movss xmm2, minRe
shufps xmm2, xmm2, 00000000b
addps xmm6, xmm2

movss xmm2, wspIm
shufps xmm2, xmm2, 00000000b
mulps xmm7, xmm2
movss xmm2, minIm
shufps xmm2, xmm2, 00000000b
addps xmm7, xmm2

xorps xmm1, xmm1



petlado:
movups xmm2, xmm6
movups xmm3, xmm7

mulps xmm2, xmm2
mulps xmm3, xmm3
subps xmm2, xmm3

mov R10D, 2
cvtsi2ss xmm8, R10D
shufps xmm8, xmm8, 00000000b ;zmieniono xmm0 na xmm8

mulps xmm6, xmm8
mulps xmm6, xmm7 ;2*im*re
addps xmm6, xmm5 ; +im
addps xmm2, xmm4 ; +re

movups xmm7, xmm6
movups xmm6, xmm2

;zwi阫szanie licznika iteracji

mov R10D, 1
movd xmm0, R10d
shufps xmm0, xmm0, 00000000b
addps xmm1, xmm0

;por體nanie z 255
mov R10D, 254
movd xmm0, R10D
shufps xmm0, xmm0, 00000000b

pcmpgtd xmm1, xmm0
movmskps R10D, xmm1 ; je縠li zero to lecim dalej

sprawdz0:
mov R11D, 0001b
and R11D, R10D
cmp R11D, 0
je sprawdz1

cvtsi2ss xmm2, ECX
mulss xmm2, wspRe
addss xmm2, minRe
cvtsi2ss xmm3, EDX
mulss xmm3, wspIm
addss xmm3, minIm
insertps xmm6, xmm2, 00000000b
insertps xmm7, xmm3, 00000000b
extractps EAX, xmm0, 00000000b
extractps R14d, xmm1, 00000000b
mov R15, RSI
add RSI, RAX
mov BYTE PTR [RSI], R14b
mov RSI, R15
mov EAX, EDX
sub EAX, wiersze
imul EAX, SZEROKOSC
add EAX, ECX
pinsrd xmm0, EAX, 00000000b
xor EAX, EAX
pinsrd xmm1, EAX, 00000000b
inc ECX
cmp ECX, SZEROKOSC
jne petlado
xor ECX, ECX
inc EDX
cmp EDX, suma ; mo縠 nie pykn规
je koniec


sprawdz1:
mov R11D, 0010b
and R11D, R10D
cmp R11D, 0
je sprawdz2

cvtsi2ss xmm2, ECX
mulss xmm2, wspRe
addss xmm2, minRe
cvtsi2ss xmm3, EDX
mulss xmm3, wspIm
addss xmm3, minIm
insertps xmm6, xmm2, 00010000b
insertps xmm7, xmm3, 00010000b
extractps EAX, xmm0, 00000001b
extractps R14d, xmm1, 00000001b
mov R15, RSI
add RSI, RAX
mov BYTE PTR [RSI], R14b
mov RSI, R15
mov EAX, EDX
sub EAX, wiersze
imul EAX, SZEROKOSC
add EAX, ECX
pinsrd xmm0, EAX, 00000001b
xor EAX, EAX
pinsrd xmm1, EAX, 00000001b
inc ECX
cmp ECX, SZEROKOSC
jne petlado
xor ECX, ECX
inc EDX
cmp EDX, suma ; mo縠 nie pykn规
je koniec

sprawdz2:
mov R11D, 0100b
and R11D, R10D
cmp R11D, 0
je sprawdz3

cvtsi2ss xmm2, ECX
mulss xmm2, wspRe
addss xmm2, minRe
cvtsi2ss xmm3, EDX
mulss xmm3, wspIm
addss xmm3, minIm
insertps xmm6, xmm2, 00100000b
insertps xmm7, xmm3, 00100000b
extractps EAX, xmm0, 00000010b
extractps R14d, xmm1, 00000010b
mov R15, RSI
add RSI, RAX
mov BYTE PTR [RSI], R14b
mov RSI, R15
mov EAX, EDX
sub EAX, wiersze
imul EAX, SZEROKOSC
add EAX, ECX
pinsrd xmm0, EAX, 00000010b
xor EAX, EAX
pinsrd xmm1, EAX, 00000010b
inc ECX
cmp ECX, SZEROKOSC
jne petlado
xor ECX, ECX
inc EDX
cmp EDX, suma ; mo縠 nie pykn规
je koniec

sprawdz3:
mov R11D, 1000b
and R11D, R10D
cmp R11D, 0
je modul

cvtsi2ss xmm2, ECX
mulss xmm2, wspRe
addss xmm2, minRe
cvtsi2ss xmm3, EDX
mulss xmm3, wspIm
addss xmm3, minIm
insertps xmm6, xmm2, 00110000b
insertps xmm7, xmm3, 00110000b
extractps EAX, xmm0, 00000011b
extractps R14d, xmm1, 00000011b
mov R15, RSI
add RSI, RAX
mov BYTE PTR [RSI], R14b
mov RSI, R15
mov EAX, EDX
sub EAX, wiersze
imul EAX, SZEROKOSC
add EAX, ECX
pinsrd xmm0, EAX, 00000011b
xor EAX, EAX
pinsrd xmm1, EAX, 00000011b
inc ECX
cmp ECX, SZEROKOSC
jne petlado
xor ECX, ECX
inc EDX
cmp EDX, suma ; mo縠 nie pykn规
je koniec

;sprawdzanie modul2
modul:
movups xmm2, xmm6
movups xmm3, xmm7
mulps xmm2, xmm2
mulps xmm3, xmm3
addps xmm2, xmm3

mov R13d, 4
cvtsi2ss xmm3, R13d
shufps xmm3, xmm3, 00000000b
cmpps xmm3, xmm2, 2
movmskps R10D, xmm3

; do optymalizacji

sprawdz01:
mov R11D, 0001b
and R11D, R10D
cmp R11D, 0
je sprawdz11

cvtsi2ss xmm2, ECX
mulss xmm2, wspRe
addss xmm2, minRe
cvtsi2ss xmm3, EDX
mulss xmm3, wspIm
addss xmm3, minIm
insertps xmm6, xmm2, 00000000b
insertps xmm7, xmm3, 00000000b
extractps EAX, xmm0, 00000000b
extractps R14d, xmm1, 00000000b
mov R15, RSI
add RSI, RAX
mov BYTE PTR [RSI], R14b
mov RSI, R15
mov EAX, EDX
sub EAX, wiersze
imul EAX, SZEROKOSC
add EAX, ECX
pinsrd xmm0, EAX, 00000000b
xor EAX, EAX
pinsrd xmm1, EAX, 00000000b
inc ECX
cmp ECX, SZEROKOSC
jne petlado
xor ECX, ECX
inc EDX
cmp EDX, suma ; mo縠 nie pykn规
je koniec


sprawdz11:
mov R11D, 0010b
and R11D, R10D
cmp R11D, 0
je sprawdz21

cvtsi2ss xmm2, ECX
mulss xmm2, wspRe
addss xmm2, minRe
cvtsi2ss xmm3, EDX
mulss xmm3, wspIm
addss xmm3, minIm
insertps xmm6, xmm2, 00010000b
insertps xmm7, xmm3, 00010000b
extractps EAX, xmm0, 00000001b
extractps R14d, xmm1, 00000001b
mov R15, RSI
add RSI, RAX
mov BYTE PTR [RSI], R14b
mov RSI, R15
mov EAX, EDX
sub EAX, wiersze
imul EAX, SZEROKOSC
add EAX, ECX
pinsrd xmm0, EAX, 00000001b
xor EAX, EAX
pinsrd xmm1, EAX, 00000001b
inc ECX
cmp ECX, SZEROKOSC
jne petlado
xor ECX, ECX
inc EDX
cmp EDX, suma ; mo縠 nie pykn规
je koniec

sprawdz21:
mov R11D, 0100b
and R11D, R10D
cmp R11D, 0
je sprawdz31

cvtsi2ss xmm2, ECX
mulss xmm2, wspRe
addss xmm2, minRe
cvtsi2ss xmm3, EDX
mulss xmm3, wspIm
addss xmm3, minIm
insertps xmm6, xmm2, 00100000b
insertps xmm7, xmm3, 00100000b
extractps EAX, xmm0, 00000010b
extractps R14d, xmm1, 00000010b
mov R15, RSI
add RSI, RAX
mov BYTE PTR [RSI], R14b
mov RSI, R15
mov EAX, EDX
sub EAX, wiersze
imul EAX, SZEROKOSC
add EAX, ECX
pinsrd xmm0, EAX, 00000010b
xor EAX, EAX
pinsrd xmm1, EAX, 00000010b
inc ECX
cmp ECX, SZEROKOSC
jne petlado
xor ECX, ECX
inc EDX
cmp EDX, suma ; mo縠 nie pykn规
je koniec

sprawdz31:
mov R11D, 1000b
and R11D, R10D
cmp R11D, 0
je petlado

cvtsi2ss xmm2, ECX
mulss xmm2, wspRe
addss xmm2, minRe
cvtsi2ss xmm3, EDX
mulss xmm3, wspIm
addss xmm3, minIm
insertps xmm6, xmm2, 00110000b
insertps xmm7, xmm3, 00110000b
extractps EAX, xmm0, 00000011b
extractps R14d, xmm1, 00000011b
mov R15, RSI
add RSI, RAX
mov BYTE PTR [RSI], R14b
mov RSI, R15
mov EAX, EDX
sub EAX, wiersze
imul EAX, SZEROKOSC
add EAX, ECX
pinsrd xmm0, EAX, 00000011b
xor EAX, EAX
pinsrd xmm1, EAX, 00000011b
inc ECX
cmp ECX, SZEROKOSC
jne petlado
xor ECX, ECX
inc EDX
cmp EDX, suma ; mo縠 nie pykn规
je koniec

jmp petlado

koniec:

ret
FraktalJulii endp
end