// hashtable.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACIDAD_INICIAL 16
#define FACTOR_CARGA_MAX 0.75

typedef struct Nodo {
    char *clave;
    int valor;
    struct Nodo *siguiente;
} Nodo;

typedef struct {
    Nodo **cubetas;
    size_t capacidad;
    size_t tamano;
} TablaHash;

/* Hash djb2 */
static size_t hash(const char *s) {
    size_t h = 5381;
    int c;
    while ((c = (unsigned char)*s++))
        h = ((h << 5) + h) + c;
    return h;
}

static char *copiar_cadena(const char *s) {
    size_t n = strlen(s) + 1;
    char *copia = malloc(n);
    if (copia) memcpy(copia, s, n);
    return copia;
}

TablaHash *tabla_crear(void) {
    TablaHash *t = malloc(sizeof(TablaHash));
    if (!t) return NULL;
    t->capacidad = CAPACIDAD_INICIAL;
    t->tamano = 0;
    t->cubetas = calloc(t->capacidad, sizeof(Nodo *));
    if (!t->cubetas) {
        free(t);
        return NULL;
    }
    return t;
}

static int tabla_redimensionar(TablaHash *t) {
    size_t nueva_cap = t->capacidad * 2;
    Nodo **nuevas = calloc(nueva_cap, sizeof(Nodo *));
    if (!nuevas) return 0;

    for (size_t i = 0; i < t->capacidad; i++) {
        Nodo *n = t->cubetas[i];
        while (n) {
            Nodo *sig = n->siguiente;
            size_t idx = hash(n->clave) % nueva_cap;
            n->siguiente = nuevas[idx];
            nuevas[idx] = n;
            n = sig;
        }
    }
    free(t->cubetas);
    t->cubetas = nuevas;
    t->capacidad = nueva_cap;
    return 1;
}

/* Inserta o actualiza. Devuelve 1 si tuvo éxito, 0 si falló. */
int tabla_insertar(TablaHash *t, const char *clave, int valor) {
    size_t idx = hash(clave) % t->capacidad;

    for (Nodo *n = t->cubetas[idx]; n; n = n->siguiente) {
        if (strcmp(n->clave, clave) == 0) {
            n->valor = valor;
            return 1;
        }
    }

    if ((double)(t->tamano + 1) / t->capacidad > FACTOR_CARGA_MAX) {
        if (!tabla_redimensionar(t)) return 0;
        idx = hash(clave) % t->capacidad;
    }

    Nodo *nuevo = malloc(sizeof(Nodo));
    if (!nuevo) return 0;
    nuevo->clave = copiar_cadena(clave);
    if (!nuevo->clave) {
        free(nuevo);
        return 0;
    }
    nuevo->valor = valor;
    nuevo->siguiente = t->cubetas[idx];
    t->cubetas[idx] = nuevo;
    t->tamano++;
    return 1;
}

/* Devuelve 1 si la clave existe y guarda el valor en *salida. */
int tabla_obtener(const TablaHash *t, const char *clave, int *salida) {
    size_t idx = hash(clave) % t->capacidad;
    for (Nodo *n = t->cubetas[idx]; n; n = n->siguiente) {
        if (strcmp(n->clave, clave) == 0) {
            if (salida) *salida = n->valor;
            return 1;
        }
    }
    return 0;
}

/* Devuelve 1 si eliminó la clave, 0 si no existía. */
int tabla_eliminar(TablaHash *t, const char *clave) {
    size_t idx = hash(clave) % t->capacidad;
    Nodo *prev = NULL;
    for (Nodo *n = t->cubetas[idx]; n; prev = n, n = n->siguiente) {
        if (strcmp(n->clave, clave) == 0) {
            if (prev) prev->siguiente = n->siguiente;
            else t->cubetas[idx] = n->siguiente;
            free(n->clave);
            free(n);
            t->tamano--;
            return 1;
        }
    }
    return 0;
}

void tabla_destruir(TablaHash *t) {
    if (!t) return;
    for (size_t i = 0; i < t->capacidad; i++) {
        Nodo *n = t->cubetas[i];
        while (n) {
            Nodo *sig = n->siguiente;
            free(n->clave);
            free(n);
            n = sig;
        }
    }
    free(t->cubetas);
    free(t);
}

/* ---------- Ejemplo de uso ---------- */
int main(void) {
    TablaHash *t = tabla_crear();
    if (!t) return 1;

    tabla_insertar(t, "manzana", 10);
    tabla_insertar(t, "pera", 20);
    tabla_insertar(t, "uva", 30);
    tabla_insertar(t, "manzana", 99);  // actualiza

    int v;
    if (tabla_obtener(t, "manzana", &v))
        printf("manzana = %d\n", v);

    if (!tabla_obtener(t, "kiwi", &v))
        printf("kiwi no existe\n");

    tabla_eliminar(t, "pera");
    printf("tamaño: %zu\n", t->tamano);

    // Prueba de redimensionamiento
    char buf[32];
    for (int i = 0; i < 1000; i++) {
        snprintf(buf, sizeof buf, "clave%d", i);
        tabla_insertar(t, buf, i);
    }
    printf("tamaño: %zu, capacidad: %zu\n", t->tamano, t->capacidad);

    tabla_destruir(t);
    return 0;
}
