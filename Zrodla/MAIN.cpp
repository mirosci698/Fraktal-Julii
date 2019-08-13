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

#define WYSOKOSC 1080	//przyjmujemy rozdzielczo�� fHD i 24 bitow� g��bie kolor�w
#define SZEROKOSC 1920
#define BAJTY_KOLOROW 3
#define ILOSC_FLOAT 2

int main(int argc, char* argv[])
{
	std::vector<unsigned char> macierzPikseli(WYSOKOSC * SZEROKOSC * BAJTY_KOLOROW); //wszystkie piksele jako jedna tablica, wektor ze wzgl�du na du�y rozmiar - pomini�cie stosu
	std::string stalaC = "";
	int liczbaWatkow = -1;
	std::string tryb = "";	//sta�e na wczytywane warto�ci z warto�ciami kontrolnymi
	if (argc > 3)
		for (int i = 1; i < argc; i++)	//odczyt prze��cznik�w i warto�ci
		{
			std::string s = argv[i];
			if (s == "-l")	//sta�a zespolona c
				if (i + 1 < argc)
					stalaC = argv[i + 1];
			if (s == "-t")	//opcjonalna ilo�� w�tk�w na nast�pnym argumencie
				if (i + 1 < argc)
					liczbaWatkow = atoi(argv[i + 1]);
			if (s == "-c" || s == "-a")	//asm czy C
				tryb = s;
		}
	else
		std::cout << "Z�a liczba argument�w!" << std::endl;
	//Sprawdzanie ilo��i w�t�w w komputerze
	unsigned int iloscWatkowSprzetowych = std::thread::hardware_concurrency();
	//Sprawdzanie poprawno�ci danych i parsowanie
	if (tryb == "")
		std::cout << "Nie podano trybu!" << std::endl;
	if (liczbaWatkow == -1 || liczbaWatkow > iloscWatkowSprzetowych)
		liczbaWatkow = iloscWatkowSprzetowych;
	std::istringstream strumienWej('(' + stalaC + ')');
	std::complex<float> zespolona;
	strumienWej >> zespolona;
	if (strumienWej.fail())
		std::cout << "Nie podano sta�ej zespolonej!" << std::endl;
	//Przechowanie realis i imaginalis w 2elementowej tablicy float-�w - �atwiejsze przekazanie do funkcji
	float daneFloat[ILOSC_FLOAT];
	daneFloat[0] = zespolona.real();
	daneFloat[1] = zespolona.imag();
	//utworzenie pustej tablicy w�tk�w dla ich �atwiejszej obs�ugi
	std::vector<std::thread> watki;
	//offset dla podzielenia liczby pikseli kt�re b�d� obs�ugiwane w ka�dym z w�tkow
	int offset = WYSOKOSC * SZEROKOSC / liczbaWatkow;
	//tablica offset�w na wypadek braku podzielno�ci
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
	//uchwyt do biblioteki i �ci�gni�cie funkcji asemblerowej
	HINSTANCE dllHandle = NULL;
	dllHandle = LoadLibrary(L"DLL_ASM.dll");
	FraktalJulii procedura = (FraktalJulii)GetProcAddress(dllHandle, "FraktalJulii");
	//dzia�anie w zale�no�ci od poprawnej liczby w�tk�w i argument�w
	if (liczbaWatkow > -1 && argc > 3 && !strumienWej.fail())
		if (tryb == "-c")
		{
			for (int i = 0; i < liczbaWatkow; i++)
				watki.push_back(std::thread(fraktalJulii, daneFloat, &macierzPikseli[(i * offset + i* offsety[i]) * BAJTY_KOLOROW], (i * offset + i*offsety[i]) * BAJTY_KOLOROW, offset + offsety[i])); 
			//dodanie konkretnej ilo�ci w�tk�w i funkcji z arg. do nich
			for (int i = 0; i < liczbaWatkow; i++)
				watki[i].join(); //wywo�anie w�tk�w
		}
		else
			if (tryb == "-a")
			{
				for (int i = 0; i < liczbaWatkow; i++)
					watki.push_back(std::thread(procedura, daneFloat, &macierzPikseli[(i * offset + i* offsety[i]) * BAJTY_KOLOROW], (i * offset + i* offsety[i]) * BAJTY_KOLOROW, offset + offsety[i]));
				//dodanie konkretnej ilo�ci w�tk�w i funkcji z arg. do nich
				for (int i = 0; i < liczbaWatkow; i++)
					watki[i].join(); //wywo�anie w�tk�w
			}
	getchar();
    return 0;
}

