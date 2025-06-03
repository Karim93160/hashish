![GitHub stars](https://img.shields.io/github/stars/Karim93160/hashish?style=social)
![GitHub forks](https://img.shields.io/github/forks/Karim93160/hashish?style=social)

[🇫🇷 Français](https://github.com/karim93160/hashish/blob/main/README.md) | [🇬🇧 English](https://github.com/karim93160/hashish/blob/main/README_EN.md) | [🇪🇸 Español](https://github.com/karim93160/hashish/blob/main/README_ES.md)

![Estado del Proyecto](https://img.shields.io/badge/Estado%20del%20Proyecto-ESTABLE%20%F0%9F%91%8D-green)
![GitHub Gif](https://github.com/Karim93160/Dark-Web/blob/7612fb26e60cc7816e80ae5bb792eeac1942dee8/20250530_022342.gif)

---

<div align="center">

[![Python](https://img.shields.io/badge/-%F0%9F%90%8DPython-3776AB?style=for-the-badge&logo=python&logoColor=FFD43B&labelColor=3776AB)](https://www.python.org/)
[![Bash](https://img.shields.io/badge/-%E2%9A%A1%EF%B8%8FShell-4EAA25?style=for-the-badge&logo=gnu-bash&logoColor=white&labelColor=4EAA25)](https://www.gnu.org/software/bash/)
[![C++](https://img.shields.io/badge/-%E2%9C%94%EF%B8%8FC++-FF69B4?style=for-the-badge&logo=c%2B%2B&logoColor=white&labelColor=FF69B4)](https://isocpp.org/)

</div>

![GitHub Gif](https://github.com/Karim93160/Dark-Web/blob/1ac92f41e1878d3e101b6177d0ca3249896d21fd/Screen_Recording_20250603_234629_Termux-ezgif.com-video-to-gif-converter.gif)
[![Versión](https://img.shields.io/badge/Versión-5.5.1-blue.svg)](https://github.com/Karim93160/wazabi)
[![Licencia](https://img.shields.io/badge/Licencia-MIT-yellow.svg?style=flat-square)](https://opensource.org/licenses/MIT)
[![Mantenimiento](https://img.shields.io/badge/Mantenido-Sí-green.svg?style=flat-square)](https://github.com/YOUR_GITHUB_USERNAME/hashish/commits/main)
[![Plataforma](https://img.shields.io/badge/Plataforma-Termux%20%7C%20Linux-lightgrey.svg?style=flat-square)](https://termux.com/)
[![Contribuciones Bienvenidas](https://img.shields.io/badge/Contribuciones-Bienvenidas-brightgreen.svg?style=flat-square)](https://github.com/YOUR_GITHUB_USERNAME/hashish/CONTRIBUTING.md)

> ⚠️ **Esta es una herramienta ética destinada a pruebas de seguridad y educación. Cualquier uso malicioso está estrictamente prohibido.** ⚠️

## 🎯 Presentación

**Hashish Ethical Toolkit** es una colección de herramientas de código abierto diseñadas para profesionales de seguridad, entusiastas de la ciberseguridad e investigadores de seguridad ética. Desarrollado principalmente para su uso en Termux y entornos Linux, Hashish ofrece una interfaz simple e intuitiva para realizar diversas tareas, desde el crackeo de hashes hasta el reconocimiento de redes y OSINT.

Nuestro objetivo es proporcionar una caja de herramientas versátil y fácil de usar que permita a los usuarios comprender y mejorar su postura de seguridad.

---
## 🛠️ Instalación Rápida 📥
*Copie* y *Pegue* en Termux
el siguiente comando:
```
{ yes "N" | pkg update || true; } && \
{ yes "N" | pkg upgrade || true; } && \
pkg install -y git python nmap whois dnsutils curl rsync build-essential openssl \
clang ncurses-utils termux-tools && \
pip install shodan && \
git clone https://github.com/Karim93160/hashish.git ~/hashish && \
cd ~/hashish && \
chmod +x installer.sh && \
./installer.sh && \
termux-reload-settings

```

---
<div align="center">
  <h2>💬 Comunidad Hashish - ¡Únete a la Discusión! 💬</h2>
  <p>
    ¿Tienes preguntas, ideas o simplemente quieres chatear sobre el proyecto Hashish?
    ¡Únete a nuestra comunidad en GitHub Discussions! Es el lugar perfecto para conversaciones abiertas y soporte general.
  </p>
  <p>
    <a href="https://github.com/karim93160/hashish/discussions">
      <img src="https://img.shields.io/badge/Únete%20a%20la%20Comunidad-Discusiones-blue?style=for-the-badge&logo=github" alt="Únete a la Comunidad">
    </a>
  </p>
</div>

**Nos complace anunciar una**
### *Actualización Mayor: ¡Integración de un Módulo de Cracking C++ Optimizado y Ataque por Tabla Rainbow!* 🎉

Nos complace anunciar una mejora significativa de HASHISH: ¡la integración de un **módulo de cracking de hash completamente reescrito en C++** (`hashcracker.cpp`) y la introducción de un **método de ataque por tabla rainbow** con su generador dedicado (`rainbow_generator.cpp`)! Esta actualización trae mayor rendimiento y nuevas capacidades para sus análisis.

#### **¿Qué cambia con `hashcracker.cpp`?**

1. **Velocidad Superior 🚀:** C++ permite una ejecución mucho más rápida de los cálculos de hash y comparaciones en comparación con versiones anteriores. Espere una aceleración notable, especialmente para ataques de fuerza bruta y diccionario en grandes listas.

2. **Estimación Precisa del Tiempo ⏱️:** Antes de lanzar un ataque de fuerza bruta, HASHISH ahora realiza un **benchmark rápido** para estimar la velocidad de hash (en H/s) de su sistema. Basado en esto y la complejidad (longitud de contraseña, conjunto de caracteres), proporciona una estimación del **número total de intentos** necesarios y, lo más importante, el **tiempo estimado** para completar el cracking (mostrado en días, horas, minutos, segundos).
    * Esto le permite tomar decisiones informadas sobre la viabilidad de un ataque antes de iniciarlo, evitando así esperar indefinidamente.

3. **Gestión Mejorada del Multithreading 🔥:** El módulo C++ aprovecha completamente los **múltiples núcleos de su procesador** (gracias a OpenMP cuando está disponible) para distribuir las tareas de hash, acelerando así los ataques.

---

### **Nuevo Método: Ataque por Tabla Rainbow 🌈**

Además de las optimizaciones, HASHISH introduce la poderosa técnica de ataque por **tabla rainbow**. Este método ofrece un compromiso tiempo-memoria, permitiendo crackear hashes mucho más rápido que la fuerza bruta, siempre que se tenga una tabla precalculada.

#### **Descripción del Método Rainbow**

Una tabla rainbow almacena cadenas de hashes reducidos. Cada cadena comienza con una contraseña potencial, que se hashea. El resultado luego pasa por una "función de reducción" para reconvertirlo en una nueva contraseña potencial, que a su vez se hashea, y así sucesivamente, por una cierta longitud. Solo se almacenan la contraseña inicial y el hash final de cada cadena.

Durante el ataque, el hash objetivo se reduce y hashea repetidamente, buscando si alguno de los hashes resultantes coincide con un hash final en la tabla. Si se encuentra una coincidencia, la cadena se reconstruye desde la contraseña inicial almacenada para recuperar la contraseña original.

* **Ventajas:**
    * ⚡ **Alta velocidad de cracking** una vez que la tabla está generada y cargada.
    * 💡 Menos cálculos en tiempo real comparado con la fuerza bruta para el mismo espacio de búsqueda (si la tabla lo cubre).
* **Desventajas:**
    * ⏳ **Tiempo de precálculo significativo** para generar la tabla.
    * 💾 **Espacio de almacenamiento considerable** para tablas grandes.
    * 🎯 **Especificidad:** Una tabla generalmente es específica para un algoritmo de hash (MD5, SHA256, etc.), conjunto de caracteres y rango de longitudes de contraseña.

#### **`rainbow_generator.cpp`: Forje su `rainbow.txt` ⚙️**

Antes de poder lanzar un ataque por tabla rainbow, debe generar una tabla. Este es el papel de `rainbow_generator.cpp`. Esta herramienta independiente le permite crear su archivo de tabla (nombrado `rainbow.txt` por defecto, usado luego por `hashcracker.cpp`).

**Manual de uso de `rainbow_generator.cpp` (Preajustes para `rainbow.txt`):**

1. **Lance `rainbow_generator.cpp`** (se compilará como `rainbow_generator` o similar).
2. **Parámetros a definir durante la generación:** La herramienta le guiará para ingresar la siguiente información. La **consistencia** de estos parámetros entre la generación y el ataque es **ABSOLUTAMENTE CRUCIAL**.

    | Parámetro          | Ejemplo de Valor                 | Descripción                                                                                                | Módulo Concerniente    | Énfasis           |
    | :----------------- | :-------------------------------- | :--------------------------------------------------------------------------------------------------------- | :-------------------- | :---------------- |
    | 📤 **Archivo Salida** | `rainbow.txt`                     | Nombre del archivo donde se guardará la tabla (formato: `contraseña_inicial:hash_final_de_cadena`).         | `rainbow_generator.cpp` | 📝                |
    | 🔡 **Charset** | `abcdefghijklmnopqrstuvwxyz0123456789` | Conjunto de caracteres usados para generar contraseñas en las cadenas. **DEBE coincidir con el charset objetivo.** | `rainbow_generator.cpp` | 🔑 **¡Crucial!** |
    | 📏 **Long. Mín Pass**| `6`                               | Longitud mínima de las contraseñas a generar en las cadenas.                                        | `rainbow_generator.cpp` | 🔢                |
    | 📐 **Long. Máx Pass**| `8`                               | Longitud máxima de las contraseñas a generar en las cadenas.                                        | `rainbow_generator.cpp` | 🔢                |
    | 🔗 **Núm. Cadenas** | `1000000` (1 Millón)             | Número de cadenas a generar. Impacta la cobertura de la tabla y su tamaño. Cuantas más, mejor (pero más grande/largo). | `rainbow_generator.cpp` | 📈                |
    | ⛓️ **Long. Cadena** | `10000` (diez mil)               | Longitud de cada cadena (número de hashes/reducciones). Compromiso entre tiempo de búsqueda y tamaño.      | `rainbow_generator.cpp` | ⚖️                |
    | 🛡️ **Tipo Hash** | `MD5` / `SHA256` etc.             | Algoritmo de hash usado (ej: MD5, SHA1, SHA256). **DEBE coincidir con el tipo de hash objetivo.** | `rainbow_generator.cpp` | 🔑 **¡Crucial!** |

3. **Consideraciones Importantes:**
    * **Consistencia Absoluta 🔑:** El `Charset`, el rango `Long. Mín/Máx Pass`, y el `Tipo Hash` usados para la generación **deben ser imperativamente los mismos** que los que especificará durante el ataque con `hashcracker.cpp` y deben corresponder a las características esperadas de la contraseña objetivo. Cualquier inconsistencia hará que la tabla sea inútil para ese hash específico.
    * **Tiempo y Espacio ⏳💾:** La generación de tablas rainbow, especialmente para conjuntos de caracteres grandes, cadenas largas, gran número de cadenas, o contraseñas largas, puede tomar **muchísimo tiempo** (horas, días, incluso semanas) y consumir una **cantidad significativa de espacio en disco** (desde algunos MB hasta varios GB o TB). ¡Planifique en consecuencia!
    * El generador producirá un archivo (por ejemplo, `rainbow.txt`) conteniendo pares `contraseña_inicial:hash_final_de_cadena`.

#### **Usar `rainbow.txt` en `hashcracker.cpp` (Fase de Ataque) ⚔️**

Una vez que su tabla `rainbow.txt` (o un archivo con nombre personalizado) esté generada:

1. Lance `hashcracker.cpp`.
2. Ingrese el hash a crackear.
3. Elija la opción **3. Ataque por Tabla Rainbow**.
4. El programa le pedirá:
    * La ruta a su archivo de tabla rainbow (por defecto, buscará `rainbow.txt` en el directorio del ejecutable).
    * **Los parámetros de la tabla:**
        * El **Charset para la reducción** (el usado durante la generación de la tabla).
        * La **longitud de las cadenas** (Long. Cadena) de la tabla.
        * La **longitud mínima y máxima de contraseña asumida** (Long. Mín/Máx Pass) durante la generación.
5. `hashcracker.cpp` cargará entonces la tabla en memoria (esto puede tomar tiempo para tablas grandes) y comenzará el proceso de búsqueda. Aplicará las funciones de reducción y hash al hash objetivo, buscando coincidencias con los hashes finales en la tabla. Si se encuentra una coincidencia, reconstruirá la cadena para recuperar la contraseña.

---

### **¿Cómo usar los nuevos módulos?**

El módulo `hashcracker.cpp` es ahora el motor principal para los ataques por **diccionario** y por **fuerza bruta** cuando elige estas opciones en HASHISH. Para el ataque por **tabla rainbow**, primero deberá generar una tabla con `rainbow_generator.cpp` (u obtener una tabla compatible), luego usar la opción dedicada en `hashcracker.cpp`.

Estamos convencidos de que estas actualizaciones harán que HASHISH sea aún más potente y útil para sus necesidades de seguridad ética y aprendizaje. ¡No dude en probarlas y compartir sus comentarios!

---
## ✨ Características Principales

**Hash Cracker:**
Descifre diferentes tipos de hashes usando técnicas comunes.

**Escáner Web:**
Realice análisis básicos de seguridad en sitios web y direcciones IP.

**Reconocimiento:**
Recopile información sobre objetivos usando diversas técnicas de reconocimiento de red.

**OSINT (Inteligencia de Fuentes Abiertas):**
Recopile información públicamente accesible sobre objetivos específicos.

---

## 🚀 Uso

Para iniciar la herramienta principal:


*O, si la instalación global falló:*


________

*🤝 Contribuciones*

**¡Las contribuciones son bienvenidas! Si desea mejorar Hashish, corregir errores o agregar nuevas funcionalidades, por favor consulte nuestra Guía de Contribución.**

[![Patrocíname en GitHub](https://img.shields.io/badge/Patrocinar-GitHub-brightgreen.svg)](https://github.com/sponsors/karim93160)
[![Cómprame un café](https://img.shields.io/badge/Donar-Buy%20Me%20A%20Coffee-FFDD00.svg)](https://www.buymeacoffee.com/karim93160)
[![Apóyame en Ko-fi](https://img.shields.io/badge/Donar-Ko--fi-F16061.svg)](https://ko-fi.com/karim93160)
[![Apóyame en Patreon](https://img.shields.io/badge/Patreon-Apóyame-FF424D.svg)](https://www.patreon.com/karim93160)
[![Donar en Liberapay](https://img.shields.io/badge/Donar-Liberapay-F6C915.svg)](https://liberapay.com/karim93160/donate)

_________

## Licencia 📜

hashish se distribuye bajo la [Licencia MIT](https://github.com/Karim93160/hashish/blob/677eecb0be4fc81a0becc2c2154c65ffe73ddbb1/LICENSE)

_________

## Contacto 📧

Para cualquier pregunta o sugerencia, no dude en abrir un [issue en GitHub](https://github.com/Karim93160/hashish/issues) o contactarnos por email:

[![Contacto por Email](https://img.shields.io/badge/Contacto-por%20Email-blue.svg)](mailto:karim9316077185@gmail.com)

_________
<div align="center">
  <h2>🌿 Hashish - Código de Conducta 🌿</h2>
  <p>
    Nos comprometemos a crear un ambiente acogedor y respetuoso para todos los colaboradores.
    Por favor, tómese un momento para leer nuestro <a href="CODE_OF_CONDUCT.md">Código de Conducta</a>.
    Al participar en este proyecto, acepta sus términos.
  </p>
  <p>
    <a href="CODE_OF_CONDUCT.md">
      <img src="https://img.shields.io/badge/Código%20de%20Conducta-Por%20Favor%20Lea-blueviolet?style=for-the-badge&logo=github" alt="Código de Conducta">
    </a>
  </p>
</div>

<div align="center">
  <h2>🐞 Reportar un Bug en Hashish 🐞</h2>
  <p>
    ¿Encuentras un problema con Hashish? ¡Ayúdanos a mejorar el proyecto reportando los bugs!
    Haz clic en el botón de abajo para abrir directamente un nuevo informe de bug prellenado.
  </p>
  <p>
    <a href="https://github.com/karim93160/hashish/issues/new?assignees=&labels=bug&projects=&template=bug_report.md&title=">
      <img src="https://img.shields.io/badge/Reportar%20un%20Bug-Abrir%20una%20Issue-red?style=for-the-badge&logo=bugsnag" alt="Reportar un Bug">
    </a>
  </p>
</div>



**⚠️ Advertencia**

*Hashish Ethical Toolkit está destinado únicamente para uso ético y legal. Los desarrolladores no son responsables de cualquier uso abusivo o ilegal de esta herramienta.*
<p align="center">
Hecho con ❤️ por Karim
</p>
