// MAIN.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MAIN.h"
#include <iostream>
#include "DLL_C.h"
#include <cstdlib>
#include <thread>
#include <complex>
#include <sstream>
#include <vector>

#define WYSOKOSC 1080	//przyjmujemy rozdzielczoœæ fHD i 24 bitow¹ g³êbie kolorów
#define SZEROKOSC 1920
#define BAJTY_KOLOROW 3
#define ILOSC_FLOAT 2

int main(int argc, char* argv[])
{
	std::vector<unsigned char> macierzPikseli(WYSOKOSC * SZEROKOSC * BAJTY_KOLOROW); //wszystkie piksele jako jedna tablica, wektor ze wzglêdu na du¿y rozmiar - pominiêcie stosu
	std::string stalaC = "";
	int liczbaWatkow = -1;
	std::string tryb = "";	//sta³e na wczytywane wartoœci z wartoœciami kontrolnymi
	if (argc > 3)
		for (int i = 1; i < argc; i++)	//odczyt prze³¹czników i wartoœci
		{
			std::string s = argv[i];
			if (s == "-l")	//sta³a zespolona c
				if (i + 1 < argc)
					stalaC = argv[i + 1];
			if (s == "-t")	//opcjonalna iloœæ w¹tków na nastêpnym argumencie
				if (i + 1 < argc)
					liczbaWatkow = atoi(argv[i + 1]);
			if (s == "-c" || s == "-a")	//asm czy C
				tryb = s;
		}
	else
		std::cout << "Z³a liczba argumentów!" << std::endl;
	//Sprawdzanie iloœæi w¹tów w komputerze
	unsigned int iloscWatkowSprzetowych = std::thread::hardware_concurrency();
	//Sprawdzanie poprawnoœci danych i parsowanie
	if (tryb == "")
		std::cout << "Nie podano trybu!" << std::endl;
	if (liczbaWatkow == -1 || liczbaWatkow > iloscWatkowSprzetowych)
		liczbaWatkow = iloscWatkowSprzetowych;
	std::istringstream strumienWej('(' + stalaC + ')');
	std::complex<float> zespolona;
	strumienWej >> zespolona;
	if (strumienWej.fail())
		std::cout << "Nie podano sta³ej zespolonej!" << std::endl;
	//Przechowanie realis i imaginalis w 2elementowej tablicy float-ów - ³atwiejsze przekazanie do funkcji
	float daneFloat[ILOSC_FLOAT];
	daneFloat[0] = zespolona.real();
	daneFloat[1] = zespolona.imag();
	//utworzenie pustej tablicy w¹tków dla ich ³atwiejszej obs³ugi
	std::vector<std::thread> watki;
	//offset dla podzielenia liczby pikseli które bêd¹ obs³ugiwane w ka¿dym z w¹tkow
	int offset = WYSOKOSC * SZEROKOSC / liczbaWatkow;
	//tablica offsetów na wypadek braku podzielnoœci
	int reszta = WYSOKOSC * SZEROKOSC - offset * liczbaWatkow;
	int * offsety = new int[liczbaWatkow];
	for (int i = 0; i < liczbaWatkow; i++)
		offsety[i] = 0;
	int i = 0;
	while (reszta > 0)
	{
		offsety[i] = 1;
		i++;
		reszta--;
	}
	//uchwyt do biblioteki i œci¹gniêcie funkcji asemblerowej
	HINSTANCE dllHandle = NULL;
	dllHandle = LoadLibrary(L"DLL_ASM.dll");
	FraktalJulii procedura = (FraktalJulii)GetProcAddress(dllHandle, "FraktalJulii");
	//dzia³anie w zale¿noœci od poprawnej liczby w¹tków i argumentów
	if (liczbaWatkow > -1 && argc > 3 && !strumienWej.fail())
		if (tryb == "-c")
		{
			for (int i = 0; i < liczbaWatkow; i++)
				watki.push_back(std::thread(fraktalJulii, daneFloat, &macierzPikseli[(i * offset + i* offsety[i]) * BAJTY_KOLOROW], (i * offset + i*offsety[i]) * BAJTY_KOLOROW, offset + offsety[i])); 
			//dodanie konkretnej iloœci w¹tków i funkcji z arg. do nich
			for (int i = 0; i < liczbaWatkow; i++)
				watki[i].join(); //wywo³anie w¹tków
		}
		else
			if (tryb == "-a")
			{
				for (int i = 0; i < liczbaWatkow; i++)
					watki.push_back(std::thread(procedura, daneFloat, &macierzPikseli[(i * offset + i* offsety[i]) * BAJTY_KOLOROW], (i * offset + i* offsety[i]) * BAJTY_KOLOROW, offset + offsety[i]));
				//dodanie konkretnej iloœci w¹tków i funkcji z arg. do nich
				for (int i = 0; i < liczbaWatkow; i++)
					watki[i].join(); //wywo³anie w¹tków
			}
	getchar();
    return 0;
}

