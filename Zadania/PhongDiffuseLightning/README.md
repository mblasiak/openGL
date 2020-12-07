# Oświetlenie dyfuzyjne (Model Phonga)

W tym ćwiczeniu zaimplementujemy  część dyfuzyjną oświetlenia Phonga. 

Jako punkt wyjścia weźmiemy zadanie z oteksturowaną piramidką. Proszę skopiować to zadanie do nowego katalogu.  

## Klasa `Quad`

Zaczniemy od stworzenia nowej klasy reprezentującej kwadrat. W tym celu proszę skopiować pliki `piramid.h` i `piramid.cpp` nazywając je `quad.h` i `quad.cpp`. W tych plikach należy zamienić nazwę klasy `Piramid` na klasę `Quad`. 

Następnie proszę dokonać takich zmian w konstruktorze klasy `Quad`, żeby reprezentowała kwadrat o boku o długości 2, leżący w płaszczyźnie poziomej o środku w środku układu współrzędnych. Proszę nadać wierzchołkom takie współrzędne tekstury, żeby teksturą pokryć cały kwadrat. 

Teraz możemy podmienić pole ze wskaźnikiem do piramidki w klasie `SimpleShapeApplication` i podstawić tam kwadrat. Powinniśmy zobaczyć oteksturowany kwadrat na ekranie.  Proszę też podmienić plik z teksturą na plik `silver.png`. Proszę jednak zauważyć, że ten plik zawiera cztery kanały: RGBA. Należy to uwzględnić w poleceniu `glTexImage`.  


Przy okazji naprawimy mój błąd z poprzednich zadań :( Po mojej „poprawie” kod w poprzednich zadaniach korzysta z inteligentnych wskaźników do przechowywania wskaźnika na piramidę. Powoduje to, że ich konstruktor jest wywoływany automatycznie kiedy niszczony jest obiekt klasy `SimpleShapeApplication`. Problem w tym, że jak mogą Państwo sprawdzić w pliku `src/Application/application.cpp` dzieje się to już po wywołaniu funkcji `glfwTerminate` w metodzie `run`. A to oznacza, że nie ma już kontekstu OpenGL-a i działanie takich funkcji jak `glDeleteBuffer` czy `glDeleteTextures` jest nieokreślone. 

Dlatego w tej klasie (`Application`) zdefiniowana jest metoda `cleanup` wywoływana tuż przed wywołaniem `glfwTerminate`. Tą metodę  można nadpisać w klasach dziedziczących i   wywołać w niej destruktor  obiektu `Quad`:
```c++
void SimpleShapeApplication::cleanup() {
    delete quad;
}
```

## Oświetlenie 

Teraz  przejdziemy do implementowania oświetlenia. 

### Dodanie normalnych 

Pierwszą rzeczą jaką zrobimy to dodanie wektorów normalnych do bufora wierzchołków. W konstruktorze klasy `Quad` dopisujemy odpowiednie współrzędne w buforze wierzchołków za współrzędnymi tekstury. Czyli bufor wierzchołków powinien wyglądac następująco:
```
x,y,z, u,v,n_x, n_y, n_z, 
...
```
Wektory normalne powinny być identyczne w każdym wierzchołku i prostopadłe do płaszczyzny kwadratu. 
Po dodaniu dodatkowych współrzędnych musimy odpowiednio zmodyfikować polecenia `glVertexAttribPointer`, tak aby kwadrat dalej się wyświetlał na ekranie. 

Teraz w shaderze wierzchołków dodajemy odpowiadający normalnym atrybut wejściowy:
```glsl
layout(location=2) in  vec3 a_vertex_normal; 
```
a w konstruktorze klasy `Quad` odpowiednie polecenia `glVertexAttribPointer` i `glEnableVertexAttribArray`. 

### Macierze M*V, P  i macierz normalnych. 

Teraz musimy zdecydować w jakim układzie będziemy dokonywali naszych obliczeń. Proponuję robić to w układzie widoku/kamery (view space). 
To znaczy, że musimy do tego układu przekształcić zarówno współrzędne wierzchołka, jak i wektory normalne. Do tego celu służy macierz `V*M`. Przy czym jak Państwo mam, nadzieję pamiętają z wykładu :) do przekształcenia normalnych potrzebna jest macierz 
```text
N = R^(-1)^T
```
gdzie `R` jest to "lewy górny” fragment 3x3 macierzy `V*M`. Do shadera wierzchołków prześlemy więc trzy macierze: `P`, `V*M` i `N`. 

W shaderze wierchołków modyfikujmey w tym celu blok uniform `Transformations` na 
```glsl
layout(std140)  uniform Transformations {
    mat4 P;
    mat4 VM;
    mat3 N;
};
```
Odpowiednio też musimy zmienić kod przypisujący wartość do zmiennej `gl_Position`. 

Po stronie CPU w metodzie `init` musimy zmienić rozmiar alokowanego bufora uniform dla bloku `Transformations`. 
Proszę jednak pamiętać o standardzie `std140`.  Zgodnie z nim wektory trzyelementowe wyrównywane są tak jak wektory czteroelementowe. To oznacza, że macierz 3x3 zajmuje 3x4 float-y, a nie 3x3. 

Po zmianie rozmiarów bufora proszę zmienić kod w metodzie `frame` aby przesyłać do niego macierze `P` i `V*M` (oczywiście macierz `M` jest opcjonalna). 

Jeśli wszystko poszło dobrze, to na ekranie powinniśmy znów zobaczyć nasz kwadrat. 

#### Macierz normalnych 

Macierz normalnych `N` możemy usyskać poleceniami
```c++
auto R = glm::mat3(VM);
auto N = glm::transpose(glm::inverse(R));
```

Przesłanie jej do shadera wymaga pewnej uwagi. Macierz normalnych, tak jak wszystkie macierze w OpenGL-u jest ułożona w pamięci kolumnami:
```text
0 3 6
1 4 7
2 5 8
```
Z powodu standardu `std140` w buforze uniform powinna być ułożona jako:
```text
0 1 2 * 3 4 5 * 6 7 8 *
```
gdzie `*` oznacza dowolną wartość. To oznacza, że musimy do jej przesłania użyć trzech  osobnych poleceń `glBufferSubData`. Proszę zaimplementować to w metodzie `frame`. Adres kolumny `col` macierzy możemy uzyskać jako `&N[col]`. 

  
## Przekazywanie zmiennych do shadera fragmentów. 


Potrzebne zmienne w przekształcone do przestrzeni widoku musimy przekazać do shadera fragmentów, gdzie będą wykonywane obliczenia oświetlenia. W tym celu dodajemy w shaderze wierzchołków zmienne wyjściowe
```glsl
out vec3 vertex_normal_in_vs;
out vec3 vertex_position_in_vs; 
```
i odpowiadające im zmienne wejściowe w shaderze fragementów. 

W shaderze wierzchołków nadajemy im odpowiednie wartości:
```glsl
vertex_position_in_vs = (VM*a_vertex_position).xyz; 
```
W przypadku wektora normalnego bezpiecznie jest go jeszcze unormować po przekształceniu:
```glsl
vertex_normal_in_vs = normalize(N*a_vertex_normal);
```

## Światło

###  Zródło światła

No i w końcu potrzebne jest źródło światła. Jego własności przekażemy w kolejnym bloku uniform, który dodamy w shaderze fragmentów:
```glsl
layout(std140) uniform Light {
    vec3 position_in_vs;
    vec3 color;
    vec3 a; 
} light;
```
Proszę zwrócić uwagę na dodatkowy element, jakim jest tu nazwa _instancji_ `light` dodana po deklaracji bloku. Dzięki temu w shaderze będziemy się odwoływać do zmiennych z bloku przez nazwy kwalifikowane np. `light.position_in_vs`. 

Po stronie CPU w metodzie `init` proszę stworzyć i zaalokować odpowiedni bufor uniform. Proszę znów pamiętać, że standard `std140` wymaga wyrównania do wektorów czterowymiarowych. 

Położenie światła najlepiej określić we współrzędnych świata, więc przed przesłaniem go do shadera należy przekształcić je do współrzędnych kamery za pomocą macierzy `V`. 

Światło po stronie  CPU zdefiniujemy jako pole `light_` typu:
```c++
struct Light {
    glm::vec4 position;
    glm::vec4 color; 
    glm::vec4 a;  
}
``` 

Proszę dodać odpowiedni kod w metodzie `init` i `frame`. W tej pierwszej inicjalizujemy wartość pola `ligh_a`, a w tej drugiej transformujemy położenie światła do współrzędnych kamery i ładujemy do odpowiedniego bufora uniform. Światło proszę ustawić w odległości 0.5 nad środkiem kwadratu, a jego kolor na biały. Zmienną `a` odpowiedzialną za  osłabienie światła proszę zostawić na razie nieustaloną. 
Proszę pamiętać, że aby położenie światła poprawnie transformowało się za pomocą macierzy `V` musi mieć czwartą współrzędną ustawioną na 1. 

### Oświetlenie dyfuzyjne 

Teraz już w shaderze fragmentów powinniśmy mieć wszystkie wielkości potrzebne do wyliczenia oświetlenia. 

Zacznijmy od wektora normalnego. Został on przekazany w zmiennej `vertex_normal_in_vs`. Po interpolacji jego długość mogła ulec zmianie, dlatego musimy go znormalizować na nowo:
```glsl
vec3 normal = normalize(vertex_normal_in_vs); 
```
Kolejny potrzebny wektor to wektor "do światła"
```glsl
 vec3 light_vector = normalize(light.position_in_vs - vertex_position_in_vs);
```

Na razie nie będziemy potrzebowali wektora "do obserwatora” ponieważ w modelu Phonga oświetlenie dyfuzyjne od niego nie zależy. 
Teraz liczymy natężenie światła padającego:
```glsl
float light_in = max(0.0,dot(normal, light_vector));
```
I w końcu możemy przypisać pikselowi kolor: 
```glsl
vec4 diffuse_color = texture(diffuse_map, vertex_tex_coord);
vFragColor.a = diffuse_color.a;
vFragColor.rgb = light_in * diffuse_color.rgb*light.color;
```

Powinni Państwo zobaczyć plamę światła na środku kwadratu. Dobrym testem jest przybliżanie i oddalanie światła od powierzchni kwadratu. Paradoksalnie przy oddalaniu kwadrat będzie się on stawał coraz jaśniejszy. Spowodowane jest to faktem, że na razie nie zaimplementowaliśmy osłabiania światła z odległością, natomiast oddalanie źródła światła powoduje, że pada ono pod mniejszym kątem na powierzchnię. W  załączonych plikach `diffuse_dist.png`  pokazane są przykłady jak ma to wyglądąc. Swiatło ustawione jest w odlegści `dist/100` od kwadratu nad jego środkiem. 

### Osłabienie światła

Kolejny krok to włączenie osłabienia światła z odległością. W tym celu wykorzystamy przesłany już do shadera wektor `a` w bloku uniform `Light`. Funkcja osłabiająca będzie miała postać:
```glsl
float attenuation = 1.0/(a[0]+a[1]*r+a[2]*r*r)
```
gdzie `r` jest odległością  fragmentu/piksela od źródła światła. Żeby obliczyć `r` rozbijemy liczenie wektora `light_vector` na dwa etapy. Najpierw policzymy wektor do światła bez normalizacji
```glsl
vec3 light_vector = light.position_in_vs - vertex_position_in_vs;
```
potem policzymy `r`:
```glsl
float r = length(light_vector); 
```
i na koniec znormalizujemy `light_vector`
```glsl
light_vector/=r;
```
Teraz możemy użyć `r` do obliczenia osłabienia światła. Na koniec  zmniejszamy intensywność światła:
```glsl
  float light_in = max(0.0, dot(normal, light_vector))*attenuation;
```

W pliku `diffuse_att_100.png` jest przykładowy wynik działania programu dla światła umieszczonego w odległości 1 od  kwadratu i wektora  `a={1.0, 0.0, 1.0, 0.0}`.  

### Tylne ścianki

Jeśli popatrzą Państwo na kwadrat "od spodu", czyli od tej strony na którą światło nie pada, zobaczą Państwo, że ona również  jest oświetlona. To nieintuicyjne zachowanie wynika z tego, że trójkąty  mają tylko jeden wektor normalny, taki samą  dla obu ścianek. Możemy to zmienić przyjmując konwencję, że normalna "wychodzi" zawsze ze ścianki "przedniej" (Frontface) i w szaderze zmieniać jej kierunek, jeśli widzimy ściankę "od tyłu" (Backface). 
 Informacja o tym, czy widzimy ściankę przednią czy tylną jest przechowywana w zmiennej `gl_FrontFacing`. Możemy więc dodać kod
```glsl
if (!gl_FrontFacing) {
        normal = -normal;
    }
```
Teraz patrząc od dołu, powinniśmy widzieć czarny kwadrat. 

### Światło "ambient"  

Ten czarny kwadrat, to też nie jest do końca poprawne zachowanie. W rzeczywistości powierzchnie, do których nie dociera światło bezpośrednie, rzadko są całkowicie czarne. Dociera bowiem do nich światło rozproszone od innych powierzchni. To oświetlenie globalne (global illumination) jest bardzo trudne do zamodelowania, zwłaszcza w czasie rzeczywistym. Dlatego model Phonga zawiera dodatkowy, bardzo prosty człon światła "ambient", które jest zawsze obecne i nie zależy od położenia i kierunku. Charakteryzuje się tylko kolorem, przy czym przez kolor rozumiem trójkę RGB, a więc uwzględnia to również natężenie. 

Możemy taki człon (kolor światła ambient) dodać zarówno do bloku uniform `Light` jak i struktury `Light` po stronie CPU i przekazać go do bufora uniform razem z resztą zmiennych. 

W szaderze fragmentów możemy go dodać np. ten sposób:
```glsl
vec4 diffuse_color = texture(diffuse_map, vertex_tex_coord);
vFragColor.a = diffuse_color.a;
vFragColor.rgb = diffuse_color.rgb*light.ambient.rgb;
```
a potem resztę swiatła dodajemy  do niego
```glsl
 vFragColor.rgb += light_in * (diffuse_color.rgb + specular_color*spec_intensity)*light.color;
```
W pliku `diffuse_att_amb_100.png` jest przykładowy wynik działaia programu dla światła umieszczonego w odległości   1 od  kwadratu, wektora `a={1.0, 0.0, 1.0, 0.0}` i światła "ambient"  o kolorze `{0.2, 0.2, 0.2}`. 














