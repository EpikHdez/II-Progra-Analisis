// Needleman-Wunsch.cpp: archivo de proyecto principal.

#include "stdafx.h"

using namespace System;

int max(int num1, int num2, int num3)
{
	int m1 = num1 > num2 ? num1 : num2;
	int m2 = num2 > num3 ? num2 : num3;

	return m1 > m2 ? m1 : m2;
}

int NeedlemanWunsch(String ^h1, String ^h2)
{
	int match, insert, erase;
	int length1 = h1->Length;
	int length2 = h2->Length;
	array<int, 2> ^F = gcnew array<int, 2>(length1, length2);

	for (int i = 0; i < length1; i++)
		F[i, 0] = -i;

	for (int j = 0; j < length2; j++)
		F[0, j] = -j;

	for (int i = 1; i < length1; i++)
	{
		for (int j = 1; j < length2; j++)
		{
			match = F[i - 1, j - 1] + (h1[i] == h2[j] ? 1 : -1);
			insert = F[i, j - 1] - 1;
			erase = F[i - 1, j] - 1;
			F[i, j] = max(match, insert, erase);
		}
	}

	return F[length1 - 1, length2 - 1];
}

int main(array<System::String ^> ^args)
{
	String ^h1, ^h2, ^userAnswer;
	bool keepIt = true;

	while (keepIt)
	{
		Console::Write(L"Ingrese la primera hilera a comparar: ");
		h1 = Console::ReadLine();
		Console::Write(L"Ingrese la segunda hilera a comparar: ");
		h2 = Console::ReadLine();

		Console::WriteLine(L"\nCalculando...");
		Console::WriteLine(L"El costo total es de {0}.\n", NeedlemanWunsch(h1, h2));

		Console::Write(L"Desea evaluar otras hileras? (y/n): ");
		userAnswer = Console::ReadLine();

		if (userAnswer == "n" || userAnswer == "N")
			keepIt = false;
	}

    return 0;
}
