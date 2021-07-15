#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>

using namespace std;
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
	
	int n; 
	tsp_in>> n;
	int sciezka[n+1];
	for (int i=0;i<=n;i++) ini_in>>sciezka[i]; // wczytywanie optymalnej œcie¿ki z pliku konf.
	
	int mapa[n][n]; // macierz odleg³oœci
	int permutacje[n+1]={0}; // tablica kolejnoœci
	int best[n+1]; // najlepsza œcie¿ka
	
	
	for (int i=0; i<n;i++)
	{
		for (int j=0; j<n;j++)
			
			tsp_in>>mapa[i][j]; // wczytywanie macierzy z zadanego pliku
	}

 // zmienne punktów czasowych dla zegara
chrono::steady_clock::time_point start;
chrono::steady_clock::time_point end;
long double czas;

// dane z pliku .ini do pliku wynikowego
csv_out<<nazwa<<";"<<powtorzenia<<";"<<optymalne<<";";
for (int i=0;i<=n;i++) csv_out<<sciezka[i]<<";";
csv_out<<"\n";
	
for (int w=0;w<powtorzenia;w++) // powtarzanie algorytmu zadan¹ iloœæ razy
{
	start = chrono::steady_clock::now();

	//// WYGENERUJ ROZWI¥ZANIE STARTOWE S0 ***[B1]***

	for (int i=0;i<n;i++)
		permutacje[i]=i;
	
	int cost=0;
	
	//// PRZYJMIJ ROZWI¥ZANIE STARTOWE JAKO OBECNIE NAJLEPSZE ***[B2]***
	for (int i=0;i<n;i++)
		cost+=mapa[permutacje[i]][permutacje[i+1]];
		
	for (int i=0;i<=n;i++) 
		best[i]=permutacje[i];
	
	
	//// CZY ISTNIEJE NIESPRAWDZONA ŒCIE¯KA ***[B3]***	
	while(next_permutation(permutacje+1, permutacje+n)) // sprawdzanie wszelkich mo¿liwych przejœæ przez punkty (wychodz¹c z zerowego) 
	{
		int costTemp=0;
		
		//// OBLICZENIE KOSZTU NIESPRAWDZONEJ ŒCIE¯KI ***[B4]***
		for (int i=0;i<n;i++)
		{
			costTemp+=mapa[permutacje[i]][permutacje[i+1]];
		}
		
		/// PORÓWNANIE Z OBECNIE NAJLEPSZ¥ ***[B5]***
		if (costTemp<cost)
		{
			//// ZAPAMIÊTANIE JEŒLI LEPSZA ***[B6]***
			cost=costTemp;
			for (int i=0;i<n+1;i++) best[i]=permutacje[i];	
		}
	}
	
	end = chrono::steady_clock::now();
	czas = chrono::duration_cast<chrono::microseconds>(end - start).count(); // czas dzia³ania 
	
	csv_out<<czas<<"\n";
	
	//WYPISANIE NAJLEPSZEGO ROZWI¥ZANIA ***[B7]***
	if (w == powtorzenia-1) // po ostatnim powtórzeniu
	{
		cout<<cost<<endl;
		csv_out<<cost<<";";
		for (int i=0;i<n+1;i++) 
		{
			cout<<best[i]<<"  ";
			csv_out<<best[i]<<";";
		}
		csv_out<<"\n";
		cout<<endl;	
	}
	
}
}
return 0;
}
