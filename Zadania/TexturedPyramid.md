# Teksturowanie piramidy


W tym ćwiczeniu nauczymy się nakładać tekstury na obiekty. Jako bazę do tego ćwiczenia proszę wybrać ostatnie zrobione ćwiczenie z piramidką przed ćwiczeniem Animacja Piramidy czyli Zoom lub Ruch Kamery. 

## Klasa `Pyramid`

Skorzystamy jednak z klasy `Pyramid` stworzonej w zadaniu z animacją. Po stworzeniu nowego  katalogu  dla obecnego zadania proszę przekopiować do niego pliki `pyramid.h` i `pyramid.cpp` z zadania z animacją. Jeśli Państwo nie zrobili tego zadania, to proszę stworzyć te pliki zgodnie z opisem w pierwszych punktach [zadania z animacją](https://github.com/Dydaktyka/BaseGraphicProject/blob/main/Zadania/PyramidAnimation.md). 

Po przekopiowaniu/stworzeniu  tych plików proszę wykorzystać klasę `Pyramid` do rysowania piramidki, tak jako to było opisane w ćwiczeniu z animacją i usunać zbędny kod z metody `init`: 

 1. W pliku `CMakeLists.txt` w  katalogu zadania w poleceniu `add_executable` dodajemy nazwy plików `pyramid.h` i `pyramid.cpp`. 
 1. Dodajemy wskaźnik do obiektu klasy `Pyramid` jako  publiczne pole klasy `SimpleShapeApplication`. W metodzie `init` tej klasy poleceniem `new` tworzymy nowy obiekt tej klasy i przypisujemy jego wskaźnik do tego nowego pola. 
 1. W metodzie `frame` zastępujemy polecenia rysowania piramidy wywołaniem metody `draw` obiektu `Pyramid`. 
 1. Teraz usuwamy cały niepotrzebny kod z  metody `init`. 


## Ładowanie obrazka tekstury 
 
Jako tekstury użyjemy dostarczonego pliku `multicolor.png`. W katalogu zadania proszę stworzyć podkatalog `Textures` i zapisać tam ten plik. 

Teksturę stworzymy w konstruktorze klasy `Pyramid`. Do wczytania obrazka posłuży nam biblioteka `stb_image`. Potrzebne pliki znajdują się w katalogu `src/3rdParty/src/stb`. W celu ich wykorzystania należy dodać w pliku `pyramid.cpp` linijki
```
#define STB_IMAGE_IMPLEMENTATION
#include "3rdParty/src/stb/stb_image.h"
```

Do wczytywania obrazów służy funkcja `stbi_load`
```
int width, height, n_channels; 
uint8_t  *data = stbi_load(filename, &width, &height, &n_channels, 0);
```
Proszę zwrócić uwagę, że parametry drugi, trzeci i czwarty to są parametry _wyjściowe_. Powinny tam być przekazane _wskaźniki_ na zmienne w ktorych zwracane wartości zostaną zapisane. 
Parametr `filename` to nazwa pliku. Dla wygody makro `PROJECT_DIR` zawiera ścieżkę do katalogu zadania. Możemy więc uzyskać pełną ścieżkę do pliku z teksturą za pomocą 
```
auto texture_filename = std::string(PROJECT_DIR) + "/Textures/multicolor.png";
```
Należy jednak pamiętać, że funkcja `stbi_load` oczekuje jako pierwszego parametru stałej znakowej `C`, czyli `char *`, a nie `std::string`. W celu konwersji możemy wykorzystać metodę `c_str()` z klasy `std::string`. 

Proszę umieścić kod wczytujący obrazek w konstruktorze klasy `Pyramid`. Po udanym wywołaniu  funkcji `stbi_load` zwrócony zostaje niezerowy (non null) wskaźnik do danych, a zmienne `width`, `height` i `n_channels` powinny zawierać odczytane wartości. Proszę dodać kod który sprawdza czy zwracany wskaźnik jest niezerowy i jeśli tak to wypisuje rozmiary wczytanej tekstury i komunikat o błędzie w przeciwnym przypadku. 

Następnie dodajemy w klasie `Pyramid` pole które będzie zawierało teksturę:
```
GLuint diffuse_texture_; 
```

W konstruktorze należy je zainicjaliziwać za pomocą funkcji `glGenTextures`. 

Potem bindujemy tą teksturę do targetu `GL_TEXTURE_2D`
```
glBindTexture(GL_TEXTURE_2D, diffuse_texture_);
```
i ładujemy do niej obrazek
```
glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, width, height,0, GL_RGB, GL_UNSIGNED_BYTE,data);
```
Na koniec ustalamy rodzaj  interpolacji
```
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
```
I możemy już ją odbindować
```
glBindTexture(GL_TEXTURE_2D, 0);
```

## Współrzędne tekstury

Teraz musimy  określić położenie każdej ze ścianek piramidy na teksturze. W tym celu musimy przypisać jej wierzchołkom współrzędne tekstury. Wartości tych współrzędnych znajdują sie w pliku `UV.png`. Proszę zauważyć, że będziemy potrzebowali tylko 8 wierzchołków, a nie 16 jak do tej pory. Współrzędne tekstury wpiszemy zamiast kolorów wierzchołków. To znaczy wartośći R i G będą teraz odpowiadały wartościom U i V, a wartość B należy usunąć. 

Proszę zacząć od usunięcia niepotrzebnych wierzchołków i odpowiedniego zmodyfikowania bufora indeksów. Powinno wystarczyć 8 wierzchołków: cztery wierzchołki podstawy i cztery razy powtórzony "czubek" piramidy. Proszę pamiętać o zachowaniu odpowiedniej orientacji. 


Teraz proszę usunać wartości B z kolorów wierzchołków i dokonać odpowiedniej modyfikacji w poleceniach `glVertexAttribPointer`. Jeśli wszystko zostało zrobione poprawanie, piramidka powinna dalej się wyświetlać tak samo, tylko ze zmienionymi kolorami. Dzieje się tak dlatego, że OpenGL automatycznie uzupełnia atrybuty wierzchołków do 4 wartości. W tym wypadku wartość B koloru zostanie uzupełniona jako zerowa. 

W shaderach proszę zmienić nazwe oraz typ atrybutu `a_vertex_color` i zmiennej `vertex_color`, tak żeby odpowiadała nowemu znaczeniu jako współrzędnych tekstury. Na razie będziemy ją dalej traktowali jako kolor. Ale ponieważ  zmieniliśmy typ zmiennych na `vec2` trzeba odpowiednio zmodyfikować przypisywanie koloru do `vFragColor` w shaderze fragmentów. 

Teraz proszę wpisać poprawne wartości współrzędnych tekstury zgodnie z mapą UV. 

## Teksturowanie

I w kóncu wykorzystamy teksturę do nadania kolorów pikselom. W tym celu w shaderze fragmentów dodajemy zmienna uniform
```
uniform sampler2D diffuse_map; 
```
Po stronie CPU w metodzie `init` przypiszemy tej zmiennej wartość zero co oznacza, że sampler będzie korzystał z jednostki teksturującej (texture unit) z indeksem zero: 
```
auto  u_diffuse_map_location = glGetUniformLocation(program,"diffuse_map");
    if(u_diffuse_map_location==-1) {
        std::cerr<<"Cannot find uniform diffuse_map\n";
    } else {
        glUniform1ui(u_diffuse_map_location,0); 
    }
```
Ważne jest, aby polecenie `glUniform1ui` zostało wydane po poleceniu `glUseProgram`. 
W funkcji `draw` klasy `Pyramid` dodajemy polecenia bindowania tekstury  do jednstki teksturującej zero:
```
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D,diffuse_texture_); 
```
Po wywołaniu funkcji `glDrawElements` możemy teksturę odbindować. 

Po wykonaniu programu otrzymamy komunikat o nie znalezieniu zmiennej uniform. To jest w porządku. Na razie nie korzystamy jeszcze z tego samplera w shaderze i jest on nieaktywny. 

I na koniec modyfikujemy kod szadera fragmentów, tak aby kolor fragmentu był pobierany z  tekstury `color` za pomocą funkcji `texture`:
```
vFragColor = texture(color, texture_coords); 
```
Teraz już powinniśmy zobaczyć oteksturowaną piramidę :) 






 

