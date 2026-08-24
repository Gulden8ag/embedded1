# Portafolio Web con MkDocs

Plantilla para crear y publicar un portafolio web utilizando **Markdown**, **MkDocs Material**, **Git** y **GitHub Pages**.

El contenido del sitio se edita localmente en Visual Studio Code dentro de la carpeta `docs/`. Los cambios se guardan con Git y, al hacer `push` al repositorio, el sitio se publica automáticamente mediante GitHub Actions.

---

## Requisitos

Antes de comenzar instala:

- [Visual Studio Code](https://code.visualstudio.com/)
- [Git](https://git-scm.com/downloads)
- [Python 3.10+](https://www.python.org/downloads/)

> En Windows, durante la instalación de Python, activa **Add Python to PATH**.

Comprueba las instalaciones desde una terminal:

```bash
python --version
git --version
```

En macOS/Linux puede ser necesario utilizar:

```bash
python3 --version
```

---

## 1. Crear un repositorio desde esta plantilla

En un navegador de internet abre esta plantilla en GitHub y haz clic en:

```text
Use this template → Create a new repository
```

Asigna un nombre al nuevo repositorio y créalo.

Después copia la dirección HTTPS desde:

```text
Code → HTTPS
```

Te deberia dar un texto similar a:

```text
https://github.com/<tu-usuario>/<tu-repo>.git
```

---

## 2. Clonar el repositorio

Abre Visual Studio Code y selecciona:

```text
Terminal → New Terminal
```

Ve a la carpeta donde quieras guardar el proyecto:

```bash
cd ruta/a/tu/carpeta
```

Clona el repositorio:

```bash
git clone https://github.com/<tu-usuario>/<tu-repo>.git
```

Entra a la carpeta:

```bash
cd <tu-repo>
```

Opcionalmente puedes abrirla directamente en Visual Studio Code:

```bash
code .
```

---

## 3. Configurar Git

Si es la primera vez que utilizas Git en tu computadora, configura tu nombre de usuario y correo de tu cuenta de GitHub:

```bash
git config --global user.name "Tu Nombre de Usuario"
git config --global user.email "tu.email@example.com"
```

---

## 4. Instalar las dependencias

Desde la carpeta principal del repositorio ejecuta:

```bash
python -m pip install -r requirements.txt
```

esto instalara todo lo necesario para ejecutar el sitio localmente.

En algunos sistemas puede ser necesario utilizar:

```bash
python3 -m pip install -r requirements.txt
```

Este paso solo se realiza una vez por computadora.

---

## 5. Ejecutar el sitio localmente

En tu terminal asegurandote que estás en la carpeta principal del repositorio ejecuta:

```bash
mkdocs serve
```

Abre en el navegador:

```text
http://127.0.0.1:8000/
```

Los cambios realizados dentro de `docs/` se actualizarán automáticamente al guardar los archivos.

Para detener el servidor:

```text
Ctrl + C
```

---

## Estructura del proyecto

```text
.
├── docs/
│   └── index.md
│
├── mkdocs.yml
├── requirements.txt
│
└── .github/
    └── workflows/
```

### `docs/`

Contiene las páginas del sitio.

La página principal es:

```text
docs/index.md
```

Para agregar nuevas páginas simplemente crea nuevos archivos `.md` dentro de `docs/`.

Por ejemplo:

```text
docs/
├── index.md
├── proyecto1.md
├── proyecto2.md
└── contacto.md
```

Se recomienda evitar espacios y acentos en los nombres de archivo.

---

### `mkdocs.yml`

Contiene la configuración general del sitio.

El menú lateral puede definirse mediante `nav:`.

Por ejemplo:

```yaml
nav:
  - Inicio: index.md
  - Proyecto 1: proyecto1.md
  - Proyecto 2: proyecto2.md
```

---

### `.github/workflows/`

Contiene la configuración utilizada para publicar automáticamente el sitio.

**No es necesario modificar esta carpeta**.

---

# Flujo normal de trabajo

```Bash
git status
git pull
# trabajar
git status
git add .
git status
git commit -m "..."
git push
```


Antes de comenzar a editar, verifica que tu repositorio local esté actualizado usando:

```bash
git status
```

Del status veras distintos mensajes, algunos tipicos son:

```text
On branch main
Your branch is up to date with 'origin/main'.
nothing to commit, working tree clean
```

Significado:
 - Estas en la rama `main`
 - Tu computadora esta sincronizada con la nube (GitHub)
 - no tienes cambios pendientes de guardar


```text
On branch main
Your branch is up to date with 'origin/main'.

Changes not staged for commit:
  (use "git add <file>..." to update what will be committed)

        modified:   docs/index.md

no changes added to commit
```

Significado:
 - git detecto cambios en `docs/index.md` que no han sido guardados en un commit

```text
Changes to be committed:

        modified:   docs/index.md
        new file:   docs/practica1.md
```

Significado:
    - Los cambios ya estan preparados para ser guardados en un commit


Si tu repositorio local no está actualizado, primero realiza:

```bash
git pull
```

Después realiza los cambios necesarios en los archivos dentro de `docs/`.

Puedes visualizar el sitio localmente con:

```bash
mkdocs serve
```

Cuando termines:

```bash
git status
```

Prepara los cambios:

```bash
git add .
```

Puedes comprobar nuevamente qué archivos serán incluidos:

```bash
git status
```

Crea un commit:

```bash
git commit -m "Descripcion de los cambios"
```

Finalmente sube los cambios:

```bash
git push
```

El flujo habitual es:

```text
git pull
    ↓
Editar archivos
    ↓
mkdocs serve
    ↓
git status
    ↓
git add .
    ↓
git commit
    ↓
git push
```

---

# Publicar con GitHub Pages

El repositorio incluye un flujo de GitHub Actions encargado de generar y publicar automáticamente el sitio.

Después del primer `push`, revisa la pestaña:

```text
Actions
```

y verifica que el proceso termine correctamente.

Después entra a:

```text
Settings
→ Pages
```

Selecciona:

```text
Source: Deploy from a branch
Branch: gh-pages
Folder: /root
```

Una vez configurado, el sitio estará disponible en una dirección similar a:

```text
https://<tu-usuario>.github.io/<tu-repo>/
```

Esta configuración se realiza solamente una vez.

Después, cada:

```bash
git push
```

actualizará automáticamente el sitio publicado.

---

# Problemas comunes

## `python` no se reconoce

Prueba:

```bash
python3 --version
```

Si tampoco funciona, verifica que Python esté instalado y agregado al `PATH`.

---

## `mkdocs` no se reconoce

Instala nuevamente las dependencias:

```bash
python -m pip install -r requirements.txt
```

---

## Revisar la carpeta actual

En macOS/Linux:

```bash
ls
```

En Windows:

```bash
dir
```

---

## `git push` no funciona

Primero intenta actualizar el repositorio:

```bash
git pull
```

y después:

```bash
git push
```

Si Git reporta un **conflicto**, evita utilizar comandos como `--force` hasta resolverlo correctamente.

---

## Resumen rápido

Para trabajar normalmente en el sitio:

```bash
git pull

mkdocs serve

# editar archivos

git status
git add .
git commit -m "Descripcion de los cambios"
git push
```