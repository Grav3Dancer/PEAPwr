#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <algorithm>
#include <math.h>
using namespace std;

int** mapa;
int rozmiar;
int* sciezka;
int koszt;

int zamiana = 2;

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
	sciezka = new int [rozmiar+1];
}

void usunMape() //usuwanie mapy
{
	for (int i=0;i<rozmiar;i++)
	{
		delete[] mapa[i];
	}
	delete[] mapa;
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

int obliczKoszt(int *path)
{
	int cost = 0;
	
	for (int i = 0;i<rozmiar;i++)
		cost += mapa[path[i]][path[i+1]];
	
	return cost;
}

struct Solution
	{
		int* sol;
		int cost;
	};
	
bool compareSols(Solution s1, Solution s2)
{
	return (s1.cost < s2.cost);
}



void crossPMX(Solution p1, Solution p2, Solution c1, Solution c2) // krzy¿owanie PMX
{
	int path1[rozmiar+1] = {0};
	int path2[rozmiar+1] = {0};
	bool swapped1[rozmiar] = {0};
	bool swapped2[rozmiar] = {0};
	
	int a, b; //cutpoints
	do
	{
		a = rand() % (rozmiar-1) +1;
		b = rand() % (rozmiar-1) +1;
	} while (a==b);
	
	if (a>b) // a pierwsze
	{
		int temp = a;
		a = b;
		b = temp;
	}
	
	for (int i=a;i<=b;i++)
	{
		c1.sol[i] = p2.sol[i];
		c2.sol[i] = p1.sol[i];
		swapped1[p2.sol[i]] = true;
		swapped2[p1.sol[i]] = true;
	}
	
	int x=1, y=1;
	for (int i=b+1;i<rozmiar;i++)
	{
		if(swapped1[p1.sol[i]] == false)
		{
			path1[x] = p1.sol[i];
			x++;
		}
		if(swapped2[p2.sol[i]] == false)
		{
			path2[y] = p2.sol[i];
			y++;
		}
	}
	for (int i=1;i<=b;i++)
	{
		if(swapped1[p1.sol[i]] == false)
		{
			path1[x] = p1.sol[i];
			x++;
		}
		if(swapped2[p2.sol[i]] == false)
		{
			path2[y] = p2.sol[i];
			y++;
		}
	}
	
	x=1, y=1;
	for (int i=b+1;i<rozmiar;i++)
	{
		c1.sol[i] = path1[x];
		c2.sol[i] = path2[y];
		x++;
		y++;
	}
	for (int i=1;i<a;i++)
	{
		c1.sol[i] = path1[x];
		c2.sol[i] = path2[y];
		x++;
		y++;
	}
}

void crossOX(Solution p1, Solution p2, Solution c1, Solution c2) // krzy¿owanie OX
{
	int mapping1[rozmiar] = {0};
	int mapping2[rozmiar] = {0};
	
	int a, b; //cutpoints
	do
	{
		a = rand() % (rozmiar-1) +1;
		b = rand() % (rozmiar-1) +1;
	} while (a==b);
	
	if (a>b) // a pierwsze
	{
		int temp = a;
		a = b;
		b = temp;
	}
	
	for (int i=a;i<=b;i++)
	{
		c1.sol[i] = p2.sol[i];
		c2.sol[i] = p1.sol[i];
		mapping1[p2.sol[i]] = p1.sol[i];
		mapping2[p1.sol[i]] = p2.sol[i];
	}
	
	for (int i=1;i<a;i++)
	{
		if (mapping1[p1.sol[i]]==0)
			c1.sol[i] = p1.sol[i];
		else
		{
			int x = p1.sol[i];
			while(mapping1[x] != 0) x = mapping1[x];
			c1.sol[i] = x;
		}	
		
		if (mapping2[p2.sol[i]]==0)
			c2.sol[i] = p2.sol[i];
		else
		{
			int x = p2.sol[i];
			while(mapping2[x] != 0) x = mapping2[x];
			c2.sol[i] = x;
		}
	}
	
	for (int i=b+1;i<rozmiar;i++)
	{
		if (mapping1[p1.sol[i]]==0)
			c1.sol[i] = p1.sol[i];
		else
		{
			int x = p1.sol[i];
			while(mapping1[x] != 0) x = mapping1[x];
			c1.sol[i] = x;
		}	
		
		if (mapping2[p2.sol[i]]==0)
			c2.sol[i] = p2.sol[i];
		else
		{
			int x = p2.sol[i];
			while(mapping2[x] != 0) x = mapping2[x];
			c2.sol[i] = x;
		}		
	}
}

void mutation(Solution s) // mutacja
{
	int x = rand() %101;
	if (x>=100) return; // prawdopodobieñstwo mutacji
	int a, b; //cutpoints
	
	if (zamiana == 0) //swap
	{
		do
		{
			a = rand() % (rozmiar-1) +1;
			b = rand() % (rozmiar-1) +1;
		} while (a==b);
		int tmp = s.sol[a];
		s.sol[a] = s.sol[b];
		s.sol[b] = tmp;
	}
	
	else if(zamiana == 1) //invert
	{
		do
		{
			a = rand() % (rozmiar-1) +1;
			b = rand() % (rozmiar-1) +1;
		} while (a==b);
		if(a>b)
		{
			int tmp = a;
			a = b;
			b = tmp;
		}
		int j=1;
		for (int i=a+1;i<=b;i++)
		{
			int tmp = s.sol[i];
			s.sol[i] = s.sol[b-j];
			s.sol[b-j] = tmp;
			j++;
		}
	}
	
	else if (zamiana == 2) //insert
	{
		do
		{
			a = rand() % (rozmiar-1) +1;
			b = rand() % (rozmiar-1) +1;
		} while (a==b);
		if(a>b)
		{
			int tmp = a;
			a = b;
			b = tmp;
		}
		int tmp = s.sol[b];
		for (int i=b;i>a;i--)
		{
			s.sol[i] = s.sol[i-1];
		}
		s.sol[a] = tmp;
	}
	else if (zamiana == 3) //next-swap
	{
		a = rand() % (rozmiar-2) +1;
		b = a+1;
		int tmp = s.sol[a];
		s.sol[a] = s.sol[b];
		s.sol[b] = tmp;
	}
}


int Genetic() // algorytm Generyczny
{
	chrono::steady_clock::time_point startControl;
	chrono::steady_clock::time_point endControl;
	long double czasControl;
	
	
	
	int gen_size = rozmiar * 10;
	
	//// ****[B1]****
	int start[rozmiar+1];
	start[0] = 0;
	start[rozmiar] = 0;
	for (int i=1;i<rozmiar;i++) start[i] = i;
	
	Solution populacja[(int)(gen_size*1.5)];
	
	for(int i=0;i<gen_size*1.5;i++)
	{
		populacja[i].sol = new int[rozmiar+1];
		for(int j=0;j<=rozmiar;j++)
		{
			populacja[i].sol[j] = start[j];
		}
		
		populacja[i].cost = obliczKoszt(populacja[i].sol);
		
		random_shuffle(start+1, start+rozmiar);	
	}
	
	
	for(int i=1;i<gen_size*1.5;i++)
	{
		random_shuffle(start+1, start+rozmiar);
		int t_cost = obliczKoszt(start);
		
		if (t_cost < populacja[i].cost)
		{
			for(int j=0;j<=rozmiar;j++)
			{
				populacja[i].sol[j] = start[j];
			}
			populacja[i].cost = t_cost;
		}	
	}
	//// ************
	
	//// ****[B2]****
	int best[rozmiar];
	int bestCost = INT_MAX;
		
	startControl = chrono::steady_clock::now();

	int licznik2 = 0;
	//// *************
	
	for(int i = 0; i < 50000; i++) // glowna petla algorytmu //// ****[B3]****
	{
		sort(populacja, populacja+(int)(gen_size*1.5), compareSols); // ocena (dlugosc sciezki), sortowanie populacji wzgledem oceny //// ****[B4]****
		
		
		if (populacja[0].cost < bestCost) // jesli najlepszy z populacji jest lepszy niz obecnie znany, zapisz //// ****[B5]****
		{	//// ****[B6]****
			licznik2 = 0;
			bestCost = populacja[0].cost;
			for(int j=0;j<=rozmiar;j++)
			{
				best[j] = populacja[0].sol[j];
			}
			//// *************
		}
		else 
		{
			licznik2++;
		}
		
		
		if (licznik2 > 20000) break; //// ****[B7]****
		
		
		// druga wersja selekcji
		/*
		for (int j= gen_size; j<(gen_size*1.5)-1; j+=2)
		{
			int a, b;
			if (j<((gen_size)/10)*14)
			{
			do
			{
				a = rand() %(gen_size/2);
				b = rand() %(gen_size/2);
			} while (a == b);
			}
			else
			{
			do
			{
				a = rand() %((gen_size/2));
				b = rand() %((gen_size/10)) + (gen_size/10)*6;
			} while (a == b);	
			}
			
			crossPMX(populacja[a], populacja[b], populacja[j], populacja[j+1]);
			mutation(populacja[j]);
			mutation(populacja[j+1]);

			populacja[j].cost = obliczKoszt(populacja[j].sol);
			populacja[j+1].cost = obliczKoszt(populacja[j+1].sol);
		}
		*/
	
		
		
		for (int j= gen_size; j<(gen_size*1.5)-1; j+=2) //// ****[B8]****
		{
			//// ****[B9]****
			int a, b;
			do
			{
				a = rand() %(gen_size/2);
				b = rand() %(gen_size/2);
			} while (a == b);	
			
			
			crossPMX(populacja[a], populacja[b], populacja[j], populacja[j+1]);
			mutation(populacja[j]);
			mutation(populacja[j+1]);

			populacja[j].cost = obliczKoszt(populacja[j].sol);
			populacja[j+1].cost = obliczKoszt(populacja[j+1].sol);
			//// *************
		}
		
		
		

		//// ****[B10]****
		endControl = chrono::steady_clock::now();
		czasControl = chrono::duration_cast<chrono::seconds>(endControl - startControl).count();
		if(czasControl>600) break;
		//// *************
	}
	//// ****[B11]****
	for(int k=0;k<=rozmiar;k++)
		sciezka[k] = best[k];
	
	return bestCost;
	//// *************
	
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
			cost = Genetic();
			end = chrono::steady_clock::now();
			czas = chrono::duration_cast<chrono::milliseconds>(end - start).count(); // czas dzia³ania
		
			cout<<"koszt: "<<cost<< "  blad: " <<( (double)cost/(double)optymalne ) -1 <<" czas: "<<czas<<"ms"<<endl;
	
			csv_out<<czas<<";"<<cost<<";"<< ( (double)cost/(double)optymalne ) -1 <<"\n";
		}
		usunMape();
	}
	

	
	
	return 0;
}


