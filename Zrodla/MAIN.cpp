// MAIN.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MAIN.h"
#include <iostream> //wypis informacji w konsoli
#include "DLL_C.h"
#include <thread> //w¹tki
#include <complex> //odpowiednie wczytanie wartoœci zespolonej
#include <sstream>  //odpowiednie wczytanie wartoœci zespolonej
#include <vector> //tworzymy vector ¿eby nie tworzyæ na stosie
#include <fstream> //obs³uga plików
#include <ctime> //pomiar czasu

#define WYSOKOSC 14400//przyjmujemy rozdzielczoœæ i 24 bitow¹ g³êbie kolorów
#define SZEROKOSC 19200
#define BAJTY_KOLOROW 4
#define ILOSC_FLOAT 2

int main(int argc, char* argv[])
{
	std::vector<unsigned char> piksele(WYSOKOSC * SZEROKOSC);
	std::vector<unsigned char> tablicaWyjsciowa(WYSOKOSC * SZEROKOSC * BAJTY_KOLOROW); //wszystkie piksele jako jedna tablica, wektor ze wzglêdu na du¿y rozmiar - pominiêcie stosu
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
		std::cout << "Z³a liczba argumentow!" << std::endl;
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
		std::cout << "Nie podano stalej zespolonej!" << std::endl;
	//Przechowanie realis i imaginalis w 2elementowej tablicy float-ów - ³atwiejsze przekazanie do funkcji
	float daneFloat[ILOSC_FLOAT];
	daneFloat[0] = zespolona.real();
	daneFloat[1] = zespolona.imag();
	//utworzenie pustej tablicy w¹tków dla ich ³atwiejszej obs³ugi
	std::vector<std::thread> watki;
	//offset dla podzielenia liczby pikseli które bêd¹ obs³ugiwane w ka¿dym z w¹tkow
	int offset = WYSOKOSC / liczbaWatkow;
	//tablica offsetów na wypadek braku podzielnoœci
	int reszta = WYSOKOSC - offset * liczbaWatkow;
	int * offsety = new int[liczbaWatkow + 1];
	for (int i = 0; i < liczbaWatkow + 1; i++)
		offsety[i] = 0;
	int a = 1; //na pierwszej pozycji musi byæ offset 0, bo nic nie by³o przesuwane wczeœniej
	while (reszta > 0)
	{
		offsety[a] = 1;
		a++;
		reszta--;
	}
	//uchwyt do biblioteki i œci¹gniêcie funkcji asemblerowej
	HINSTANCE dllHandle = NULL;
	dllHandle = LoadLibrary(L"DLL_ASM.dll");
	//wskaŸnik na funkcjê - z C lub asm i przypisanie w zale¿noœci od prze³¹cznika
	void(*funkcja)(float*, unsigned char*, int, int); 
	if (liczbaWatkow > -1 && argc > 3 && !strumienWej.fail())
	{
		if (tryb == "-c") //przypisanie w zale¿noœci od prze³¹cznika
			funkcja = fraktalJulii;
		else
			if (tryb == "-a")
				funkcja = (void(*)(float*, unsigned char*, int, int))GetProcAddress(dllHandle, "FraktalJulii");
		clock_t czas1 = clock(); //odczyt czasu przed w¹tkami
		for (int i = 0; i < liczbaWatkow; i++) //dodanie konkretnej iloœci w¹tków i funkcji z arg. do nich
			watki.push_back(std::thread(funkcja, daneFloat, &piksele[i * offset * SZEROKOSC + i* offsety[i]], i * offset * SZEROKOSC + i*offsety[i], offset * SZEROKOSC + offsety[i + 1]));
		for (int i = 0; i < liczbaWatkow; i++)
			watki[i].join(); //wywo³anie w¹tków
		clock_t czas2 = clock(); //odczyt czasu po w¹tkach
		std::cout << "Wykonano" << std::endl;
		std::cout << "Czas: " << (double)(czas2 - czas1) / CLOCKS_PER_SEC * 1.0 << std::endl;
	}
	std::cout << "Zapis do pliku. Nie zamykaj do informacji o zakonczeniu zapisu." << std::endl;
	//uzupe³nienie wartoœci wszystkich 4 bajtów odpowiedzialnych za piksel
	std::vector<unsigned char> wyjsciowa(WYSOKOSC * SZEROKOSC * BAJTY_KOLOROW);
	for (int i = 0; i < WYSOKOSC * SZEROKOSC; i++)
	{
		for (int j = 0; j < BAJTY_KOLOROW - 1; j++) //alfa ustalamy osobno
			wyjsciowa[i*BAJTY_KOLOROW + j] = piksele[i]; //wartoœæ piksela policzona raz i powielona - skala szaroœci
		wyjsciowa[i*BAJTY_KOLOROW + BAJTY_KOLOROW - 1] = 255; //alfa
	}
	//nag³ówek bitmapy - uzupe³nienie odpowiednich wartoœci pliku bitmapy
	BMPHeader naglowek1;
	naglowek1.Signature = 0x4D42;
	naglowek1.FileSize = 0x7A + wyjsciowa.size();
	naglowek1.Reserved1 = 0;
	naglowek1.Reserved2 = 0;
	naglowek1.Offset = 0x7A;
	//nag³ówek DIB - uzupe³nienie odpowiednich wartoœci pliku bitmapy
	DIBHeader naglowek2;
	naglowek2.DIBHeaderSize = 0x6C;
	naglowek2.ImageWidth = SZEROKOSC;
	naglowek2.ImageLength = WYSOKOSC;
	naglowek2.Planes = 1;
	naglowek2.BitsPerPixel = 32;
	naglowek2.Compression = 3;
	naglowek2.ImageSize = WYSOKOSC * SZEROKOSC * BAJTY_KOLOROW;
	naglowek2.XPixelsPerMeter = 2835;
	naglowek2.YPixelsPerMeter = 2835;
	naglowek2.ColorsInColorTable = 0;
	naglowek2.ImportantColorCount = 0;
	naglowek2.RedChannelBitmask = 0x00FF0000;
	naglowek2.GreenChannelBitmask = 0x0000FF00;
	naglowek2.BlueChannelBitmask = 0x000000FF;
	naglowek2.AlphaChannelBitmask = 0xFF000000;
	naglowek2.ColorSpaceType = 0x57696E20;
	for (int i = 0; i < 36; i++)
		naglowek2.ColorSpaceEndpoints[i] = 0;
	naglowek2.RedGamma = 0;
	naglowek2.GreenGamma = 0;
	naglowek2.BlueGamma = 0;
	std::ofstream plik; //utworzenie pliku bitmapy
	plik.open("Julia.bmp", std::ios::binary);
	plik.write((const char *)& naglowek1.Signature, sizeof naglowek1.Signature); //zapis kolejnych bajtów z odpowiednimi wartoœciami
	plik.write((const char *)& naglowek1.FileSize, sizeof naglowek1.FileSize);
	plik.write((const char *)& naglowek1.Reserved1, sizeof naglowek1.Reserved1);
	plik.write((const char *)& naglowek1.Reserved2, sizeof naglowek1.Reserved2);
	plik.write((const char *)& naglowek1.Offset, sizeof naglowek1.Offset);

	plik.write((const char *)& naglowek2.DIBHeaderSize, sizeof naglowek2.DIBHeaderSize);
	plik.write((const char *)& naglowek2.ImageWidth, sizeof naglowek2.ImageWidth);
	plik.write((const char *)& naglowek2.ImageLength, sizeof naglowek2.ImageLength);
	plik.write((const char *)& naglowek2.Planes, sizeof naglowek2.Planes);
	plik.write((const char *)& naglowek2.BitsPerPixel, sizeof naglowek2.BitsPerPixel);

	plik.write((const char *)& naglowek2.Compression, sizeof naglowek2.Compression);
	plik.write((const char *)& naglowek2.ImageSize, sizeof naglowek2.ImageSize);
	plik.write((const char *)& naglowek2.XPixelsPerMeter, sizeof naglowek2.XPixelsPerMeter);
	plik.write((const char *)& naglowek2.YPixelsPerMeter, sizeof naglowek2.YPixelsPerMeter);
	plik.write((const char *)& naglowek2.ColorsInColorTable, sizeof naglowek2.ColorsInColorTable);

	plik.write((const char *)& naglowek2.ImportantColorCount, sizeof naglowek2.ImportantColorCount);
	plik.write((const char *)& naglowek2.RedChannelBitmask, sizeof naglowek2.RedChannelBitmask);
	plik.write((const char *)& naglowek2.GreenChannelBitmask, sizeof naglowek2.GreenChannelBitmask);
	plik.write((const char *)& naglowek2.BlueChannelBitmask, sizeof naglowek2.BlueChannelBitmask);
	plik.write((const char *)& naglowek2.AlphaChannelBitmask, sizeof naglowek2.AlphaChannelBitmask);

	plik.write((const char *)& naglowek2.ColorSpaceType, sizeof naglowek2.ColorSpaceType);
	for (int i = 0; i < 36; i++)
		plik.write((const char *)& naglowek2.ColorSpaceEndpoints[i], sizeof naglowek2.ColorSpaceEndpoints[i]);
	plik.write((const char *)& naglowek2.RedGamma, sizeof naglowek2.RedGamma);
	plik.write((const char *)& naglowek2.GreenGamma, sizeof naglowek2.GreenGamma);
	plik.write((const char *)& naglowek2.BlueGamma, sizeof naglowek2.BlueGamma);

	for (int i = 0; i < WYSOKOSC; i++) //zapis wszystkich bitów bitmapy (w postaci 4 bajtów RGBA)
		for (int j = 0; j < SZEROKOSC; j++)
			for (int k = 0; k < BAJTY_KOLOROW; k++) //zapis od ty³u, gdy¿ tak przechowywane s¹ informacje w bmp
				plik.write((const char *)& wyjsciowa[(WYSOKOSC - i - 1)*SZEROKOSC*BAJTY_KOLOROW + j*BAJTY_KOLOROW + k], sizeof wyjsciowa[(WYSOKOSC - i - 1)*SZEROKOSC*BAJTY_KOLOROW + j*BAJTY_KOLOROW + k]);
	plik.close();
	std::cout << "Zapisano. <Enter> by zakonczyc.";
	getchar();
    return 0;
}

