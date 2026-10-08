# Tabla hash en C

Implementación de una tabla hash en C con claves de tipo `char *` y valores `int`. Resuelve colisiones con encadenamiento separado (listas enlazadas) y se redimensiona automáticamente.

## Características

- Colisiones resueltas con listas enlazadas en cada cubeta.
- Función hash djb2.
- Redimensionamiento automático: la capacidad se duplica cuando el factor de carga supera 0.75.
- La tabla copia las claves, por lo que se pueden pasar buffers temporales.
- Insertar una clave existente actualiza su valor.

## Compilación y ejecución

```bash
gcc -Wall -Wextra -std=c11 -o hashtable hashtable.c
./hashtable
```

## API

| Función | Descripción |
|---|---|
| `TablaHash *tabla_crear(void)` | Crea una tabla vacía. Devuelve `NULL` si falla. |
| `int tabla_insertar(TablaHash *t, const char *clave, int valor)` | Inserta o actualiza. Devuelve 1 si tuvo éxito. |
| `int tabla_obtener(const TablaHash *t, const char *clave, int *salida)` | Busca una clave. Devuelve 1 si existe y guarda el valor en `*salida`. |
| `int tabla_eliminar(TablaHash *t, const char *clave)` | Elimina una clave. Devuelve 1 si existía. |
| `void tabla_destruir(TablaHash *t)` | Libera toda la memoria de la tabla. |

## Ejemplo

```c
TablaHash *t = tabla_crear();
tabla_insertar(t, "manzana", 10);

int v;
if (tabla_obtener(t, "manzana", &v))
    printf("manzana = %d\n", v);

tabla_eliminar(t, "manzana");
tabla_destruir(t);
```

## Complejidad

| Operación | Caso promedio | Peor caso |
|---|---|---|
| Insertar | O(1) amortizado | O(n) |
| Buscar | O(1) | O(n) |
| Eliminar | O(1) | O(n) |

## Autor

[errhernandez12](https://github.com/errhernandez12)
