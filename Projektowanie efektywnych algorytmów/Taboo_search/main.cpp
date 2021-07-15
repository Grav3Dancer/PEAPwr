#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <algorithm>
#include <math.h>
#include <vector>
using namespace std;

int** mapa;
int rozmiar;
int* sciezka;
int koszt;

int** tabu_list;
int kadencja;

int beg = 1; // 0 - najlepsze z rozmiar*rozmiar losowych, 1 - random, 2 - <1,2,3,4...>
int kad = 3; // dlugosc_kadecji = rozmiar/kad lub rozmiar*kad



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

void czyscTabu()
{
	for (int i = 0; i < rozmiar; i++)
	{
		for (int j = 0; j < rozmiar; j++)
		{
			tabu_list[i][j] = 0;
		}
	}
}

void zmniejszKadencje()
{
	for (int i = 0; i < rozmiar; i++)
	{
		for (int j = 0; j < rozmiar; j++)
		{
			if (tabu_list[i][j] > 0) tabu_list[i][j] -= 1;
		}
	}
}


bool szukajTabu(int x, int y) // czy zamiana znajduje siê w liscie tabu
{
	if (tabu_list[x][y] != 0 || tabu_list[y][x] != 0) return true;
	else return false;
}


void wpiszTabu(int x, int y)
{
	tabu_list[x][y] = kadencja;
	tabu_list[y][x] = kadencja;
}


void stworzMape(int size) //tworzenie mapy odleg³oœci
{
	rozmiar = size;
	
	mapa = new int*[rozmiar];
	
	tabu_list = new int*[rozmiar];
	
	kadencja = rozmiar / kad;
	
	for (int i=0;i<rozmiar;i++)
	{
		mapa[i] = new int[rozmiar];
		tabu_list[i] = new int[rozmiar];
		
		for (int j=0;j<rozmiar;j++)
		{
			mapa[i][j] = 0;
			tabu_list[i][j] = 0;
		}
	}
	
	sciezka = new int [rozmiar+1];

}

void usunMape() //usuwanie mapy
{
	for (int i=0;i<rozmiar;i++)
	{
		delete[] mapa[i];
		delete[] tabu_list[i];
	}
	delete[] mapa;
	delete[] tabu_list;
	delete[] sciezka;
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

int obliczKoszt(int* path)
{
	int cost = 0;
	
	for (int i = 0; i<rozmiar; i++)
	{
		cost += mapa[path[i]][path[i+1]];
		
	}
	
	return cost;
}


int Taboo ()
{
	int bestGlobalCost;
	int bestGlobalPath[rozmiar+1];
	
	int bestCost=0;
	int bestPath[rozmiar+1];
	
	int tempStart[rozmiar+1]={0};
	
	bestPath[0]=0;
	bestPath[rozmiar]=0;
	
	///// **[B1]**
	for (int i=1;i<rozmiar;i++) 
	{
		bestPath[i]=i;
		tempStart[i]=i;
	}
	
	bestCost = obliczKoszt(bestPath);
	
	if (beg == 0)
		for (int i=0;i<(rozmiar*rozmiar);i++)
		{
			random_shuffle(tempStart+1, tempStart+rozmiar);
			int ab = obliczKoszt(tempStart);
			if (ab < bestCost)
			{	
				bestCost = ab;
				for (int k = 0;k<=rozmiar;k++)
					bestPath[k] = tempStart[k];
			}
		}
	else if(beg == 1) 
	{
		random_shuffle(bestPath+1, bestPath+rozmiar);
		bestCost = obliczKoszt(bestPath);
	}
	///// ****
	
	///// **[B2]** 
	bestGlobalCost = bestCost;
	
	for (int i=0;i<=rozmiar;i++) bestGlobalPath[i] = bestPath[i];
	/////
	
	///// **[B3]**
	int licznik = 0;
	int licznik2 = 0;
	
	chrono::steady_clock::time_point TS_start;
	chrono::steady_clock::time_point TS_end;
	long double TS_czas;
	
	TS_start = chrono::steady_clock::now();
	///// ****
	
	for (int i = 0; i < 6000; i++) ///// **[B4]**
	{
		
		if (licznik2 > 5*rozmiar) break; ///// **[B5]**
		
		if (licznik > (rozmiar)) ///// **[B6]**
		{
			///// **[B7]**
			random_shuffle(bestPath+1, bestPath+rozmiar);
			bestCost = obliczKoszt(bestPath);
			licznik = 0;
			///// ****
		}
		
		///// **[B8]**
		zmniejszKadencje();
		
		//zmienne najlepszej sciezki z sasiedztwa
		int bestNbhPath[rozmiar+1];
		int bestNbhCost = INT_MAX;
		
		//zamieniane wierzcholki
		int first, second;
		
		licznik++;
		licznik2++;
		///// ****
		
		//przegladanie calego sasiedztwa
		for (int a = 1; a < (rozmiar-1); a++) //a - pierwszy wierzcholek
		{
			///// **[B9]**
			for (int b = a+1; b < rozmiar; b++) //b - drugi wierzcholek
			{
				int tempCost;
				int tempPath[rozmiar+1];
				int tempPath1[rozmiar+1];
				int tempPath2[rozmiar+1];
				int tempPath3[rozmiar+1];
				
				for (int k=0;k<=rozmiar;k++)
				{
					tempPath[k] = bestPath[k];
					tempPath1[k] = bestPath[k];
					tempPath2[k] = bestPath[k];
					tempPath3[k] = bestPath[k];
				}
				
				int x = tempPath[a];
				int y = tempPath[b];
				
				// swap
				int temp = tempPath1[a];
				tempPath1[a] = tempPath1[b];
				tempPath1[b] = temp;
				
				
				// invert
				int c=0;	
				for(int k=b;k>=a;k--)
				{
					tempPath2[a+c] = bestPath[k];
					c++;
				}
				
				
				// insert
				tempPath3[a] = bestPath[b];	
				for (int k = a+1; k <= b; k++)
					tempPath3[k] = bestPath[k-1];
				
				
				int tempCost1 = obliczKoszt(tempPath1); // koszt sprawdzanej sciezki
				int tempCost2 = obliczKoszt(tempPath2);
				int tempCost3 = obliczKoszt(tempPath3);
				
				int lowest = INT_MAX;
				if (tempCost1 < lowest)
				{
					lowest = tempCost1;
					for(int k=0;k<=rozmiar;k++) tempPath[k] = tempPath1[k];
				}
				
				if (tempCost2 < lowest)
				{
					lowest = tempCost2;
					for(int k=0;k<=rozmiar;k++) tempPath[k] = tempPath2[k];
				}
				
				if (tempCost3 < lowest)
				{
					lowest = tempCost3;
					for(int k=0;k<=rozmiar;k++) tempPath[k] = tempPath3[k];
				}
				
				tempCost = lowest;
				
				///// **[B10]**
				if ( tempCost < bestGlobalCost) // bezwarunkowa akceptacja jeœli lepszy od najlepszego
				{
					///// **[B11]**
					bestGlobalCost = tempCost;
					for (int k=0;k<=rozmiar;k++) bestGlobalPath[k] = tempPath[k];
					
					bestNbhCost = tempCost;
					for (int k=0;k<=rozmiar;k++) bestNbhPath[k] = tempPath[k];
					
					first = x;
					second = y;
					
					licznik = 0;
					licznik2 = 0;
					///// ****
				}
				
				if (tempCost >= bestNbhCost)
				{
					//nic 
				}
					
				else if ( tempCost < bestNbhCost && !szukajTabu(x,y) ) ///// **[B12]**
				{
					///// **[B13]**
					bestNbhCost = tempCost;
					for (int k = 0;k<rozmiar+1;k++) bestNbhPath[k]=tempPath[k];
					
					first = x;
					second = y;
					///// ****
				}
				
			}
				
		}
		
		for (int k=0;k<=rozmiar;k++) bestPath[k] = bestNbhPath[k]; ///// **[B14]**
		
		wpiszTabu(first, second); ///// **[B14]**
		
		
		///// **[B15]**
		TS_end = chrono::steady_clock::now();
		TS_czas = chrono::duration_cast<chrono::seconds>(TS_end - TS_start).count();
		if (TS_czas > 180) break;
		///// ****
		
	}
	
	
	
	
	///////////
	
	
	
	///// **[B16]**
	for (int k=0;k<=rozmiar;k++) sciezka[k] = bestGlobalPath[k];
	return bestGlobalCost;
}




int main(int argc, char** argv) 
{	
	srand(time(NULL));
	
	
	string ini_name; // nazwa pliku .ini
	cout<<"Podaj nazwe pliku .ini: "<<endl;
	cin>>ini_name;
	
	ifstream ini_in(ini_name);
	
	ifstream ini_outname(ini_name);
	string csv_out_name;
	
	while(!ini_outname.eof()) ini_outname >> csv_out_name; // wczytanie nazwy pliku wynikowego z koñca pliku konfiguracyjnego
	
	ofstream csv_out(csv_out_name);

	
	while (true)
	{
		string nazwa;
		int powtorzenia, optymalne;
	
		ini_in>>nazwa;
		if (nazwa == csv_out_name) break; // nazwa pliku wynikowego -> koniec nazw instancji do badania
	
		ini_in>>powtorzenia>>optymalne; // ilosc powtorzen, najlepszy koszt dla danej instancji
		
		ifstream tsp_in(nazwa);
		
		int rozmiarMapy;
		tsp_in>> rozmiarMapy;
		stworzMape(rozmiarMapy);
		for (int i=0; i<rozmiar;i++)
		{
			for (int j=0; j<rozmiar;j++)
				tsp_in>>mapa[i][j]; // wczytywanie macierzy z zadanego pliku
		}
		
		chrono::steady_clock::time_point start;
		chrono::steady_clock::time_point end;
		long double czas;

		// dane z pliku .ini do pliku wynikowego
		csv_out<<nazwa<<";"<<powtorzenia<<";"<<optymalne<<"\n";
		
		cout<<nazwa<<" "<<optymalne<<endl;
		
		for (int i=0;i<powtorzenia;i++)
		{
			int cost;
			start = chrono::steady_clock::now();
			cost = Taboo();
			end = chrono::steady_clock::now();
			czas = chrono::duration_cast<chrono::milliseconds>(end - start).count(); // czas dzia³ania
		
			cout<<"koszt: "<<cost<< "  blad: " << (double)cost/(double)optymalne<<" czas: "<<czas<<"ms"<<endl;
			
			
			csv_out<<czas<<";"<<cost<<";"<<(double)cost/(double)optymalne<<"\n";
		}
		usunMape();
	}

	
	return 0;
}
