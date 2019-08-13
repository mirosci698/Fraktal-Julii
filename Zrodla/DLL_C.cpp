// DLL_C.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DLL_C.h"
#include <vector>
#include <iostream>
#include <ctime>

#define WYSOKOSC 1440 //1080
#define SZEROKOSC 1920 //1920

struct zespolona {
	float re;
	float im;
};

void fraktalJulii(float* tablica, unsigned char* tablicaPixeli, int index, int offsetPikseli, clock_t* tablicaCzasow)
{
	std::cout << "fraktal " << std::endl;

	clock_t start;
	start = clock();

	float maxRe = 1.6;
	float minRe = -1.6;
	float maxIm = 1.2;
	float minIm = -1.2;

	float wspRe = (maxRe - minRe) / SZEROKOSC;
	float wspIm = (maxIm - minIm) / WYSOKOSC;

	int wiersze = index / SZEROKOSC;

	int offsetwierszy = offsetPikseli / SZEROKOSC;

	for (int i = wiersze; i < wiersze + offsetwierszy; i++)
		for (int j = 0; j < SZEROKOSC; j++) //po pikselach
		{
			//petla ciagu
			float punktRe = minRe + j*wspRe;
			float punktIm = minIm + i*wspIm;
			unsigned char iteracja = 0;
			zespolona z;
			z.re = punktRe;
			z.im = punktIm;
			float modul2 = 0;
			do {
				//kwadrat
				float starere = z.re;
				float stareim = z.im;
				z.re = starere*starere - stareim*stareim;
				z.im = 2 * starere*stareim;
				z.re += tablica[0];
				z.im += tablica[1];
				iteracja++;
				modul2 = z.re*z.re + z.im*z.im;
			} while (modul2 < 4 && iteracja < 255);
			//std::cout << "Piksel " << i << " " << j << "wyliczony" << std::endl;
			tablicaPixeli[(i - wiersze)*SZEROKOSC + j] = iteracja; //jeden kolor
		}
	clock_t koniec;
	koniec = clock();
	tablicaCzasow[0] = start;
	tablicaCzasow[1] = koniec;
}

