![Estado del Proyecto](https://img.shields.io/badge/Estado%20del%20Proyecto-ESTABLE%20%F0%9F%91%8D-green)
![GitHub Gif](https://github.com/Karim93160/Dark-Web/blob/7612fb26e60cc7816e80ae5bb792eeac1942dee8/20250530_022342.gif)

---

<div align="center">

[![Python](https://img.shields.io/badge/-%F0%9F%90%8DPython-3776AB?style=for-the-badge&logo=python&logoColor=FFD43B&labelColor=3776AB)](https://www.pytho>
[![Bash](https://img.shields.io/badge/-%E2%9A%A1%EF%B8%8FShell-4EAA25?style=for-the-badge&logo=gnu-bash&logoColor=white&labelColor=4EAA25)](https://www.g>
[![C++](https://img.shields.io/badge/-%E2%9C%94%EF%B8%8FC++-FF69B4?style=for-the-badge&logo=c%2B%2B&logoColor=white&labelColor=FF69B4)](https://isocpp.or>

</div>

![GitHub Gif](https://github.com/Karim93160/Dark-Web/blob/8ce3d8fac41a5cbeb94f8082d4febd5d688732d1/Screen_Recording_20250531_163158_Termux-ezgif.com-vide>
[![Version](https://img.shields.io/badge/Version-5.1-red.svg)](https://github.com/Karim93160/wazabi)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](https://opensource.org/licenses/MIT)
[![Maintenance](https://img.shields.io/badge/Maintained-Yes-green.svg?style=flat-square)](https://github.com/YOUR_GITHUB_USERNAME/hashish/commits/main)
[![Platform](https://img.shields.io/badge/Platform-Termux%20%7C%20Linux-lightgrey.svg?style=flat-square)](https://termux.com/)
[![Contributions Welcome](https://img.shields.io/badge/Contributions-Welcome-brightgreen.svg?style=flat-square)](https://github.com/YOUR_GITHUB_USERNAME/>

> ⚠️ **Esta es una herramienta ética destinada a pruebas de seguridad y educación. Cualquier uso malintencionado está estrictamente prohibido.** ⚠️

## 🎯 Presentación

**Hashish Ethical Toolkit** es una colección de herramientas de código abierto diseñadas para profesionales de la seguridad, entusiastas de la cibersegur>

Nuestro objetivo es proporcionar una caja de herramientas versátil y fácil de usar, que permita a los usuarios comprender y mejorar su postura de segurid>

---
## 🛠️ Instalación Rápida 📥
*Copie* y luego *Pegue* en Termux
el siguiente comando:
```{ yes "N" | pkg update -y || true; } && \
{ yes "N" | pkg upgrade -y || true; } && \
pkg install -y git python nmap whois dnsutils curl rsync build-essential openssl ncurses-utils || \
{ echo "Instalación de los paquetes fallida - intento de continuación..."; } && \
pip install shodan && \
git clone [https://github.com/Karim93160/hashish.git](https://github.com/Karim93160/hashish.git) ~/hashish && \
cd ~/hashish && \
chmod +x installer.sh && \
./installer.sh && \
termux-reload-settings


Nos complace anunciar una
Actualización Mayor: ¡Integración de un Módulo de Cracking C++ Optimizado y Ataque por Tabla Arcoíris! 🎉
Nos complace anunciar una mejora significativa de HASHISH: la integración de un módulo de cracking de hash completamente reescrito en C++ (hashcracker.cp>
¿Qué cambia con hashcracker.cpp?
 * Velocidad Superior 🚀: C++ permite una ejecución mucho más rápida de los cálculos de hash y las comparaciones en comparación con las versiones anterio>
 * Estimación Precisa del Tiempo ⏱️: Antes de lanzar un ataque de fuerza bruta, HASHISH ahora realiza un benchmark rápido para estimar la velocidad de has>
   * Esto le permite tomar decisiones informadas sobre la viabilidad de un ataque antes de lanzarlo, evitando así esperar indefinidamente.
 * Gestión Mejorada del Multihilo 🔥: El módulo C++ aprovecha al máximo los múltiples núcleos de su procesador (gracias a OpenMP cuando está disponible) >
Nuevo Método: Ataque por Tabla Arcoíris 🌈
Además de las optimizaciones, HASHISH introduce la potente técnica de ataque por tabla arcoíris. Este método ofrece un compromiso tiempo-memoria, permiti>
Descripción del Método Arcoíris
Una tabla arcoíris almacena cadenas de hashes reducidos. Cada cadena comienza con una contraseña potencial, que se hashea. El resultado se pasa luego por>
 * Ventajas:
   * ⚡ Velocidad de cracking elevada una vez que la tabla se genera y se carga.
   * 💡 Menos cálculos en tiempo real en comparación con la fuerza bruta para el mismo espacio de búsqueda (si la tabla lo cubre).
 * Desventajas:
   * ⏳ Tiempo de pre-cálculo importante para generar la tabla.
   * 💾 Espacio de almacenamiento considerable para las tablas voluminosas.
   * 🎯 Especificidad: Una tabla es generalmente específica para un algoritmo de hash (MD5, SHA256, etc.), un juego de caracteres y un rango de longitude>
rainbow_generator.cpp: Forja tu rainbow.txt ⚙️
Antes de poder lanzar un ataque por tabla arcoíris, debe generar una tabla. Este es el papel de rainbow_generator.cpp. Esta herramienta autónoma le permi>
Manual de usuario de rainbow_generator.cpp (Preajustes para rainbow.txt):
 * Lance rainbow_generator.cpp (se compilará bajo el nombre rainbow_generator o similar).
 * Parámetros a definir durante la generación: La herramienta le guiará para introducir la siguiente información. La coherencia de estos parámetros entre>
   | Parámetro | Ejemplo de Valor | Descripción | Módulo Concernido | Énfasis |
   |---|---|---|---|---|
   | 📤 Archivo de Salida | rainbow.txt | Nombre del archivo donde se guardará la tabla (formato: contraseña_inicial:hash_final_de_cadena). | rainbow_gen>
   | 🔡 Charset | abcdefghijklmnopqrstuvwxyz0123456789 | Conjunto de caracteres utilizados para generar las contraseñas en las cadenas. DEBE coincidir co>
   | 📏 Longitud Mín. Contraseña | 6 | Longitud mínima de las contraseñas a generar dentro de las cadenas. | rainbow_generator.cpp | 🔢 |
   | 📐 Longitud Máx. Contraseña | 8 | Longitud máxima de las contraseñas a generar dentro de las cadenas. | rainbow_generator.cpp | 🔢 |
   | 🔗 Nº Cadenas | 1000000 (1 Millón) | Número de cadenas a generar. Impacta la cobertura de la tabla y su tamaño. Cuanto más, mejor (pero más grande/l>
   | ⛓️ Longitud Cadena | 10000 (diez mil) | Longitud de cada cadena (número de hashes/reducciones). Compromiso entre tiempo de búsqueda y tamaño. | rainb>
   | 🛡️ Tipo de Hash | MD5 / SHA256 etc. | Algoritmo de hash utilizado (ej: MD5, SHA1, SHA256). DEBE coincidir con el tipo de hash objetivo. | rainbow_gen>
 * Consideraciones Importantes:
   * Coherencia Absoluta 🔑: El Charset, el rango Longitud Mín./Máx. Contraseña, y el Tipo de Hash utilizados para la generación deben ser absolutamente >
   * Tiempo & Espacio ⏳💾: La generación de tablas arcoíris, especialmente para conjuntos de caracteres amplios, cadenas largas, un gran número de caden>
   * El generador producirá un archivo (por ejemplo, rainbow.txt) que contiene pares contraseña_de_inicio:hash_de_fin_de_cadena.
Utilizar rainbow.txt en hashcracker.cpp (Fase de Ataque) ⚔️
Una vez que su tabla rainbow.txt (o un archivo con un nombre personalizado) se genera:
 * Lance hashcracker.cpp.
 * Introduzca el hash a crackear.
 * Elija la opción 3. Ataque por Tabla Arcoíris.
 * El programa le preguntará:
   * La ruta a su archivo de tabla arcoíris (por defecto, buscará rainbow.txt en el directorio del ejecutable).
   * Los parámetros de la tabla:
     * El Charset para la reducción (el utilizado durante la generación de la tabla).
     * La longitud de las cadenas (Longitud de Cadena) de la tabla.
     * La longitud mínima y máxima de las contraseñas asumida (Longitud Mín./Máx. Contraseña) durante la generación.
 * hashcracker.cpp cargará entonces la tabla en memoria (esto puede llevar tiempo para las tablas grandes) y comenzará el proceso de búsqueda. Aplicará l>
¿Cómo utilizar los nuevos módulos?
El módulo hashcracker.cpp es ahora el motor principal para los ataques por diccionario y por fuerza bruta cuando elige estas opciones en HASHISH. Para el>
Estamos convencidos de que estas actualizaciones harán que HASHISH sea aún más potente y útil para sus necesidades de seguridad ética y aprendizaje. ¡No >
✨ Funcionalidades Principales
Hash Cracker:
Descifre diferentes tipos de hashes utilizando técnicas comunes.
Web Scanner:
Realice análisis de seguridad básicos en sitios web y direcciones IP.
Reconocimiento:
Recopile información sobre objetivos utilizando diversas técnicas de reconocimiento de redes.
OSINT (Inteligencia de Código Abierto): Reúna información accesible públicamente sobre objetivos específicos.
🚀 Uso
Para lanzar la herramienta principal:
hashish


O, si la instalación global falló:
python3 hashish.py


🤝 Contribuciones
¡Las contribuciones son bienvenidas! Si desea mejorar Hashish, corregir errores o agregar nuevas funcionalidades, consulte nuestra Guía de Contribuciones.





Licencia 📜
hashish se distribuye bajo la licencia MIT License
Contacto 📧️
Para cualquier pregunta o sugerencia, no dude en abrir un issue en GitHub o contactarnos por correo electrónico:
⚠️ Advertencia
Hashish Ethical Toolkit está destinado únicamente para uso ético y legal. Los desarrolladores no son responsables de cualquier uso indebido o ilegal de e>
<p align="center">
Hecho con ❤️ por Karim
</p>
