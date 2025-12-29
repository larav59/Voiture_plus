#include "vehicle/local_decision.h"

#include <string.h>

void handleCategory(const char *json) {
    printf("[HANDLER] Entrée dans handleCategory\n");
    const char *categories[] = {
        "yield",
        "straight_forward",
        "priority_road",
        "parking",
        "speed_limit_30",
        "end_speed_limit_30",
        "railroad_crossing",
        "roundabout",
        "black_line",
        "cars"
    };
    const int n_categories = sizeof(categories)/sizeof(categories[0]);

    for (int i = 0; i < n_categories; i++) {
        if (strstr(json, categories[i]) != NULL) {
            printf("[HANDLER] Category détectée : %s\n", categories[i]);
            
            // --- Switch simulé ---
            switch(i) {
                case 0: printf("Action: yield\n"); break;
                case 1: printf("Action: straight_forward\n"); break;
                case 2: printf("Action: priority_road\n"); break;
                case 3: printf("Action: parking\n"); break;
                case 4: printf("Action: speed_limit_30\n"); break;
                case 5: printf("Action: end_speed_limit_30\n"); break;
                case 6: printf("Action: railroad_crossing\n"); break;
                case 7: printf("Action: roundabout\n"); break;
                case 8: printf("Action: black_line\n"); break;
                case 9: printf("Action: cars\n"); break;
                default: printf("Action: inconnue\n"); break;
            }

            return; // on arrête après la première catégorie trouvée
        }
    }

    printf("[HANDLER] Category inconnue\n");
}
