# PaintRay

Una aplicación de dibujo vectorial y manipulación de figuras geométricas, desarrollada en C++ utilizando la biblioteca Raylib. Diseñada bajo una arquitectura MVC (Modelo-Vista-Controlador), ofrece un entorno de trabajo rápido, ligero y profesional para la creación gráfica.

## 🚀 Características Principales

* **Herramientas de Dibujo:** Creación interactiva de Líneas, Círculos, Elipses, Rectángulos, Triángulos y Curvas Bezier.
* **Manipulación Dinámica:** Sistema de selección inteligente que permite mover figuras completas o redimensionarlas ajustando sus puntos de control (anclas) en tiempo real.
* **Motor de Renderizado Dual:** Capacidad para alternar instantáneamente entre un renderizado *Optimizado* (funciones nativas de Raylib) y un renderizado *Píxel a Píxel* (basado en algoritmos matemáticos computacionales).
* **Inspector de Propiedades:** Panel avanzado para la gestión de colores, permitiendo modificar el Relleno, el Borde y el Fondo del lienzo (Artboard).
* **Gestión de Capas (Z-Index):** Menú contextual accesible mediante clic derecho para organizar las figuras (Traer al frente, Avanzar nivel, Retroceder nivel, Enviar al fondo) y aplicar colores rápidamente.
* **Sistema de Archivos Nativo:** Guardado y carga de proyectos mediante un formato de serialización propio (`.rpnt`). Utiliza cuadros de diálogo nativos del sistema operativo (gracias a *tinyfiledialogs*) y gestiona automáticamente la creación de directorios seguros.
* **Interfaz de Usuario Moderna:** Diseño *Flat Design* en modo oscuro inspirado en editores profesionales, con iconos vectoriales programados a mano, sistema de tooltips dinámicos y lienzo con profundidad visual.

## 🛠️ Tecnologías Utilizadas

* **Lenguaje:** C++
* **Librería Gráfica:** Raylib
* **Gestión de Diálogos:** tinyfiledialogs
* **Compilación y Construcción:** CMake / MinGW

## 👨‍💻 Desarrollo

Desarrollado por **AleexCh**.