// DLL_C.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DLL_C.h"

#define WYSOKOSC 1440 //1080
#define SZEROKOSC 1920 //1920

/*struct zespolona {
	float re;
	float im;
};*/

void fraktalJulii(float* tablica, unsigned char* tablicaPixeli, int index, int offsetPikseli)
{
	float maxRe = 1.6;
	float minRe = -1.6;
	float maxIm = 1.2;
	float minIm = -1.2;

	float wspRe = (maxRe - minRe) / SZEROKOSC;
	float wspIm = (maxIm - minIm) / WYSOKOSC;

	int wiersze = index / SZEROKOSC;

	int offsetwierszy = offsetPikseli / SZEROKOSC;

	unsigned char iteracja;
	float re;
	float im;
	float modul2;
	float starere;
	float stareim;

	for (int i = wiersze; i < wiersze + offsetwierszy; i++)
	{
		for (int j = 0; j < SZEROKOSC; j++) //po pikselach
		{
			//petla ciagu
			iteracja = 0;
			re = minRe + j*wspRe;
			im = minIm + i*wspIm;
			//float modul2 = 0;
			do {
				//kwadrat
				starere = re;
				//stareim = im;
				re = re*re - im*im;
				im = 2 * starere*im;
				re += tablica[0];
				im += tablica[1];
				iteracja++;
				modul2 = re*re + im*im;
			} while (modul2 < 4 && iteracja < 255);
			//std::cout << "Piksel " << i << " " << j << "wyliczony" << std::endl;
			tablicaPixeli[(i - wiersze)*SZEROKOSC + j] = iteracja; //jeden kolor
		}
	}
}

