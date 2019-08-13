// DLL_C.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DLL_C.h"
#include <iostream>

void fraktalJulii(float* tablica, unsigned char* tablicaPixeli, int index, int offsetPikseli) 
{
	std::cout << "Otrzymany adres: " << tablica << std::endl << " adres tablicy pixeli: " << tablicaPixeli <<
		std::endl << " index tablicy pixeli: " << index << " offset tablicy pixeli: " << offsetPikseli << std::endl;
}

