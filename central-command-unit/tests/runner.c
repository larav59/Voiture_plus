/**
 * @file test.h
 * @brief Module pour faciliter la création de tests unitaires.
 * @author Lukas
 * @date 2025-10-18
 */
 #include "tests/runner.h"

static test_entry_t *testTable = NULL; /**< Tableau dynamique des tests */
static size_t testCount = 0;           /**< Nombre de tests enregistrés */
static size_t testCapacity = 0;        /**< Capacité du tableau */


/**
 * @brief Enregistre un test unitaire
 * @param name Nom du test
 * @param function Pointeur vers la fonction de test
 */
void test_register(const char *name, test_function_t function, const char *description) {
	if(testCount >= testCapacity) {
		size_t newCapacity = testCapacity == 0 ? 1 : testCapacity * 2;
		testTable = realloc(testTable, newCapacity * sizeof(test_entry_t));
		CHECK_ALLOC(testTable);
		testCapacity = newCapacity;
	}
	testTable[testCount++] = (test_entry_t){name, description, function};
}

/**
 * @brief Exécute tous les tests enregistrés
 * @note Affiche les résultats des tests sur la sortie standard.
 */
void test_run(void) {
	printf(COLOR_BLUE"Running %zu tests...\n"COLOR_RESET, testCount);
	for(size_t i = 0; i < testCount; i++) {
		printf(COLOR_YELLOW"--- Running test: %s ---\n"COLOR_RESET, testTable[i].name);
		printf("Description: %s\n", testTable[i].description);
		testTable[i].function();
	}
	printf(COLOR_BLUE"All tests completed.\n"COLOR_RESET);
}


/**
 * @brief Point d'entrée pour exécuter les tests unitaires
 * @return Exit code
 */
int main(void) {
	test_run();
	return EXIT_SUCCESS;
}