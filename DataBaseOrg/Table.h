#include <variant>
#include <vector>
#include "Values.h"

#pragma once

/*
	DESCRIPTION

	Tabela przechowuje wszystkie dane we wierszach.

	Wiersz ma swój indeks oraz cia³o.

*/

// Dla przechowywania danych
typedef std::vector<Value> rowBody_t;
// Typ wiersza
typedef std::pair<int, rowBody_t> row_t;
// Typ listy wierszy
typedef std::vector<row_t> rowList_t;