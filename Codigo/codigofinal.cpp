#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

// ==========================================
//   BLOQUE 1: DEFINICIÓN DE ESTRUCTURAS
// ==========================================

// Nodo para representar un Proceso y construir Listas/Colas
struct Proceso {
    int id;
    char nombre[50];
    int prioridad;
    char estado[30];
    Proceso *siguiente; // Puntero al siguiente nodo
};

// Nodo para simular bloques de Memoria (Estructura de Pila)
struct Memoria {
    int idProceso;
    int bloque;
    int tamano;
    Memoria *siguiente; // Puntero al elemento de abajo en la pila
};

// ==========================================
//   BLOQUE 2: VARIABLES GLOBALES Y HASH
// ==========================================

// Punteros de control para las tres estructuras principales
Proceso *listaProcesos = NULL; // Cabeza de la Lista Enlazada Lineal
Proceso *colaCPU = NULL;       // Frente de la Cola de Prioridad
Memoria *pilaMemoria = NULL;   // Tope de la Pila de Memoria

const int TAM_HASH = 10;
Proceso *tablaHash[TAM_HASH];  // Arreglo de punteros para acceso directo

// Función matemática para indexar (Método de la división)
int funcionHash(int id) {
    return id % TAM_HASH;
}

// Inicializa las casillas de la Tabla Hash apuntando a vacío (NULL)
void inicializarHash() {
    for (int i = 0; i < TAM_HASH; i++) {
        tablaHash[i] = NULL;
    }
}

// Vincula un proceso a su casilla correspondiente en la tabla Hash
void insertarHash(Proceso* proc) {
    int pos = funcionHash(proc->id);
    tablaHash[pos] = proc; // Guarda la referencia directa en memoria
}

// ==========================================
//   BLOQUE 3: VALIDACIÓN Y BÚSQUEDA
// ==========================================

// Recorre secuencialmente la lista para verificar si un ID ya existe
bool existeProceso(int id) {
    Proceso *actual = listaProcesos;
    while (actual != NULL) {
        if (actual->id == id) return true; // ID duplicado detectado
        actual = actual->siguiente;
    }
    return false;
}

// Busca un proceso intentando primero acceso rápido (O(1)) por Hash
void buscarProceso() {
    int id;
    cout << "\nIngrese ID a buscar: ";
    cin >> id;

    // Intento 1: Buscar directamente mediante el índice Hash
    int pos = funcionHash(id);
    Proceso *actual = tablaHash[pos];

    if (actual != NULL && actual->id == id) {
        cout << "\nProceso encontrado (via Hash):\n";
        cout << "ID: " << actual->id << "\nNombre: " << actual->nombre << endl;
        return;
    }

    // Intento 2: Búsqueda secuencial exhaustiva (Por si hubo colisión)
    actual = listaProcesos;
    while (actual != NULL) {
        if (actual->id == id) {
            cout << "\nProceso encontrado (via Lista):\n";
            cout << "ID: " << actual->id << "\nNombre: " << actual->nombre << endl;
            return;
        }
        actual = actual->siguiente;
    }
    cout << "Proceso no encontrado.\n";
}

// ==========================================
//   BLOQUE 4: GESTIÓN DE LA LISTA PRINCIPAL
// ==========================================

// Crea un nuevo nodo de proceso, valida sus campos y lo añade al final de la lista
void registrarProceso() {
    Proceso *nuevo = new Proceso;

    cout << "\nIngrese ID del proceso: ";
    cin >> nuevo->id;

    // Validación de ID positivo y único
    if (nuevo->id <= 0 || existeProceso(nuevo->id)) {
        cout << "Error: ID invalido o repetido.\n";
        delete nuevo;
        return;
    }

    cin.ignore();
    cout << "Ingrese nombre del proceso: ";
    cin.getline(nuevo->nombre, 50);

    cout << "Ingrese prioridad (1-5): ";
    cin >> nuevo->prioridad;

    // Validación de rango de prioridad
    if (nuevo->prioridad < 1 || nuevo->prioridad > 5) {
        cout << "Error: prioridad invalida.\n";
        delete nuevo;
        return;
    }

    strcpy(nuevo->estado, "Activo");
    nuevo->siguiente = NULL;

    // Inserción al final de la Lista General de Procesos
    if (listaProcesos == NULL) {
        listaProcesos = nuevo; // Primer nodo del sistema
    } else {
        Proceso *actual = listaProcesos;
        while (actual->siguiente != NULL) {
            actual = actual->siguiente; // Avanza hasta el último nodo
        }
        actual->siguiente = nuevo; // Enlaza el nuevo nodo
    }

    insertarHash(nuevo); // Actualiza la referencia en la Tabla Hash
    cout << "Proceso registrado correctamente.\n";
}

// Recorre e imprime en pantalla todos los procesos registrados secuencialmente
void mostrarProcesos() {
    if (listaProcesos == NULL) {
        cout << "\nNo hay procesos registrados.\n";
        return;
    }

    Proceso *actual = listaProcesos;
    cout << "\n=== LISTA DE PROCESOS ===\n";
    while (actual != NULL) {
        cout << "ID: " << actual->id << " | Nombre: " << actual->nombre 
             << " | Prioridad: " << actual->prioridad << " | Estado: " << actual->estado << endl;
        actual = actual->siguiente; // Salta al siguiente nodo
    }
}

// Localiza un proceso por ID y permite reescribir su valor de prioridad
void modificarPrioridad() {
    int id, nuevaPrioridad;
    cout << "\nIngrese ID del proceso: ";
    cin >> id;

    Proceso *actual = listaProcesos;
    while (actual != NULL) {
        if (actual->id == id) {
            cout << "Ingrese nueva prioridad (1-5): ";
            cin >> nuevaPrioridad;

            if (nuevaPrioridad < 1 || nuevaPrioridad > 5) {
                cout << "Prioridad invalida.\n";
                return;
            }
            actual->prioridad = nuevaPrioridad;
            cout << "Prioridad actualizada correctamente.\n";
            return;
        }
        actual = actual->siguiente;
    }
    cout << "Proceso no encontrado.\n";
}

// Localiza un proceso por ID y permite reescribir su cadena de estado
void cambiarEstado() {
    int id;
    cout << "\nIngrese ID del proceso: ";
    cin >> id;

    Proceso *actual = listaProcesos;
    while (actual != NULL) {
        if (actual->id == id) {
            cin.ignore();
            cout << "Ingrese nuevo estado: ";
            cin.getline(actual->estado, 30);
            cout << "Estado actualizado correctamente.\n";
            return;
        }
        actual = actual->siguiente;
    }
    cout << "Proceso no encontrado.\n";
}

// Desenlaza un nodo de la lista general, limpia su rastro en Hash y libera su memoria
void eliminarProceso() {
    int id;
    cout << "\nIngrese ID del proceso a eliminar: ";
    cin >> id;

    Proceso *actual = listaProcesos;
    Proceso *anterior = NULL;

    // Búsqueda manteniendo rastro del nodo anterior para no romper la lista
    while (actual != NULL) {
        if (actual->id == id) {
            if (anterior == NULL) {
                listaProcesos = actual->siguiente; // El nodo a eliminar era la cabeza
            } else {
                anterior->siguiente = actual->siguiente; // Puentea el nodo intermedio
            }

            // Remueve de forma segura la referencia de la Tabla Hash si apuntaba a este nodo
            int pos = funcionHash(id);
            if (tablaHash[pos] == actual) {
                tablaHash[pos] = NULL; 
            }

            delete actual; // Destrucción del objeto en la memoria Heap
            cout << "Proceso eliminado correctamente.\n";
            return;
        }
        anterior = actual;
        actual = actual->siguiente;
    }
    cout << "Proceso no encontrado.\n";
}

// ==========================================
//   BLOQUE 5: COLA DE PRIORIDAD (CPU)
// ==========================================

// Inserta un clon de un proceso en una cola ordenada de mayor a menor prioridad
void encolarCPU() {
    int id;
    cout << "\nIngrese ID del proceso para enviar a CPU: ";
    cin >> id;

    // Buscar el proceso original en la lista general
    Proceso *buscado = listaProcesos;
    while (buscado != NULL && buscado->id != id) {
        buscado = buscado->siguiente;
    }

    if (buscado == NULL) {
        cout << "Proceso no encontrado.\n";
        return;
    }

    strcpy(buscado->estado, "En cola CPU");

    // Construcción del nodo réplica para la Cola
    Proceso *nuevo = new Proceso;
    nuevo->id = buscado->id;
    strcpy(nuevo->nombre, buscado->nombre);
    nuevo->prioridad = buscado->prioridad;
    strcpy(nuevo->estado, buscado->estado);
    nuevo->siguiente = NULL;

    // Algoritmo de Inserción Ordenada (De Mayor prioridad a Menor)
    if (colaCPU == NULL || nuevo->prioridad > colaCPU->prioridad) {
        nuevo->siguiente = colaCPU; // Inserta al principio si tiene la prioridad más alta
        colaCPU = nuevo;
    } else {
        Proceso *actual = colaCPU;
        // Avanza mientras el siguiente elemento tenga mayor o igual prioridad que el nuevo
        while (actual->siguiente != NULL && actual->siguiente->prioridad >= nuevo->prioridad) {
            actual = actual->siguiente;
        }
        nuevo->siguiente = actual->siguiente; // Ajusta punteros intermedios
        actual->siguiente = nuevo;
    }
    cout << "Proceso encolado segun prioridad en CPU.\n";
}

// Procesa el elemento al frente de la cola (Mayor prioridad), cambiándole el estado y liberándolo
void ejecutarCPU() {
    if (colaCPU == NULL) {
        cout << "\nLa cola de CPU esta vacia.\n";
        return;
    }

    Proceso *temp = colaCPU; // Almacena el frente temporalmente
    cout << "\nEjecutando proceso en CPU:\n";
    cout << "ID: " << temp->id << " | Nombre: " << temp->nombre << " | Prioridad: " << temp->prioridad << endl;

    // Sincroniza el cambio de estado en la Lista Principal
    Proceso *actual = listaProcesos;
    while (actual != NULL) {
        if (actual->id == temp->id) {
            strcpy(actual->estado, "Terminado/Activo");
            break;
        }
        actual = actual->siguiente;
    }

    colaCPU = colaCPU->siguiente; // Avanza el frente de la cola
    delete temp;                  // Libera el nodo procesado de la CPU

    cout << "Proceso ejecutado y retirado de la cola de CPU.\n";
}

// Recorre e imprime los elementos en espera dentro de la Cola de CPU
void mostrarColaCPU() {
    if (colaCPU == NULL) {
        cout << "\nLa cola de CPU esta vacia.\n";
        return;
    }

    Proceso *actual = colaCPU;
    cout << "\n=== COLA DE PRIORIDAD CPU ===\n";
    while (actual != NULL) {
        cout << "ID: " << actual->id << " | Nombre: " << actual->nombre << " | Prioridad: " << actual->prioridad << endl;
        actual = actual->siguiente;
    }
}

// ==========================================
//   BLOQUE 6: PILA DE MEMORIA (LIFO)
// ==========================================

// Inserta un bloque de memoria en el tope de la estructura (Operación PUSH)
void asignarMemoria() {
    int idProc;
    cout << "\nIngrese ID del proceso: ";
    cin >> idProc;

    if (!existeProceso(idProc)) {
        cout << "Error: el proceso no existe.\n";
        return;
    }

    Memoria *nuevo = new Memoria;
    nuevo->idProceso = idProc;

    cout << "Ingrese numero de bloque: ";
    cin >> nuevo->bloque;

    cout << "Ingrese tamano del bloque: ";
    cin >> nuevo->tamano;

    if (nuevo->tamano <= 0) {
        cout << "Tamano invalido.\n";
        delete nuevo;
        return;
    }

    // Mecánica de Pila: El nuevo siempre apunta a la antigua cabeza
    nuevo->siguiente = pilaMemoria;
    pilaMemoria = nuevo; // El nuevo nodo pasa a ser el tope

    cout << "Memoria asignada correctamente con PUSH.\n";
}

// Extrae y destruye el último bloque de memoria añadido en el tope (Operación POP)
void liberarMemoria() {
    if (pilaMemoria == NULL) {
        cout << "\nLa pila de memoria esta vacia.\n";
        return;
    }

    Memoria *temp = pilaMemoria; // Captura el nodo del tope

    cout << "\nLiberando memoria (POP):\n";
    cout << "ID Proceso: " << temp->idProceso << " | Bloque: " << temp->bloque << " | Tamano: " << temp->tamano << endl;

    pilaMemoria = pilaMemoria->siguiente; // El tope baja al elemento inferior
    delete temp;                          // Se libera la memoria física

    cout << "Memoria liberada correctamente.\n";
}

// Recorre e imprime la pila de memoria desde el tope hasta la base
void mostrarMemoria() {
    if (pilaMemoria == NULL) {
        cout << "\nNo hay memoria asignada en la pila.\n";
        return;
    }

    Memoria *actual = pilaMemoria;
    cout << "\n=== PILA DE MEMORIA ===\n";
    while (actual != NULL) {
        cout << "ID Proceso: " << actual->idProceso << " | Bloque: " << actual->bloque << " | Tamano: " << actual->tamano << endl;
        actual = actual->siguiente;
    }
}

// ==========================================
//   BLOQUE 7: ENTRADA/SALIDA DE ARCHIVOS
// ==========================================

// Escribe secuencialmente los datos de la Lista Principal en un archivo plano de texto
void guardarArchivo() {
    ofstream archivo("procesos.txt");
    if (!archivo) {
        cout << "Error al crear archivo.\n";
        return;
    }

    Proceso *actual = listaProcesos;
    while (actual != NULL) {
        archivo << actual->id << "\n";
        archivo << actual->nombre << "\n";
        archivo << actual->prioridad << "\n";
        archivo << actual->estado << "\n";
        actual = actual->siguiente;
    }

    archivo.close();
    cout << "Datos guardados en procesos.txt\n";
}

// Lee el archivo de texto y reconstruye la lista general y el Hash de forma limpia
void cargarArchivo() {
    ifstream archivo("procesos.txt");
    if (!archivo) {
        cout << "No existe archivo previo. Iniciando base de datos vacia.\n";
        return;
    }

    int idTemporal;
    // Control de lectura basado en la obtención del ID primario para evitar registros fantasma o duplicaciones
    while (archivo >> idTemporal) {
        Proceso *nuevo = new Proceso;
        nuevo->id = idTemporal;
        archivo.ignore();

        archivo.getline(nuevo->nombre, 50);
        archivo >> nuevo->prioridad;
        archivo.ignore();
        archivo.getline(nuevo->estado, 30);

        nuevo->siguiente = NULL;

        // Reconstrucción de la Lista General por el extremo final
        if (listaProcesos == NULL) {
            listaProcesos = nuevo;
        } else {
            Proceso *actual = listaProcesos;
            while (actual->siguiente != NULL) {
                actual = actual->siguiente;
            }
            actual->siguiente = nuevo;
        }

        insertarHash(nuevo); // Re-mapeo del nodo cargado en la Tabla Hash
    }

    archivo.close();
}

// Muestra el estado del índice de accesos directos del arreglo Hash
void mostrarHash() {
    cout << "\n=== TABLA HASH DE PROCESOS ===\n";
    for (int i = 0; i < TAM_HASH; i++) {
        cout << "Posicion [" << i << "]: ";
        if (tablaHash[i] != NULL) {
            cout << "ID " << tablaHash[i]->id << " - " << tablaHash[i]->nombre << endl;
        } else {
            cout << "Vacio\n";
        }
    }
}

// ==========================================
//   BLOQUE 8: LIMPIEZA DE MEMORIA Y MENÚ
// ==========================================

// Recorre de forma exhaustiva todas las estructuras dinámicas aplicando "delete" para evitar Fugas de Memoria
void liberarTodaLaMemoria() {
    // 1. Vacía la lista de procesos
    while(listaProcesos != NULL) {
        Proceso *temp = listaProcesos;
        listaProcesos = listaProcesos->siguiente;
        delete temp;
    }
    // 2. Vacía la cola remanente de la CPU
    while(colaCPU != NULL) {
        Proceso *temp = colaCPU;
        colaCPU = colaCPU->siguiente;
        delete temp;
    }
    // 3. Vacía la pila de asignaciones de memoria
    while(pilaMemoria != NULL) {
        Memoria *temp = pilaMemoria;
        pilaMemoria = pilaMemoria->siguiente;
        delete temp;
    }
}

// Interfaz por Consola basada en Switch-Case para invocar las rutinas de control
void menu() {
    int opcion;
    do {
        cout << "\n===== SISTEMA DE GESTION DE PROCESOS =====\n";
        cout << "1. Registrar proceso\n2. Mostrar procesos\n3. Buscar proceso\n";
        cout << "4. Modificar prioridad\n5. Cambiar estado\n6. Eliminar proceso\n";
        cout << "7. Encolar proceso en CPU\n8. Ejecutar proceso CPU\n9. Mostrar cola CPU\n";
        cout << "10. Asignar memoria PUSH\n11. Liberar memoria POP\n12. Mostrar pila de memoria\n";
        cout << "13. Mostrar tabla hash\n14. Guardar datos\n0. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: registrarProceso(); break;
            case 2: mostrarProcesos(); break;
            case 3: buscarProceso(); break;
            case 4: modificarPrioridad(); break;
            case 5: cambiarEstado(); break;
            case 6: eliminarProceso(); break;
            case 7: encolarCPU(); break;
            case 8: ejecutarCPU(); break;
            case 9: mostrarColaCPU(); break;
            case 10: asignarMemoria(); break;
            case 11: liberarMemoria(); break;
            case 12: mostrarMemoria(); break;
            case 13: mostrarHash(); break;
            case 14: guardarArchivo(); break;
            case 0:
                guardarArchivo();
                liberarTodaLaMemoria(); // Llama a la rutina de destrucción total
                cout << "Memoria de estructuras liberada. Saliendo del sistema...\n";
                break;
            default:
                cout << "Opcion invalida.\n";
        }
    } while (opcion != 0);
}

// Función principal del programa
int main() {
    inicializarHash(); // Prepara el arreglo de direccionamiento hash
    cargarArchivo();    // Recupera la sesión anterior si el archivo existe
    menu();             // Inicia el ciclo interactivo de la aplicación
    return 0;
}
