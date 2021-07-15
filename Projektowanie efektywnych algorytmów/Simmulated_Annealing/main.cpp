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

int cooling = 0; //0 - geometryczny, 1 - Cauchy'ego
int swp = 0; // 0 - zamiana, 1 - odwrócenie, 2 - insert
int temperatura = 0; // 0 - zale¿na od œredniej roznicy, 1 - sta³a
int beg = 0; // 0 - najlepsze z rozmiar*rozmiar losowych, 1 - <1,2,3,4...>
int mnoznik = 15;

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

int obliczKoszt(int* path)
{
	int cost = 0;
	
	for (int i = 0;i<rozmiar;i++)
		cost += mapa[path[i]][path[i+1]];
	
	return cost;
}

int obliczT()
{
	int startowe[rozmiar+1] = {0};
	for (int i=1;i<rozmiar;i++)
		startowe[i]=i;
	int pierwsze = obliczKoszt(startowe);
	int roznice = 0;
	for (int i=0;i<rozmiar;i++)
	{
		random_shuffle(startowe+1, startowe+rozmiar);
		roznice += abs(pierwsze - obliczKoszt(startowe));	
	}
	return (roznice/rozmiar);
}



int Sim_Ann () // symulowane wy¿arzanie
{
	int bestGlobalCost;
	int bestGlobalPath[rozmiar+1];
	int bestCost=0;
	int bestPath[rozmiar+1];
	int tempStart[rozmiar+1]={0};
	
	/////	**[B1]**	/////
	double T_zero = 6* obliczT();
	double T;
	if (temperatura == 1)
		T = 500;
	else
		T = T_zero;
					
	bestPath[0]=0;
	bestPath[rozmiar]=0;
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
	/////	********	/////
	
	/////	**[B2]**	/////
	bestGlobalCost = bestCost;
	
	for (int i=0;i<=rozmiar;i++) bestGlobalPath[i] = bestPath[i];
	/////	********	/////
	
	for (int i = 0; i < 8000; i++) /////	**[B3]**
	{
		if (T < 0.001) break;	   /////	**[B3]**	
		
		for (int j = 0; j < (rozmiar*mnoznik); j++) /////	**[B4]**
		{	
			
			/////	**[B5]**	/////
			int a, b;
			do
			{
				a = rand() % (rozmiar-1) + 1;
				b = rand() % (rozmiar-1) + 1;
			}while(a==b);
			
			int tempPath[rozmiar+1];

			for (int k=0;k<=rozmiar;k++) tempPath[k] = bestPath[k];
			
			if (swp == 0) // zamiana
			{
				int temp = tempPath[a];
				tempPath[a] = tempPath[b];
				tempPath[b] = temp;
			} else if (swp == 1) // odwrócenie kawa³ka œcie¿ki
			{
				if (a>b) // a pierwsze
				{
					int temp = a;
					a = b;
					b = temp;
				}
				int c=0;
				for(int k=b;k>=a;k--)
				{
					tempPath[a+c] = bestPath[k];
					c++;
				}
			} else if (swp == 2) // insert
			{
				if (a>b) // a pierwsze
				{
					int temp = a;
					a = b;
					b = temp;
				}
				for (int k=0;k<a;k++) tempPath[k] = bestPath[k];
				tempPath[a] = bestPath[b];
				for (int k=a+1;k<=b;k++) tempPath[k] = bestPath[k-1];
				for (int k=b+1;k<=rozmiar;k++) tempPath[k] = bestPath[k];
			}
			
			int tempCost = obliczKoszt(tempPath);
			/////	********	/////
	
			/////	**[B6]**	/////
			if(tempCost < bestGlobalCost)
			{
				bestGlobalCost = tempCost;
				for (int k=0;k<=rozmiar;k++)
			 	{
			 		bestGlobalPath[k] = tempPath[k];
			 	}
			}
			/////	********	/////
			
			int roznica = tempCost - bestCost;
			
			
			if (roznica <= 0) /////		**[B7]**
			{
			 	bestCost = tempCost; 			///// **[B8]**
			 	for (int k=1;k<=rozmiar;k++) 	///// **[B8]**
			{
			 	bestPath[k] = tempPath[k];
			}
			} else if ( ((double)rand()/RAND_MAX) < (double)exp(-roznica/T) )	///// **[B9]**
			{
				bestCost = tempCost;			///// **[B8]**
			 	for (int k=0;k<=rozmiar;k++)	///// **[B8]**
			 	{
			 		bestPath[k] = tempPath[k];
			 	}
			}
			
			
		}
		/////	**[B10]**	/////
		if (cooling == 0)
			T = T * (double)0.998;
		else if (cooling == 1)
			T = T_zero/((double)(i+1)*3.5);
		/////	********	/////
	}
	
	/////	**[B11]**	/////
	for (int k=0;k<=rozmiar;k++)
	{
		sciezka[k] = bestGlobalPath[k];
	}
	return bestGlobalCost;
	/////	********	/////
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
	
	int temper, poczatkowe, chlodzenie, zamiana, mnoz;
	ini_in>>temper>>poczatkowe>>chlodzenie>>zamiana>>mnoz;
	
	temperatura = temper;
	beg = poczatkowe;
	cooling = chlodzenie;
	swp = zamiana;
	mnoznik = mnoz;
	
	if (temperatura!=0 && temperatura!=1) temperatura = 0;
	if (beg!=0 && beg!=1) beg = 0;
	if (cooling!=0 && cooling!=1) cooling = 0;
	if (swp!=0 && swp!=1 && swp!=2) swp = 0;
	if (mnoznik <= 0) mnoznik = 15;
	
	// dane z pliku .ini do pliku wynikowego
	csv_out<<temperatura<<";"<<beg<<";"<<cooling<<";"<<swp<<";"<<mnoznik<<"\n";
	
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
			cost = Sim_Ann();
			end = chrono::steady_clock::now();
			czas = chrono::duration_cast<chrono::milliseconds>(end - start).count(); // czas dzia³ania
		
			cout<<"koszt: "<<cost<< "  blad: " << (double)cost/(double)optymalne<<" czas: "<<czas<<"ms"<<endl;
	
			csv_out<<czas<<";"<<cost<<";"<<(double)cost/(double)optymalne<<"\n";
		}
		usunMape();
	}

	
	return 0;
}
