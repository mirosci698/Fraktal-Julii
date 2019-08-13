// DLL_C.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DLL_C.h"

#define WYSOKOSC 14400 
#define SZEROKOSC 19200 

#define MAX_ITER 255

void fraktalJulii(float* tablica, unsigned char* tablicaPixeli, int index, int offsetPikseli)
{
	float maxRe = 1.6f; //zmienne okreœlaj¹ce wycinek p³aszczyzny
	float minRe = -1.6f; //x od -1.6 do 1.6
	float maxIm = 1.2f; //x od -1.2 do 1.2
	float minIm = -1.2f;

	float wspRe = (maxRe - minRe) / SZEROKOSC; //obliczenie wspó³czynników okreœlaj¹cych o ile dalej bêdzie nastêpny punkt na danej osi
	float wspIm = (maxIm - minIm) / WYSOKOSC;

	int wiersze = index / SZEROKOSC; //sprawdzenie od którego wiersza zaczynamy

	int offsetwierszy = offsetPikseli / SZEROKOSC; //obliczenie ile wierszy bêdziemy opracowywaæ

	unsigned char iteracja; //deklaracja zmiennych potrzebnych w pêtli
	float re;
	float im;
	float modul2;
	float starere;

	for (int i = wiersze; i < wiersze + offsetwierszy; i++) //przejœcie po wierszach
		for (int j = 0; j < SZEROKOSC; j++) //i po pikselach w wierszu
		{
			iteracja = 0; //petla ciagu dla pojedynczego elementu
			re = minRe + j*wspRe; //obliczenie wspó³rzêdnych punktu na p³aszczyŸnie zespolonej
			im = minIm + i*wspIm;
			do {
				starere = re;
				re = re*re - im*im; //kwadrat
				im = 2 * starere*im;
				re += tablica[0]; //dodanie c
				im += tablica[1]; //w efekcie z^2 +c
				iteracja++; //zwiêkszenie iteracji
				modul2 = re*re + im*im; //obliczenie modu³u
			} while (modul2 < 4 && iteracja < MAX_ITER); //sprawdzanie warunku: modu³ musi byæ mniejszy od 2, a iteracja od 255 ¿eby iœæ dalej
			tablicaPixeli[(i - wiersze)*SZEROKOSC + j] = iteracja; //jeden kolor wiêc obliczamy jeden bajt
		}
}

