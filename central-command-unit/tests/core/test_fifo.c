#include "tests/runner.h"
#include "core/fifo.h"

TEST_REGISTER(test_fifo_multiple_push_pop, "Vérifie push/pop de plusieurs éléments dans la FIFO") {
    fifo_t f;
    fifo_init(&f);

    int a = 1, b = 2, c = 3;
    fifo_push(&f, &a);
    fifo_push(&f, &b);
    fifo_push(&f, &c);

    int *p1 = (int*)fifo_pop(&f);
    int *p2 = (int*)fifo_pop(&f);
    int *p3 = (int*)fifo_pop(&f);

    TEST_ASSERT(p1 && *p1 == 1, "Premier élément doit être 1");
    TEST_ASSERT(p2 && *p2 == 2, "Deuxième élément doit être 2");
    TEST_ASSERT(p3 && *p3 == 3, "Troisième élément doit être 3");
}

// Test FIFO reste vide après pop de tous les éléments
TEST_REGISTER(test_fifo_empty_after_pop, "Vérifie que la FIFO est vide après avoir pop tous les éléments") {
    fifo_t f;
    fifo_init(&f);

    int x = 10;
    fifo_push(&f, &x);
    fifo_pop(&f);

    TEST_ASSERT(fifo_pop(&f) == NULL, "FIFO doit être vide après avoir retiré tous les éléments");
}

// Test taille de FIFO non modifiée par pop vide
TEST_REGISTER(test_fifo_pop_empty_no_effect, "Pop sur FIFO vide ne modifie pas la taille") {
    fifo_t f;
    fifo_init(&f);

    TEST_ASSERT(f.size == 0, "FIFO initialement vide doit avoir taille 0");
    fifo_pop(&f);
    TEST_ASSERT(f.size == 0, "Pop sur FIFO vide ne doit pas modifier la taille");
}

// Test FIFO NULL protection
TEST_REGISTER(test_fifo_null_push, "Vérifie que push NULL n'ajoute pas d'élément") {
    fifo_t f;
    fifo_init(&f);

    fifo_push(&f, NULL);
    TEST_ASSERT(fifo_pop(&f) == NULL, "Push NULL ne doit pas ajouter d'élément");
}

// Test FIFO ordre LIFO vs FIFO
TEST_REGISTER(test_fifo_order, "Vérifie que FIFO conserve l'ordre d'insertion") {
    fifo_t f;
    fifo_init(&f);

    int v1 = 100, v2 = 200, v3 = 300;
    fifo_push(&f, &v1);
    fifo_push(&f, &v2);
    fifo_push(&f, &v3);

    int *p1 = (int*)fifo_pop(&f);
    int *p2 = (int*)fifo_pop(&f);
    int *p3 = (int*)fifo_pop(&f);

    TEST_ASSERT(*p1 == 100 && *p2 == 200 && *p3 == 300, "FIFO doit renvoyer les éléments dans l'ordre d'insertion");
}