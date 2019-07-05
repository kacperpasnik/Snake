// Snake.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include "pch.h"
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <iostream>

const short min_szerokosc = 10, max_szerokosc = 50;
const short min_wysokosc = 10, max_wysokosc = 25; 
//W tej zmiennej przechowywane sa wszystkie znaki jakie ma petla narysować
char *pole_gry = new char[1500];
//Zmienna do okreslenia gdzie w powyzszej zmiennej jest cialo weza- cialo_weza[0]=poczatek ciala, cialo_weza[punkty+3]=koniec
int *cialo_weza = new int[1250]; 
short punkty = 0;
//predkosc gry- opoznienie w ms odswiezania mapy i weza
int predkosc = 400;

struct wymiary	
{
	int szerokosc = 20, wysokosc = 10; //najlepsze (i zalecane) wymiary gry
};
//Funkcja rysujaca plansze wraz z wezem
void Plansza(int ostatnie_pole);
//W tej funkcji dzieja sie najwazniejsze rzeczy
void Uruchom(wymiary mapa);
/*Funkcja, ktora porusza glowa weza jezeli gracz nie wykonal ruchu (w srodku przechodzi rowniez do funkcji ruchu ciala weza).
Funkcja jest obszerna, ponieważ głowa weza moze miec 4 rozne pozycje- ^, v, < i >*/
bool RuchGlowyWeza(int &glowa_weza, int x, int y);
/*Przeciazana funkcja, ktora porusza glowa weza jezeli gracz wykonal ruch
Funkcja ta rowniez jest obszerna z powodu 4 roznych mozliwosci pozycji weza*/
bool RuchGlowyWeza(int &glowa_weza, int ruch_gracza, int x, int y); 
void RuchCialaWeza(int glowa_weza);
//Funkcja, ktora jednorazowo tworzy i zapisuje do zmiennej pole_gry[] wszystkie znaki, ktore nastepnie funkcja Plansza bedzie wywolywac w petli
void ZdefiniujPole(int x, int y, int &aktualne_pole, int &glowa_weza);
void LosujJedzenie(int x, int y);

int main()
{
	srand(time(NULL));
	wymiary mapa;
	do
	{
		printf("Podaj szerokosc mapy (min 10, max 50): ");
		std::cin >> mapa.szerokosc;
	} while (mapa.szerokosc<min_szerokosc || mapa.szerokosc>max_szerokosc);
	do
	{
		printf("Podaj wysokosc mapy (min 10, max 25): ");
		std::cin >> mapa.wysokosc;
	} while (mapa.wysokosc<min_wysokosc || mapa.wysokosc>max_wysokosc);
	//wywolanie najwazniejszej funkcji
	Uruchom(mapa);
	printf("Koniec gry, wynik: %d", punkty);
	Sleep(2000);
	delete[] pole_gry;
	delete[] cialo_weza;
	return 0;
}

void Uruchom(wymiary mapa)
{
	//jezeli ta zmienna jest false, to znaczy ze waz wszedl w granice mapy lub zjadl siebie
	bool dzialanie_gry = true;
	/*ostatnie pole jest zmienna potrzebna do zdefiniowania ile pol miesci sie w zmiennej pole_gry[]
	glowa weza to miejsce, w ktorym aktualnie znajduje sie poczatek weza */
	int ostatnie_pole = 0, glowa_weza = 0;
	//zmienna, ktora zawiera znak wcisniety przez gracza
	char klawisz;
	ZdefiniujPole(mapa.szerokosc, mapa.wysokosc, ostatnie_pole, glowa_weza);
	LosujJedzenie(mapa.szerokosc, mapa.wysokosc);
	Plansza(ostatnie_pole);
	while (true)
	{
		//kbhit kb (KeyBoard) + hit. zwraca true jezeli uzytkownik wcisnal klawisz na klawiaturze
		if (_kbhit())
		{
			//funkcja getch zwraca wcisniety klawisz
			klawisz = _getch();
			//warunek potrzebny, zeby gracz nie mogl wcisnac tego samego lub przeciwnego kierunku w czasie gry
			if (((klawisz == 'w' || klawisz == 's') && (pole_gry[glowa_weza] == 'v' || pole_gry[glowa_weza] == '^')) ||
				((klawisz == 'a' || klawisz == 'd') && (pole_gry[glowa_weza] == '>' || pole_gry[glowa_weza] == '<')))  
				continue;
			else if (klawisz == 'w' || klawisz == 'a' || klawisz == 's' || klawisz == 'd')
				dzialanie_gry = RuchGlowyWeza(glowa_weza, klawisz, mapa.szerokosc, mapa.wysokosc);
			else continue;
		}
		else
			dzialanie_gry = RuchGlowyWeza(glowa_weza, mapa.szerokosc, mapa.wysokosc);
		system("cls");
		if (!dzialanie_gry)
			break;
		Plansza(ostatnie_pole);
		Sleep(predkosc);
	}

}


void Plansza(int ostatnie_pole)
{
	for (int i = 0; i < ostatnie_pole; i++)
		printf("%c", pole_gry[i]);
}


void ZdefiniujPole(int x, int y, int &aktualne_pole, int &glowa_weza)
{
	//X=granica mapy, [spacja]= puste pole w grze, >= glowa weza, o= cialo weza, *= jedzenie
	aktualne_pole = 0;
	for (int i = 0; i < x + 2; i++, aktualne_pole++)
		pole_gry[aktualne_pole] = 'X';
	pole_gry[aktualne_pole] = '\n';
	aktualne_pole++;
	for (int i = 0; i < y; i++, aktualne_pole += 2)
	{
		pole_gry[aktualne_pole] = 'X';
		aktualne_pole++;
		for (int j = 0; j < x; j++, aktualne_pole++)
		{
			//poczatek weza (poczatkowy waz ma 3 pola, przez co w programie czesto bedzie uzywane punkty+3, bo gracz zaczyna z zerowa iloscia punktow)
			if (i == 4 && j == 4)
			{
				pole_gry[aktualne_pole] = pole_gry[aktualne_pole + 1] = pole_gry[aktualne_pole + 2] = 'o';
				cialo_weza[2] = aktualne_pole;
				cialo_weza[1] = aktualne_pole + 1;
				cialo_weza[0] = aktualne_pole + 2;
				aktualne_pole += 3;
				pole_gry[aktualne_pole] = '>';
				glowa_weza = aktualne_pole;
				j += 3;
				continue;
			}

			pole_gry[aktualne_pole] = ' ';
		}
		pole_gry[aktualne_pole] = 'X';
		pole_gry[aktualne_pole + 1] = '\n';
	}
	for (int i = 0; i < x + 2; i++, aktualne_pole++)
		pole_gry[aktualne_pole] = 'X';

}


bool RuchGlowyWeza(int &glowa_weza, int x, int y)
{
	//jezeli idzie w gore
	if (pole_gry[glowa_weza] == '^')
	{
		
		std::swap(pole_gry[glowa_weza], pole_gry[glowa_weza - (x + 3)]);
		//warunek napotkania jedzenia
		if (pole_gry[glowa_weza] == '*') 
		{
			pole_gry[glowa_weza] = 'o'; 

			punkty++;
			cialo_weza[punkty + 3 - 1] = glowa_weza;
			glowa_weza = glowa_weza - (x + 3);
			LosujJedzenie(x, y);
			predkosc -= 10;
			return true; 
		}
		if (pole_gry[glowa_weza] != ' ') 
			return false;
		RuchCialaWeza(glowa_weza); 
		glowa_weza = glowa_weza - (x + 3);
	}
	//jezeli idzie w lewo
	else if (pole_gry[glowa_weza] == '<')
	{
		std::swap(pole_gry[glowa_weza], pole_gry[glowa_weza - 1]);
		if (pole_gry[glowa_weza] == '*')
		{
			pole_gry[glowa_weza] = 'o';

			punkty++;
			cialo_weza[punkty + 3 - 1] = glowa_weza;
			glowa_weza--;
			LosujJedzenie(x, y);
			predkosc -= 10;
			return true;
		}
		if (pole_gry[glowa_weza] != ' ')
			return false;
		RuchCialaWeza(glowa_weza);
		glowa_weza--;
	}
	//jezeli idzie w dol
	else if (pole_gry[glowa_weza] == 'v')
	{
		std::swap(pole_gry[glowa_weza], pole_gry[glowa_weza + (x + 3)]);
		if (pole_gry[glowa_weza] == '*')
		{
			pole_gry[glowa_weza] = 'o';

			punkty++;
			cialo_weza[punkty + 3 - 1] = glowa_weza;
			glowa_weza = glowa_weza + (x + 3);
			LosujJedzenie(x, y);
			predkosc -= 10;
			return true;
		}
		if (pole_gry[glowa_weza] != ' ')
			return false;
		RuchCialaWeza(glowa_weza);
		glowa_weza = glowa_weza + (x + 3);
	}
	//jezeli idzie w prawo
	else if (pole_gry[glowa_weza] == '>')
	{
		std::swap(pole_gry[glowa_weza], pole_gry[glowa_weza + 1]);
		if (pole_gry[glowa_weza] == '*')
		{
			pole_gry[glowa_weza] = 'o';

			punkty++;
			cialo_weza[punkty + 3 - 1] = glowa_weza;
			glowa_weza++;
			LosujJedzenie(x, y);
			predkosc -= 10;
			return true;
		}
		if (pole_gry[glowa_weza] != ' ')
			return false;
		RuchCialaWeza(glowa_weza);
		glowa_weza++;
	}
	return true;
}

bool RuchGlowyWeza(int &glowa_weza, int ruch_gracza, int x, int y)
{
	//jezeli gracz wcisnal "gora" (w)
	if (ruch_gracza == 119)
	{
		//zamien pozycje glowy
		pole_gry[glowa_weza] = '^';
		std::swap(pole_gry[glowa_weza], pole_gry[glowa_weza - (x + 3)]);
		if (pole_gry[glowa_weza] == '*')
		{
			pole_gry[glowa_weza] = 'o';

			punkty++;
			cialo_weza[punkty + 3 - 1] = glowa_weza;
			glowa_weza = glowa_weza - (x + 3);
			LosujJedzenie(x, y);
			predkosc -= 10;
			return true;
		}
		if (pole_gry[glowa_weza] != ' ')
			return false;
		RuchCialaWeza(glowa_weza);
		glowa_weza = glowa_weza - (x + 3);
	}
	//jezeli gracz wcisnal "lewo" (a)
	else if (ruch_gracza == 97)
	{
		//zamien pozycje glowy
		pole_gry[glowa_weza] = '<';
		std::swap(pole_gry[glowa_weza], pole_gry[glowa_weza - 1]);
		if (pole_gry[glowa_weza] == '*')
		{
			pole_gry[glowa_weza] = 'o';

			punkty++;
			cialo_weza[punkty + 3 - 1] = glowa_weza;
			glowa_weza--;
			LosujJedzenie(x, y);
			predkosc -= 10;
			return true;
		}
		if (pole_gry[glowa_weza] != ' ')
			return false;
		RuchCialaWeza(glowa_weza);
		glowa_weza--;
	}
	//jezeli gracz wcisnal "dol" (s)
	else if (ruch_gracza == 115)
	{
		//zamien pozycje glowy
		pole_gry[glowa_weza] = 'v';
		std::swap(pole_gry[glowa_weza], pole_gry[glowa_weza + (x + 3)]);
		if (pole_gry[glowa_weza] == '*')
		{
			pole_gry[glowa_weza] = 'o';

			punkty++;
			cialo_weza[punkty + 3 - 1] = glowa_weza;
			glowa_weza = glowa_weza + (x + 3);
			LosujJedzenie(x, y);
			predkosc -= 10;
			return true;
		}
		if (pole_gry[glowa_weza] != ' ')
			return false;
		RuchCialaWeza(glowa_weza);
		glowa_weza = glowa_weza + (x + 3);

	}
	//jezeli gracz wcisnal "prawo" (d)
	else if (ruch_gracza == 100 && pole_gry[glowa_weza] != '<')
	{
		//zamien pozycje glowy
		pole_gry[glowa_weza] = '>';
		std::swap(pole_gry[glowa_weza], pole_gry[glowa_weza + 1]);
		if (pole_gry[glowa_weza] == '*')
		{
			pole_gry[glowa_weza] = 'o';

			punkty++;
			cialo_weza[punkty + 3 - 1] = glowa_weza;
			glowa_weza++;
			LosujJedzenie(x, y);
			predkosc -= 10;
			return true;
		}
		if (pole_gry[glowa_weza] != ' ')
			return false;
		RuchCialaWeza(glowa_weza);
		glowa_weza++;
	}
	return true;
}

void RuchCialaWeza(int glowa_weza)
{
	for (int i = 0, j = glowa_weza; i < punkty + 3; i++)
	{
		/*zamien aktualnie przetwarzane pole_gry[] (czyli pusta przestrzen) z pole_gry[cialo_weza], gdzie cialo_weza[] jest tablica
		zawierajaca kolejnosc ciala weza przed aktualnym odswiezeniem planszy */
		std::swap(pole_gry[j], pole_gry[cialo_weza[i]]);
		std::swap(j, cialo_weza[i]);
	}

}

void LosujJedzenie(int x, int y)
{
	int zakres = x * y;
	int losuj = rand() % zakres + 1;
	while (true)
	{
		if (pole_gry[losuj + (x + 3)] == ' ')
		{
			pole_gry[losuj + (x + 3)] = '*';
			break;
		}
		else
			losuj = rand() % zakres + 1;
	}

}