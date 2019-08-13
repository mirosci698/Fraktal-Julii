// DLL_C.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DLL_C.h"

#define WYSOKOSC 14400 
#define SZEROKOSC 19200 

#define MAX_ITER 255

void fraktalJulii(float* tablica, unsigned char* tablicaPixeli, int index, int offsetPikseli)
{
	float maxRe = 1.6f; //zmienne okre�laj�ce wycinek p�aszczyzny
	float minRe = -1.6f; //x od -1.6 do 1.6
	float maxIm = 1.2f; //x od -1.2 do 1.2
	float minIm = -1.2f;

	float wspRe = (maxRe - minRe) / SZEROKOSC; //obliczenie wsp�czynnik�w okre�laj�cych o ile dalej b�dzie nast�pny punkt na danej osi
	float wspIm = (maxIm - minIm) / WYSOKOSC;

	int wiersze = index / SZEROKOSC; //sprawdzenie od kt�rego wiersza zaczynamy

	int offsetwierszy = offsetPikseli / SZEROKOSC; //obliczenie ile wierszy b�dziemy opracowywa�

	unsigned char iteracja; //deklaracja zmiennych potrzebnych w p�tli
	float re;
	float im;
	float modul2;
	float starere;

	for (int i = wiersze; i < wiersze + offsetwierszy; i++) //przej�cie po wierszach
		for (int j = 0; j < SZEROKOSC; j++) //i po pikselach w wierszu
		{
			iteracja = 0; //petla ciagu dla pojedynczego elementu
			re = minRe + j*wspRe; //obliczenie wsp�rz�dnych punktu na p�aszczy�nie zespolonej
			im = minIm + i*wspIm;
			do {
				starere = re;
				re = re*re - im*im; //kwadrat
				im = 2 * starere*im;
				re += tablica[0]; //dodanie c
				im += tablica[1]; //w efekcie z^2 +c
				iteracja++; //zwi�kszenie iteracji
				modul2 = re*re + im*im; //obliczenie modu�u
			} while (modul2 < 4 && iteracja < MAX_ITER); //sprawdzanie warunku: modu� musi by� mniejszy od 2, a iteracja od 255 �eby i�� dalej
			tablicaPixeli[(i - wiersze)*SZEROKOSC + j] = iteracja; //jeden kolor wi�c obliczamy jeden bajt
		}
}

