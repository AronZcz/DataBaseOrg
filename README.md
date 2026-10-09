# DataBaseOrg

Prototyp silnika bazy danych w C++20. Aktualnie rozwijana część to indeks B-tree współpracujący z pamięcią podręczną i plikiem stron. Projekt nie jest jeszcze kompletnym DBMS: klasy bazy i tabel nie realizują CRUD, a SQL, transakcje i odzyskiwanie po awarii nie są zaimplementowane.

## Jak działa obecny prototyp

1. `Value` przechowuje wartość komórki jako `std::variant`; `DataType` wskazuje sposób porównywania. `Label` łączy wartość klucza z listą identyfikatorów wierszy (`tableIndex`).
2. `B_Tree` przechowuje identyfikator strony korzenia (`rootNodeId`). Węzły zawierają uporządkowane klucze i identyfikatory stron dzieci. `insert()` dodaje klucz lub kolejny identyfikator wiersza, a pełne węzły są dzielone przez `splitChild()`. `retrieve()` szuka dokładnej wartości i zwraca jej identyfikatory wierszy.
3. `pageManager::createNode()` zapisuje nowy węzeł przez `discPageManager`, otrzymuje `pageID` i umieszcza węzeł w `cachePageManager`. `pageManager::getNode()` udostępnia węzeł przez `RAIIhandler`.
4. `RAIIhandler::read()` zwraca stały dostęp do węzła, a `get()` dostęp do modyfikacji i oznacza stronę jako dirty. Handler przypina stronę podczas użycia i odpina ją w destruktorze.
5. `discPageManager` odczytuje i zapisuje strony o stałej wielkości. `SerializerBTreeNode` koduje węzeł oraz korzysta z `SerializerLabel` do kodowania kluczy. Istnieje również `SerializerBTree` dla metadanych drzewa.

Przykład w `DataBaseOrg.cpp` tworzy nowe drzewo, wykonuje kilka operacji na kluczach `Int` i wypisuje wyniki wyszukiwania. To ręczny przykład działania w jednym procesie, a nie test trwałości danych po restarcie.

## Pliki danych

`pageManager` otwiera `SerializedNodes.bin` i `SerializedTrees.bin` względem katalogu roboczego uruchomionego programu. Dla węzłów używa stron po **4096 bajtów** z początkiem danych na przesunięciu **32 bajtów**. Strona o `pageID = i` zaczyna się na pozycji `32 + i * 4096`. Wyszukiwanie wolnej strony skanuje strony wypełnione zerami; gdy takiej nie znajdzie, dopisuje kolejną.

Serializacja `Label` obsługuje obecnie tylko wartość `Int` (`int64_t`). Dane są zapisywane przez `memcpy` w natywnym układzie typów C++; format nie ma wersji, sumy kontrolnej ani kontroli przekroczenia rozmiaru strony. `SerializedTrees.bin` jest otwierany, ale aktualny przykład nie zapisuje do niego metadanych drzewa.

Pliki `SerializedNodes.bin`, `SerializedTrees.bin` i `testfile.dat` zostały zachowane w repozytorium jako lokalny stan projektu. Uruchomienie przykładu może zmienić `SerializedNodes.bin`.

## Budowanie i uruchamianie

Otwórz `DataBaseOrg.sln` w Visual Studio 2022 i zbuduj konfigurację **Debug | x64**. Ta konfiguracja używa standardu C++20. Program konsolowy jest generowany w `x64/Debug/DataBaseOrg.exe`.

Autor potwierdził kompilację i działanie bieżącego przykładu w swoim Visual Studio. Kompilacja uruchamiana z terminala Codex przechodziła przez pliki źródłowe, ale linkowanie w tym środowisku kończyło się błędem wersji narzędzia `MSPDB140.DLL`; nie jest to dowód poprawnego odtworzenia danych z dysku. W projekcie nie ma jeszcze automatycznego zestawu testów.

## Stan i ograniczenia

| Obszar | Stan |
| --- | --- |
| `Value`, `DataType`, porównywanie | Zaimplementowane dla typów wariantu; brak pełnej semantyki `NULL` i wartości specjalnych. |
| B-tree: insert, dokładne retrieve, podział węzłów | Implementacja prototypowa używana w jednym procesie; brak testów niezmienników i trwałości. |
| Cache i RAII pin/unpin | Podstawowe metody istnieją; brak limitu pojemności i automatycznego usuwania stron z cache. |
| Strony dyskowe i serializacja | Zapis/odczyt stron oraz serializatory drzewa, węzła i `Label`; dla `Label` obsługiwany jest tylko `Int`. |
| Trwałe odtworzenie B-tree | Niedokończone: po starcie powstaje nowe drzewo z `rootNodeId = -1`, a metadane drzewa nie są wczytywane. |
| Tabele, SQL, transakcje, recovery | Szkielety lub brak implementacji. |

Najważniejsze znane problemy w obecnym przepływie:

- `cachePageManager::get()` rzuca wyjątek przy braku strony. Dlatego `pageManager::getNode()` nie dochodzi wtedy do swojej gałęzi odczytu z `storedNodes`.
- Zmiany stron są oznaczane jako dirty, lecz `RAIIhandler` tylko odpina stronę. `pageManager::storeNode()` nie jest wywoływane przez przykład, więc zmienione węzły mogą pozostać wyłącznie w pamięci.
- `remove()` i `removeByVal()` modyfikują kopię węzła zwróconą przez `read()`, więc wynik nie trafia do cache ani pliku. `edit()` korzysta z `remove()`.
- `discPageManager::readPage()` zwraca wektor o pełnym rozmiarze także po nieudanym odczycie; obecny test `buff.empty()` nie rozpoznaje brakującej strony.
- `B_Tree::clear()` i `size()` zgłaszają `not implemented`.

Kolejny test akceptacyjny dla trwałości powinien zapisać identyfikator korzenia i zmienione strony, zakończyć proces, po ponownym uruchomieniu odtworzyć drzewo i wykonać `retrieve()` **przed** nowymi insertami. Rozmiar pliku nie powinien rosnąć wyłącznie z powodu odczytu tego samego drzewa.

Plan celów DBMS i kryteria akceptacji są prowadzone w [Notion](https://app.notion.com/p/3d7c2eb36aee807d87fcfb6d7d3d6a5c).
