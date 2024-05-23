#include <cstdlib>
#include <chrono>
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>
#include <random>

using namespace std;

#define INF 1e7  //definiowanie "nieskonczonosci" jako bardzo duzej liczby
int max_number = 0; //zmienna globalna do przechowywania maksymalnej liczny

struct Edge {// Definicja struktury 'Edge', reprezentującej krawędź grafu
    int destination;
    int weight;
}; 

void printPath(vector<int>& predecessors, int destination) {// Funkcja do wydrukowania ścieżki w grafie
    if (predecessors[destination] == -1) {// Sprawdzenie, czy istnieje poprzednik dla danego węzła
        cout << destination;// wyswietlanie docelowego
        return;
    }
    printPath(predecessors, predecessors[destination]); // Rekurencyjne wywołanie poprzednika
    cout << " -> " << destination;// wyswietlanie aktualnego węzla
}

void dijkstraMatrix(vector<vector<int>>& graph, int source, int V, int destination) {// Implementacja algorytmu Dijkstry dla macierzy sąsiedztwa
    vector<int> distances(V, INF); // Inicjalizacja wektora odległości wartościami 'INF'
    vector<bool> visited(V, false);
    vector<int> predecessors(V, -1);
    distances[source] = 0;  // Ustawienie odległości od źródła na 0

    for (int count = 0; count < V - 1; count++) {// Pętla główna algorytmu, iterująca przez węzły grafu
        int minDistance = INF;
        int minIndex = -1;

        for (int v = 0; v < V; v++) {// Wewnętrzna pętla do znalezienia węzła z minimalną odległością, który nie został jeszcze odwiedzony
            if (!visited[v] && distances[v] <= minDistance) {
                minDistance = distances[v];
                minIndex = v;
            }
        }

        if (minIndex == -1) continue; // Jeśli nie znaleziono węzła, kontynuacja pętli

        visited[minIndex] = true;// Oznaczenie węzła jako odwiedzonego

        for (int v = 0; v < V; v++) {       // Pętla aktualizująca odległości sąsiednich węzłow
            if (!visited[v] && graph[minIndex][v] && distances[minIndex] != INF &&  // Aktualizacja odległości, jeśli nowa ścieżka jest krótsza.
                distances[minIndex] + graph[minIndex][v] < distances[v]) {
                distances[v] = distances[minIndex] + graph[minIndex][v];
                predecessors[v] = minIndex;
            }
        }
    }


    if (distances[destination] != INF) {  // Wypisanie wyniku lub informacji, że ścieżka nie istnieje.
        cout << "Najkrotsza droga z " << source << " do " << destination << " wynosi: ";
        printPath(predecessors, destination);
        cout << " z odlegloscia: " << distances[destination] << endl;
    }
    else {
        cout << "Nie ma sciezki z " << source << " do " << destination << endl;
    }
}


void dijkstraList(vector<vector<Edge>>& graph, int source, int V, int destination) {    // Implementacja algorytmu Dijkstry dla listy
    vector<int> distances(V, INF);
    vector<bool> visited(V, false);
    vector<int> predecessors(V, -1);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> priorityQueue;// Kolejka priorytetowa do przechowywania par odległości i węzłów

    distances[source] = 0;
    priorityQueue.push({ 0, source });

    while (!priorityQueue.empty()) {// Pętla wykonuje się, dopóki kolejka nie jest pusta
        int u = priorityQueue.top().second;// Pobranie węzła o najmniejszej odległości
        priorityQueue.pop();// Usunięcie tego węzła z kolejki

        if (visited[u])
            continue;

        visited[u] = true;

        for (auto& edge : graph[u]) { // Pętla po wszystkich krawędziach wychodzących z węzła 'u'
            int v = edge.destination;
            int weight = edge.weight;

            if (!visited[v] && distances[u] != INF && distances[u] + weight < distances[v]) { // Aktualizacja odległości, jeśli nowa ścieżka jest krótsza
                distances[v] = distances[u] + weight;
                priorityQueue.push({ distances[v], v });
                predecessors[v] = u;
            }
        }
    }
    if (INF != distances[destination]) { // Wypisanie wyniku lub informacji, że ścieżka nie istnieje
        cout << "najkrotsza droga z " << source << " to " << destination << endl;
        printPath(predecessors, destination);
        cout << " : " << distances[destination] << endl;
    }
    else {
        cout << "Brak sciezki z " << source << " do " << destination << endl;
    }
}

void createGraphFromFile(vector<vector<int>>& matrixGraph, vector<vector<Edge>>& listGraph) { //petla do wczytywania graphu z pliku
    ifstream file("data.txt");
    if (!file) {// Sprawdzenie, czy plik został poprawnie otwarty
        cout << "Failed to open file." << endl;
        return;
    }

    int E, V;
    file >> E >> V; // Wczytanie liczby krawędzi i wierzchołków z pliku
    max_number = V;// Ustawienie maksymalnej liczby wierzchołków
    matrixGraph.clear();// Wyczyszczenie obecnej macierzy grafu
    listGraph.clear(); // Wyczyszczenie obecnej listy grafu
    matrixGraph.resize(V, vector<int>(V, 0));// Inicjalizacja macierzy sąsiedztwa
    listGraph.resize(V);// Inicjalizacja listy sąsiedztwa

    for (int i = 0; i < E; i++) {// Pętla do wczytywania krawędzi z pliku
        int start, end, weight;
        file >> start >> end >> weight;// Wczytanie krawędzi z pliku
        matrixGraph[start][end] = weight;// Dodanie krawędzi do macierzy sąsiedztwa
        listGraph[start].push_back({ end, weight });// Dodanie krawędzi do listy sąsiedztwa
    }

    file.close();
}

void generateRandomGraph(vector<vector<int>>& matrixGraph, vector<vector<Edge>>& listGraph, int V, double density) { //funkcja do generowania losowych graphów
    matrixGraph.clear();// Wyczyszczenie obecnej macierzy grafu
    listGraph.clear(); // Wyczyszczenie obecnej listy grafu

    matrixGraph.resize(V, vector<int>(V, 0));// Inicjalizacja macierzy sąsiedztwa
    listGraph.resize(V);// Inicjalizacja listy sąsiedztwa

    int maxEdges = V * (V - 1); // Maksymalna liczba możliwych krawędzi w grafie.
    int numberOfEdges = static_cast<int>(density * maxEdges);// Obliczenie liczby krawędzi na podstawie gęstości grafu

    random_device rd;  // Inicjalizacja generatora liczb losowych
    mt19937 gen(rd()); // Inicjalizacja generatora liczb losowych Mersenne Twister
    uniform_int_distribution<> distrib(0, V - 1);// Dystrybucja do losowania wierzchołków
    uniform_int_distribution<> weightDistrib(1, 10);// Dystrybucja do losowania wag krawędzi

    while (numberOfEdges > 0) {// Pętla do tworzenia krawędzi, dopóki nie osiągnięto oczekiwanej liczby
        int start = distrib(gen);
        int end = distrib(gen);
        if (start != end && matrixGraph[start][end] == 0) {// Sprawdzenie, czy krawędź już nie istnieje i czy nie tworzy pętli
            int weight = weightDistrib(gen);
            matrixGraph[start][end] = weight;
            listGraph[start].push_back({ end, weight });
            numberOfEdges--;

            // cout << "Dodano krawedz: " << start << " -> " << end << " [waga: " << weight << "]" << endl;
        }
    }
}


int main() {
    vector<vector<int>> matrixGraph;
    vector<vector<Edge>> listGraph;

    int V; int source, destination;
    double density;
    int choice;
    auto start = chrono::high_resolution_clock::now();  auto end = chrono::high_resolution_clock::now();  auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();
    
    
   while (true) {// Pętla głównego menu programu
        cout << "Wybierz opcje" << endl << "1. Stworz graph z pliku" << endl << "2.Stworz losowy graph" << endl << "3. Uruchom algorytm Djisktry" << endl << "4. koniec" << endl;
        cin >> choice;

        switch (choice) {
        case 1:
            createGraphFromFile(matrixGraph, listGraph);
            break;

        case 2:
            cout << "Podaj Ilosc wierzchołków: ";
            cin >> V;
            cout << "Wpisz gęstość (0.x): ";
            cin >> density;
            generateRandomGraph(matrixGraph, listGraph, V, density);
            break;

        case 3:
        {
            ofstream resultsFile("wyniki.txt", ios::app);// Otwarcie pliku do zapisywania wyników

            if (matrixGraph.empty() || listGraph.empty()) {// Sprawdzenie, czy graf został wczytany
                resultsFile << "Graph nie zostal wczytanie" << endl;
                break;
            }
            cout << "Podaj zrodlo" << endl;
            cin >> source;
            cout << "podaj cel" << endl;
            cin >> destination;


            start = chrono::high_resolution_clock::now();// Rozpoczęcie pomiaru czasu.
            dijkstraMatrix(matrixGraph, source, matrixGraph.size(), destination);// Uruchomienie algorytmu Dijkstry
            end = chrono::high_resolution_clock::now(); // Zakończenie pomiaru czasu
            duration = chrono::duration_cast<chrono::microseconds>(end - start).count();// Obliczenie czasu wykonania
            resultsFile << "Czas macierzowy: " << duration << " microseconds" << endl;

            start = chrono::high_resolution_clock::now();
            dijkstraList(listGraph, source, listGraph.size(), destination);
            end = chrono::high_resolution_clock::now();
            duration = chrono::duration_cast<chrono::microseconds>(end - start).count();
            resultsFile << "Czas listowy: " << duration << " microseconds" << endl;
        }
        break;


        case 4:
            cout << "Papa" << endl;
            return 0;

        default:
            cout << "Blad" << endl;
        }
    }
  
    return 0;
}
