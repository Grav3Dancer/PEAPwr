#include <iostream>
#include <windows.h>
#include <utility>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <vector>
#include <list>
#include <bits/stdc++.h>
using namespace std;

int** mapa;
int rozmiar;
int* sciezka;
int koszt;

void wyswietl() //wyœwietlanie mapy odleg³oœci jako macierzy
{
	cout<<"Mapa odleglosci : "<<endl;
	for (int i=0;i<rozmiar;i++)
	{
		for (int j=0;j<rozmiar;j++)
		{
			cout<<mapa[i][j]<< " ";
		}
		cout<<endl;
	}
	cout<<endl;
}

void stworzMape(int size) //tworzenie mapy odleg³oœci
{
	rozmiar = size;
	mapa = new int*[rozmiar];
	for (int i=0;i<rozmiar;i++)
	{
		mapa[i] = new int[rozmiar];
		for (int j=0;j<rozmiar;j++)
		{
			mapa[i][j] = 0;
		}
	}
}



void usunMape() //usuwanie mapy
{
	for (int i=0;i<rozmiar;i++)
	{
		delete[] mapa[i];
	}
	delete[] mapa;
}

void wczytaj_z_pliku(string filename) // wczytanie mapy z pliku
{
	ifstream fin(filename);
	
	int size;
	
	fin>>size;
	
	for (int i=0;i<size;i++)
	{
		for (int j=0;j<size;j++)
		{
			fin>>mapa[i][j];
		}
	}	
}

struct subset //podzbiór wierzcho³ków
{
	int przedostatni;
	int koszt;
};

subset** tab; //wszystkie podzbiory

void stworzSubset() //tworzenie tabeli ze wszystkimi podzbiorami
{
	tab = new subset*[1<<rozmiar];
	for (int i=0;i<(1<<rozmiar);i++)
	{
		tab[i] = new subset[rozmiar];
		for (int j=0;j<rozmiar;j++)
		{
			tab[i][j].koszt = 0;
			tab[i][j].przedostatni = 0;
		}
	}
}

void usunSubset() //usuwanie tabeli podzbiorów
{
	for (int i=0;i<(1<<rozmiar);i++)
	{
		delete[] tab[i];
	}
	delete[] tab;
}

int Held_Karp() //g³ówny algorytm
{	
	
	stworzSubset();
	
	//***************************** [B1] *****************************
	
	for(int i=1;i<rozmiar;i++) //przypisanie odleg³oœci dla podzbiorów jednoelementowych
	{
		tab[1<<i][i].koszt = mapa[0][i];
		tab[1<<i][i].przedostatni = 0;
	}
	
	//****************************************************************
	
	
	
	//***************************** [B2] *****************************
	
	for (int subsetSize = 2;subsetSize<rozmiar;subsetSize++) //przegl¹danie podzbiorów coraz wiêkszych rozmiarów
	{
		bool subsetTab[rozmiar]; //zmienna do generowania podzbiorów
		subsetTab[0] = 0;
		
		for (int i=1;i<rozmiar;i++) //pierwszy podzbiór danego rozmiaru
		{
			if (i <= subsetSize) subsetTab[i]=1;
			else subsetTab[i]=0;
		}
			
		do
		{	
			unsigned int temp = 0;
			//unsigned long long temp =0; //wierzcho³ki w zapisie bitowym
			
			for(int i=1;i<rozmiar;i++)
			{
				if(subsetTab[i]) temp |= 1 << i;
			}
			
			for (int i=1;i<rozmiar;i++) //szukanie minimalnego kosztu dotarcia do zbioru
			{
				if (subsetTab[i]) 
				{
					unsigned int temp2 = temp & ~(1<<i);
					//unsigned long long temp2 = temp & ~(1<<i); //podzbiór bez i-tego wierzcho³ka
					
					int dlugosc = INT_MAX;
					int koncowy_temp;
					for(int j=1;j<rozmiar;j++)
					{
						if (!subsetTab[j] || j==i) continue;
						
						if (dlugosc > tab[temp2][j].koszt + mapa[j][i]) //wybranie najlepszej œcie¿ki z ju¿ obliczonych
						{
							dlugosc = tab[temp2][j].koszt + mapa[j][i];
							koncowy_temp = j;
						}
					}
					
					tab[temp][i].koszt = dlugosc;
					tab[temp][i].przedostatni = koncowy_temp;
					
				}
			}
			
		} while (prev_permutation(subsetTab+1, subsetTab+rozmiar)); //kolejny podzbiór o danej liczbie elementów
		
	}
	//****************************************************************
	
	
	//***************************** [B3] *****************************
	
	int temp = (1<<rozmiar)-2;
	
	int ostatni; // ostatni wierzcholek przed 0
	int best = INT_MAX;
	
	for (int i=1;i<rozmiar;i++) //obliczenie najlepszego rozwiazania uwzglêdniaj¹c powrót z wierzcholka ostatniego do 0
	{
		if (best > (tab[temp][i].koszt + mapa[i][0]))
		{
			best = (tab[temp][i].koszt + mapa[i][0]);
			ostatni = i;
		}
	}
	//****************************************************************
	
	
	//***************************** [B4] *****************************
	
	for (int i=(rozmiar-1);i>0;i--) //odtworzenie œcie¿ki optymalnej
	{
		
		sciezka[i] = ostatni;
		int temp2 = temp & ~(1<<ostatni);
		ostatni = tab[temp][ostatni].przedostatni;
		temp = temp2;
	}
	sciezka[0] = 0;
	sciezka[rozmiar] = 0;
	//****************************************************************
	
	
	//wykorzystywane podczas mierzenia zu¿ycia pamiêci dla mniejszych instancji
	//	cout<<"*********przerwa********"<<endl;
	//	Sleep(10000);
	
	usunSubset();
	
	return best;
}
///////////////////
int main(int argc, char** argv)
{
	string ini_name; // nazwa pliku .ini
	cout<<"Podaj nazwe pliku .ini: "<<endl;
	cin>>ini_name;
	
	ifstream ini_in(ini_name);
	
	ifstream ini_outname(ini_name);
	string csv_out_name;
	
	while(!ini_outname.eof()) ini_outname >> csv_out_name; // wczytanie nazwy pliku wynikowego z koñca pliku konfiguracyjnego
	
	ofstream csv_out(csv_out_name);
	
	while (true) //badanie kolejnych instancji
{
	string nazwa;
	int powtorzenia, optymalne;
	
	ini_in>>nazwa;
	if (nazwa == csv_out_name) break; // nazwa pliku wynikowego -> koniec nazw instancji do badania
	
	ini_in>>powtorzenia>>optymalne; // ilosc powtorzen, najlepszy koszt dla danej instancji
	
	ifstream tsp_in(nazwa);
	
	 
	tsp_in>> rozmiar;
	
	sciezka = new int[rozmiar+1];
	
	int sciezkaOpt[rozmiar+1];
	for (int i=0;i<=rozmiar;i++) ini_in>>sciezkaOpt[i]; // wczytywanie optymalnej œcie¿ki z pliku konf.
	
	stworzMape(rozmiar);
	
	for (int i=0; i<rozmiar;i++)
	{
		for (int j=0; j<rozmiar;j++)
			
			tsp_in>>mapa[i][j]; // wczytywanie macierzy z zadanego pliku
	}

 // zmienne punktów czasowych dla zegara
chrono::steady_clock::time_point start;
chrono::steady_clock::time_point end;
long double czas;

// dane z pliku .ini do pliku wynikowego
csv_out<<nazwa<<";"<<powtorzenia<<";"<<optymalne<<";";
for (int i=0;i<=rozmiar;i++) csv_out<<sciezkaOpt[i]<<";";
csv_out<<"\n";
cout<<nazwa<<endl;
	
for (int w=0;w<powtorzenia;w++) // powtarzanie algorytmu zadan¹ iloœæ razy
{
	int cost;
	start = chrono::steady_clock::now();

	cost = Held_Karp();
	
	end = chrono::steady_clock::now();
	czas = chrono::duration_cast<chrono::microseconds>(end - start).count(); // czas dzia³ania 
	
	csv_out<<czas<<"\n";
	cout<<"czas "<<w+1<<" "<<czas<<endl;
	
	//***************************** [B5] ***************************** 
	if (w == powtorzenia-1) // po ostatnim powtórzeniu
	{	
		cout<<" koszt: "<<cost<<endl<<"sciezka: ";
		csv_out<<cost<<";";
		for (int i=0;i<rozmiar+1;i++) 
		{
			cout<<sciezka[i]<<"  ";
			csv_out<<sciezka[i]<<";";
		}
		csv_out<<"\n";
		cout<<endl;	
	}
	//****************************************************************
}

delete[] sciezka;
usunMape();

}
	
	return 0;
}
