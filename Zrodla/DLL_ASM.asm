.data
zero REAL4 0.0 ;sta³e jedynie odczytywane (potrzebne jako miejsca w pamiêci)
dwa REAL4 2.0
cztery REAL4 4.0
minRe REAL4 -1.6
maxRe REAL4 1.6
minIm REAL4 -1.2
maxIm REAL4 1.2
WYSOKOSC dd 14400
SZEROKOSC dd 19200
jeden dd 1
dwapiecpiec dd 255
.code
wstawpoczatek MACRO maska:REQ, etykieta:REQ ;makro wstawiaj¹ce na pocz¹tku liczby do rej. SIMD na pozycjê okreœlon¹ mask¹, etykieta w celu umo¿liwienia przeskoku inkrementacji wierszy
	shl r9b, 1 ;przesuñ w lewo i dodaj bo wstawiamy od najm³odszej
	inc r9b ;r9 - na bitach 0:3 przechowujemy informacjê czy jeszcze coœ jest liczone na tej pozycji (1 je¿eli tak na odpowiednim bicie)
	pinsrd xmm6, ECX, maska ; numeru wiersza (EDX) i kolumny - pixela w wierszu (ECX) do odpowiednich rejestrów
	pinsrd xmm7, EDX, maska ; re = minRe + j*wspRe - xmm6 im = minIm + i*wspIm - xmm7
	mov EAX, EDX
	sub EAX, r12d ;r12d - wiersz od ktoreko zaczynaliœmy
	imul EAX, SZEROKOSC ;imul bo w EDX mamy ju¿ licznik wierszy (nie zerujemy)
	add EAX, ECX ;obliczenie indeksu w tablicy pikseli i przechowanie w xmm0
	pinsrd xmm0, EAX, maska ;wstawienie do xmm0 na pozycjê okreœlon¹ mask¹
	inc ECX
	cmp ECX, SZEROKOSC ;nastêpny piksel w wierszu
	jne etykieta ;przeskocz inkrementacje wiersza
	xor ECX, ECX ;je¿eli przeszlibyœmy na piksel tu¿ poza wierszem
	inc EDX ;inkrementuj wiersz
	cmp EDX, r8d ; je¿eli wzieliœmy z ostatniego wiersza (koniec pikseli) - przeskocz od razu do pêtli (nie ma wiêcej danych do wstawienia)
	je petla
	etykieta:
ENDM
wymiana MACRO maska1:REQ, nastetykieta:REQ, kontrolnaet:REQ ;makro do wymiany danych na danej pozycji (maska1), nastetyketa gdzie przechodzimy po zakoñczeniu obliczeñ, kontrolna - przeskok na wypadek dalszych danych
	maskaprzes = 16 * maska1 ;druga maska przesuniêta zawsze o 4 pozycje w lewo - insertps czyta z bitów 5:4, extractps 1:0
	xor RAX, RAX
	inc EAX
	shl EAX, maska1
	and EAX, r9d ;przygotowanie w EAX maski do odczytu bitu z r9 odpowiadaj¹cego ¿¹danej pozycji
	cmp EAX, 0
	je nastetykieta ;0 na danym bicie mówi, ¿e wartoœæ na tej pozycji nie jest ju¿ potrzebna
	cvtsi2ss xmm2, ECX ;przerzucenie licznika (pojedynczego) kolumny i obliczenie wartoœci pozycji pixela
	mulss xmm2, wspRe
	addss xmm2, minRe
	cvtsi2ss xmm3, EDX ;przerzucenie licznika (pojedynczego) wiersza i obliczenie wartoœci pozycji pixela
	mulss xmm3, wspIm
	addss xmm3, minIm 
	insertps xmm6, xmm2, maskaprzes ;wpis wartoœci re i im na odpowiedni¹ pozycje
	insertps xmm7, xmm3, maskaprzes ;xmm6 - re xmm7 - im
	extractps EAX, xmm0, maska1 ;wyci¹gniêcie indeksu elementu wymienianego
	extractps R14d, xmm1, maska1 ;wyci¹gniêcie licznika elementu wymienianego
	mov R15, RSI ;przechowanie RSI
	add RSI, RAX ;tablica pixeli - przejœcie na odpowiedni indeks
	mov BYTE PTR [RSI], R14b ;zapis do tablicy
	mov RSI, R15 ;przywrócenie RSI
	cmp EDX, r8d ;sprawdzenie czy wyszliœmy poza sumê wierszy
	jb kontrolnaet
	xor RAX, RAX ;jezeli tak zmieniamy odpowiedni bit maski
	inc EAX
	shl EAX, maska1
	not EAX ;same jedynki oprócz tego bitu który zerujemy
	and r9d, EAX ;wyzerowanie
	jmp nastetykieta ;idŸ dalej
	kontrolnaet:
	mov EAX, EDX ;obliczenie indeksu umieszczanego elementu do xmm0
	sub EAX, r12d
	imul EAX, SZEROKOSC
	add EAX, ECX ; (i- wiersze)*SZEROKOSC + j z C
	pinsrd xmm0, EAX, maska1
	xor EAX, EAX
	pinsrd xmm1, EAX, maska1 ;wpis zerowego licznika iteracji
	inc ECX
	cmp ECX, SZEROKOSC ;inkrementacja licznika pikseli w wierszu
	jne nastetykieta
	xor ECX, ECX
	inc EDX
	cmp EDX, r8d ; inkrementacja licznika wierszy
	je nastetykieta
ENDM
FraktalJulii proc ;procedura asemblerowa wpisuj¹ca dane do tablicy w 2 parametrze (wskaŸnik na unsigned char w RDX), otrzymuje 2el tablicê floatów z parametrem c(RCX), indeks od którego zaczynamy i iloœæ danych (R8, R9)
LOCAL wspRe:REAL4 ;lokalne obliczenie wspolczynników okreœlaj¹cych o ile dalej bêdzie nastêpny punkt na danej osi
LOCAL wspIm:REAL4
fld maxRe ;obliczenie wsp im i re
fsub minRe
fidiv SZEROKOSC
fstp wspRe ;wspRe = (maxRe - minRe) / SZEROKOSC
fld maxIm
fsub minIm
fidiv WYSOKOSC
fstp wspIm ;wspIm = (maxIm - minIm) / WYSOKOSC
mov RSI, RCX ; za³adowanie adresu tablicy z floatami c.re i c.im
movss xmm4, REAL4 PTR[RSI] ;c.re -> xmm4
shufps xmm4, xmm4, 00000000b ;c.re na wszystkich 4 pozycjach xmm4
add RSI, 4
movss xmm5, REAL4 PTR[RSI] ;c.im -> xmm5
shufps xmm5, xmm5, 00000000b ;c.im na wszystkich 4 pozycjach xmm5
mov RSI, RDX ;obliczenie wierszy
mov EBX, SZEROKOSC ;operacje na EBX bo w zwi¹zku z formatem bmp jeden wymiar mo¿e byæ co najwy¿ej int
mov EAX, R8d ;index i offset te¿ w zakresie int
cdq ;zerowanie EDX
div EBX
mov r12d, EAX ;od ktorego wiersza zaczynamy, rzadko potrzebna wiêc wyrzucona dalej
mov EBX, SZEROKOSC
mov EAX, R9d
cdq ;zerowanie EDX
div EBX
mov r9d, EAX ;offsetwierszy - ile wierszy bêdziemy przerabiaæ
add EAX, r12d
mov r8d, EAX ;suma wierszy i offsetu wierszy - warunek koñca obliczeñ, offset ju¿ niepotrzebny
mov EDX, r12d ; licznik i z C do EDX (rzadziej zmieniany) - j z C do ECX
mov ECX, 0 ;zarówno i jak i j mog¹ byæ maksymalnie intem
xor r9, r9 ;przygotowanie maski bitowej
wstawpoczatek 0, el0 ;wstawienie pierwszych elementów do SIMD
wstawpoczatek 1, el1
wstawpoczatek 2, el2
wstawpoczatek 3, el3
petla:
cvtdq2ps xmm6, xmm6 ;przerobienie na float wszystkich 4 w rejestrze
cvtdq2ps xmm7, xmm7
movss xmm2, wspRe 
shufps xmm2, xmm2, 00000000b ;skopiowanie z pierwszej na wszystkie pozycje
mulps xmm6, xmm2
movss xmm2, minRe
shufps xmm2, xmm2, 00000000b ;skopiowanie z pierwszej na wszystkie pozycje
addps xmm6, xmm2 ;re = j*wspRe + minRe
movss xmm2, wspIm ;im = minIm + i*wspIm
shufps xmm2, xmm2, 00000000b ;skopiowanie z pierwszej na wszystkie pozycje
mulps xmm7, xmm2
movss xmm2, minIm
shufps xmm2, xmm2, 00000000b ;skopiowanie z pierwszej na wszystkie pozycje
addps xmm7, xmm2 ;im = minIm + i*wspIm
xorps xmm1, xmm1 ;przygotowanie (wyzerowanie licznika)
petlado:
movups xmm2, xmm6 ;skopiowanie tablicy re i im do operacji na nich
movups xmm3, xmm7
mulps xmm6, xmm6
mulps xmm3, xmm3
subps xmm6, xmm3 ;nowere = re^2 - im^2
movss xmm3, dwa  ;wstawienie 2 i kopiowanie na wszystkie pozycje
shufps xmm3, xmm3, 00000000b 
mulps xmm7, xmm3
mulps xmm7, xmm2 ;2*im*re
addps xmm7, xmm5 ; +im
addps xmm6, xmm4 ; +re
movd xmm3, jeden ;zwiêkszanie licznika iteracji
shufps xmm3, xmm3, 00000000b
paddd xmm1, xmm3 ; dodawanie ca³kowitoliczbowe do licznika iteracji
movd xmm3, dwapiecpiec ;mo¿na zrobiæ 4 razy
shufps xmm3, xmm3, 00000000b
pcmpgtd xmm3, xmm1 ;porownanie ca³kowitoliczbowe czy wiêksze
movmskps R10D, xmm3 ; je¿eli jeden to nie wymieniamy
sprawdz0:		;sprawdzanie czy przekroczono max iteracje
mov R11D, 0001b
and R11D, R10D
jnz sprawdz1 ;je¿eli nie by³o zera na tej pozycji to przeskocz wymianê
wymiana 0, sprawdz1, kontrol0
sprawdz1:
mov R11D, 0010b
and R11D, R10D
jnz sprawdz2 ;je¿eli nie by³o zera na tej pozycji to przeskocz wymianê
wymiana 1, sprawdz2, kontrol1
sprawdz2:
mov R11D, 0100b
and R11D, R10D
jnz sprawdz3 ;je¿eli nie by³o zera na tej pozycji to przeskocz wymianê
wymiana 2, sprawdz3, kontrol2
sprawdz3:
mov R11D, 1000b
and R11D, R10D
jnz modul ;je¿eli nie by³o zera na tej pozycji to przeskocz wymianê
wymiana 3, modul, kontrol3
modul:				;sprawdzanie modul^2 < 4
movups xmm2, xmm6
movups xmm3, xmm7
mulps xmm2, xmm2
mulps xmm3, xmm3
addps xmm2, xmm3 ;re^2 +im^2
movss xmm3, cztery
shufps xmm3, xmm3, 00000000b ;przepisanie na wszystkie pozycje
cmpps xmm3, xmm2, 2 ; sprawdz 4<= modul, dla zera pomijamy wymiany
movmskps R10D, xmm3
sprawdz01: ; do optymalizacji
mov R11D, 0001b
and R11D, R10D
jz sprawdz11 ;je¿eli by³o zero na tej pozycji to przeskocz wymianê
wymiana 0, sprawdz11, kontrol01
sprawdz11:
mov R11D, 0010b
and R11D, R10D
jz sprawdz21 ;je¿eli by³o zero na tej pozycji to przeskocz wymianê
wymiana 1, sprawdz21, kontrol11
sprawdz21:
mov R11D, 0100b
and R11D, R10D
jz sprawdz31 ;je¿eli by³o zero na tej pozycji to przeskocz wymianê
wymiana 2, sprawdz31, kontrol21
sprawdz31:
mov R11D, 1000b
and R11D, R10D
jz czykoniec ;je¿eli by³o zero na tej pozycji to przeskocz wymianê
wymiana 3, czykoniec, kontrol31
czykoniec:
cmp r9b, 0 ;sprawdz czy mamy na czym operowaæ
je koniec ;jak na ¿adnej pozycji nie ma istotnej danej to koniec - jak s¹ to obliczamy dalej
jmp petlado
koniec:
ret
FraktalJulii endp
end