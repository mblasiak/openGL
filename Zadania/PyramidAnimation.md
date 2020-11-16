# Animacja piramidy 

## Klasa `Pyramid`

W poprzednich zadaniach  zajmowiliśmy się transformacjami projekcji (Zmiana rozmiarów okna i Zoom)  oraz widoku/kamery (Ruch kamery). W tym ćwiczeniu zajmiemy się transformacją modelu. 

Zaczniemy jednak od "wyczyszczenia kodu". Jeśli Państwo do tej pory mają w kodzie  blok interfejsu `Modifiers` to proszę go usunąć zarówno z kodu szadera jak i kodu C++. 
  
  W kolejnym kroku  dokonamy refactoryzacji przenosząc funkcjonalność  związaną z modelem piramidy do osobnej klasy `Pyramid`. W tym celu tworzymy nowy plik nagłówkowy `pyramid.h`. W tym pliku tworzymy klasę `Pyramid`:
  
  
```
#pragma once

#include "glad/glad.h"


class Piramid  {
public:
    Piramid();
    ~Pyramid();
    void draw();
private:
};
```  

W tej klasie będziemy  potrzebowali bufora wierzchołków i indeksów oraz obiektu bufora wierzchołków (Vertex Array Object). Proszę dodać odpowiednie pola do klasy:

```
GLuint vao_;
GLuint buffer_[2];
``` 
  
Teraz tworzymy plik `pyramid.cpp` w którym implementujemy metody klasy `Pyramid`
```  
#include "pyramid.h"
#include <vector>

Pyramid::Piramid() {
 /*
     Tu dodajemy kod tworzący bufory indeksów i wierzchołków oraz obiekt VAO,
     Potem ładujemy dane wierzchołków i indeksow do buforów  i  podłaczamy je do atrubutów w szaderze
*/
   
}  

Pyramid::~Pyramid() {
    //Tu usuwamy VAO i bufory
} 
 
void Pyramid::draw() {
    //Tu wywolujemy plecenie glDrawElements
}  
``` 
Proszę zaimplementować powyższe metody, w dużej części polega to na skopiowaniu kodu z metody `init` w klasie `SimpleShapeApplication`. 

Teraz dodajemy wskaźnik do obiektu klasy `Pyramid` jako  publiczne pole klasy `SimpleShapeApplication`. W metodzie `init` tej klasy poleceniem `new` tworzyny nowy obiekt tej klasy i przypisujemy jego wskaźnik do tego nowego pola. 

W metodzie `frame` zastępujemy polecenia rysowania piramidy wywołaniem metoidy `draw` obiektu `Pyramid`. 

Teraz usuwamy cały niepotrzebny kod z  metody `init`. 


## Animacja piramidy 

###  Pomiar czasu

Żeby coś animować będziemy potrzebowali mierzyć upływ czasu. W tym celu skorzystamy z modułu `chrono` języka C++. Zaczniemy od dodania w klasie `SimpleShapeApplication` pola:
```
 std::chrono::steady_clock::time_point start_;
``` 
Następnie pod koniec metody `init` incjalizujemy je poleceniem
```
start_ = std::chrono::steady_clock::now();
```
W metodzie `frame` dodajemy
```
auto now = std::chrono::steady_clock::now();
auto elapsed_time = std::chrono::duration_cast<std::chrono::duration<float>>(now - start_).count();
```
Zmienna `elapsed_time` będzie teraz zawierała czas w sekundach jaki upłynął od początku uruchomienia aplikacji. 


### Obrót piramidy 

Zaczniemy od obracania kamerą wokół jej osi pionowej. Założmy, że  jeden pełny obrót powinien zająć 4s. Dodajmy więc pole do klas `SimpleShapeApplication`
```
float rotation_period; 
``` 
i w metodzie `init` nadajmy mu wartość 4.0.  To oznacza, że po czasie `elapsed_time` piramida obroci się o kąt równy
```
auto rotation_angle = 2.0f*glm::pi<float>*elapsed_time/rotation_period
```

W metodzie `frame` dodajemy macierz  `R` która dokona obrotu piramidy wokół jej osi pionowej o kąt `rotation_angle`. Wykorzystamy w tym celu  funkcję 
```
glm::rotation(glm::mat4(1.0f), rotation_angle,axis)
```
gdzie `axis` to kierunek osi pionowej piramidy, zwykle oś 'z' lub 'y'. Należy jednak zauwważyć, że ta funkcja zwraca macierz obrotu wokół danej osi i punktu (0,0,0). Jeśli więc środek piramidy znajduje się nad punktem (0,0,0) to możemy po prostu obrócić piramidę za pomocą tej macierzy. Jeśli nie, to musimy najpierw przesunąć piramidkę do środka układu, obrócić, a potem przesunąć z powrotem.  

Przesunięcia możemy dokonać za pomocą macierzy  zwracanej przez funkcję `glm::translate`.  

### Ruch orbitalny

Teraz wyślemy piramidkę na orbitę. Piramida będzie się poruszać po elipsie o osiach `a=10` i `b = 8` z okresem obiegu 20s. W tym celu  po opisanym wyżej obrocie musimy przesunąć piramidę na odpowiedni punkt na orbicie. Ten punkt będzie  posiadał współrzędne (x,y,0) gdzie 
```
orbital_rotation_period = 20.0f;
orbital_rotation_angle = 2.0f*glm::pi<float>()*elapsed_time/orbital_rotation_period;
x = a*cos(orbital_rotation_angle);
y = b*sin(orbital_rotation_angle); 
```
Oczywicie jeśli Państwa piramidka jest zorientowana w kierunku `y` do góry, to należy w powyższym kodzie zmienić współrzędną `y` na `z`.  Przesunięcia dokonany za pomocą macierzy `O`  (jak "orbital"). 

Końcowa macierz modelu jest iloczynem (złożeniem) macierzy obrotu i translacji
```
auto M = O*R; 
```


Proszę zaimplementować ten ruch orbitalny. Proszę nie zapomnieć o odpowiednim oddaleniu kamery, żeby móc objąć całą orbitę! 


### Księżyc 

Teraz dodamy księżyc do naszej "piramidalnej planety Ziemia". Księżyc będzię drugą piramidką tyle, że dwa razy mniejszą niż Ziemia. Księżyc będzie krążył po orbicie kołowej o promieniu `r_moon = 3.0f` wokół piramidki z okresem obiegu orbitalnego równym okresowi obrotu wokół własnej osi i wynoszącym 10s. Księżyc będzie krążył w tej samej płaszczyźnie co Ziemia i jego oś obrotu własnego będzie równoległa do osi obrotu ziemi. 

W celu implementacji tego ruchu  należy policzyć macierze  `R_moon`  i `O_moon` odpowiadające za obrót i ruch ksieżyca  dokładnie tak samo jak dla Ziemi. Z tym, że te macierz będą teraz określały położenie księżyca względem Ziemi. W zwiazku z tym trzeba je będzie złożyć z macierzą `O` dla Ziemi. Czyli po narysowaniu Ziemi, 
należy przesłać do szaderów za pomocą buforów uniform nową macierz PVM,
```
P*V*O*O_moon*R*S
```
przy czym `S` to macierz przeskalowywująca księżyc o połowę. Potem rysujemy piramidkę po raz kolejny. Proszę zauważyć, że nie składamy ruchu księżyca z obrotowym ruchem ziemi. 


### Satelita

Proszę dodać satelitę obiegającego Ziemię bo kołowej orbicie o promieniu 1.5 o okresie obiegu 2s. Orbita powinna leżeć w płaszczyźnie x-z (jeśli z to kierunek pionowy). 
Oznacza to, że oś obrotu orbitalnego to (0,1,0). Satelita powinien obracać się wokół swojej osi, też (0,1,0) z tym samym okresem.    

Proszę tak  ustawić kamerę żeby pokazywała widok "z góry"  znad punktu (0,0,0) obejmujący całą orbitę, tak aby kierunek 'y' był   w kierunku do góry  ekranu. 

