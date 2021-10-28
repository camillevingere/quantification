<!--
*** Thanks for checking out the quantification. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Thanks again! Now go create something AMAZING! :D
-->

<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/camille/quantification">
    <img src="images/logo.png" alt="Logo" width="300" height="109">
  </a>

  <h3 align="center">Quantification d'image</h3>

  <p align="center">
    Observation de la quantification d'image.
    <br />
    <a href="https://github.com/camillevingere/quantification"><strong>Explorer la doc »</strong></a>
    <br />
    <br />
    <a href="https://github.com/camillevingere/quantification">Voir la démo</a>
    ·
    <a href="https://github.com/camillevingere/quantification/issues">Reporter un bug</a>
    ·
    <a href="https://github.com/camillevingere/quantification/issues">Reporter une fonctionnalité</a>
  </p>
</p>

<!-- ABOUT THE PROJECT -->

## Le projet

[![m=100][test]](https://vingere.com)

Polytech’Lille, SE5, TIM

Le but de ce TP consiste à observer l’effet d’une compression par quantification des niveaux de chaque pixel d’une image bidimensionnelle (en niveau de gris ou en couleur (codage RGB ou HSV)) sur la qualité visuelle de l’image : la quantification peut être uniforme (Exercice 1) ou adaptative (Exercice 2). L’exercice 3 aborde la méthode de Floyd et Steinberg, perturbant le signal mais donnant le meilleur résultat visuel. La réalisation se fera en C.

### Réalisé avec

Ce projet a été réalisé avec les technologies suivantes

- [C](<https://fr.wikipedia.org/wiki/C_(langage)>)

<!-- GETTING STARTED -->

## Pour commencer

Il est nécéssaire d'installer gcc pour pouvoir compiler le programme.

### Prérequis

Vous pouvez les installer comme ceci.

- Mettre à jour la liste des paquets sous linux
  ```sh
  sudo apt update
  ```
- Installer le paquet build-essential
  ```sh
  sudo apt install build-essential
  ```
- Vérfier l'installation de gcc
  ```sh
  gcc --version
  ```

### Installation

1. Cloner le dépôt
   ```sh
   git clone https://github.com/camillevingere/quantification.git
   ```
2. Choisir l'exercice souhaité. Exemple :
   ```sh
   cd exercice1
   ```
3. Compiler le programme
   ```sh
   gcc -o exo1 exercice1.c
   ```
4. Lancer le projet
   ```sh
   ./exo1 woman.pgm
   ```

## Exercice 1 : Compression par quantification uniforme

### Comparaison

#### PGM

Cette quantification est problématique car plus la valeur de n augmente, plus le nombre de niveau de gris diminue. Cela influe beaucoup sur la qualité de l'image.

#### PPM

Cette quantification est moins destructrice sur les images PPM qui sont en couleur. En effet le niveau de couleur est réduit pour 3 couleurs différentes : le rouge, le vert et le bleu.

### Histogramme

#### PGM

|                     Original                      |         n=50          |
| :-----------------------------------------------: | :-------------------: |
| [![Histogramme original][histogramme_original]]() | [![n=50][n_50_pgm]]() |

|          n=100          |          n=200          |
| :---------------------: | :---------------------: |
| [![n=100][n_100_pgm]]() | [![n=200][n_200_pgm]]() |

#### PPM

|                       Original                        |         n=50          |
| :---------------------------------------------------: | :-------------------: |
| [![Histogramme original][histogramme_original_ppm]]() | [![n=50][n_50_ppm]]() |

|          n=100          |          n=200          |
| :---------------------: | :---------------------: |
| [![n=100][n_100_ppm]]() | [![n=200][n_200_ppm]]() |

### Conclusion

En comparant avec l'histogramme original, on peut voir que plus n est grand et plus il y a des "trous" dans l'histogramme. C'est à dire qu'il y a de moins en moins de variété dans les niveaux de gris. C'est encore plus flagrant avec les images en couleurs.

## Exercice 2 : Compression par quantification adaptative

### Comparaison

Cette compression propose de découper par intervalles de l'ensemble des niveaux de gris possible. On utilisera donc des images pgm pour rester en noir et blanc.

Il est nécessaire d'avoir le même nombre de pixels dans chaque intervalle. Chaque intervalle est donc plus ou moins grand en fonction de l'histogramme de l'image.

Pour ce faire, voici les étapes réalisées dans le code :

1. Calcul de l'histogramme

Premièrement on récupère l'histogramme de l'image en comptant le nombre de pixels par niveau de gris.

Exemple :

Niveau de gris : 0, Nombre de Pixels : 3
Niveau de gris : 1, Nombre de Pixels : 6
Etc...

2. Trouver les divisions d'intervalles pour m divisions en fonction de l'histogramme

Pour découper en intervalle, il faut découper de sorte à avoir le même nombre de pixels dans chaque division. Il faut donc parcourir l'histogramme et compter le nombre de pixels jusqu'à atteindre XY/m (le nombre de pixels par intervalle).

3. Changer chaque pixel dans le bon niveau de gris

L'interval étant découpé en plusieurs niveaux de gris, il faut donc affecter les nouvelles valeurs à tous les pixels de l'image.

### Histogramme

| Méthode exercice 1 (n=100) | Méthode exercice 2 (m=100) |
| :------------------------: | :------------------------: |
|  [![n=100][n_100_pgm]]()   |   [![m=50][m_50_pgm]]()    |
|    [![n=100][n=100]]()     |     [![m=100][test]]()     |

### Conclusion

On voit que la qualité de l'image est nettement moins impactée dans la deuxième méthode. L'interval est découpé de façon équilibré contrairement à la méthode numéro 1.

## Exercice 3 : Quantification de Floyd-Steinberg

L'algorithme de Floyd-Steinberg effectue une diffusion de l'erreur de quantification d'un pixel à ses voisins.

Lors de la réduction du nombre de couleurs d'une image (par exemple pour la conversion en GIF, limité à 256 couleurs), cet algorithme permet de conserver l'information qui devrait être perdue par la quantification en la poussant sur les pixels qui seront traités plus tard.

Plus précisément, 7/16 de son erreur est ajoutée au pixel à sa droite, 3/16 au pixel situé en bas à gauche, 5/16 au pixel situé en dessous et 1/16 au pixel en bas à droite.

<!-- CONTACT -->

## Contact

Camille Vingere - camille.vingere@gmail.com

Lien du projet: [https://github.com/camillevingere/quantification](https://github.com/camillevingere/quantification)

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->

[contributors-shield]: https://img.shields.io/github/contributors/camillevingere/quantification.svg?style=for-the-badge
[contributors-url]: https://github.com/camillevingere/quantification/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/camillevingere/quantification.svg?style=for-the-badge
[forks-url]: https://github.com/camillevingere/quantification/network/members
[stars-shield]: https://img.shields.io/github/stars/camillevingere/quantification.svg?style=for-the-badge
[stars-url]: https://github.com/camillevingere/quantification/stargazers
[issues-shield]: https://img.shields.io/github/issues/camillevingere/quantification.svg?style=for-the-badge
[issues-url]: https://github.com/camillevingere/quantification/issues
[license-shield]: https://img.shields.io/github/license/camillevingere/quantification.svg?style=for-the-badge
[license-url]: https://github.com/camillevingere/quantification/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://www.linkedin.com/in/camille-vingere/
[product-screenshot]: images/screenshot.png
[n_50_pgm]: images/n_50_pgm.png
[n_100_pgm]: images/n_100_pgm.png
[n_200_pgm]: images/n_200_pgm.png
[n_50_ppm]: images/n_50_ppm.png
[n_100_ppm]: images/n_100_ppm.png
[n_200_ppm]: images/n_200_ppm.png
[m_50_pgm]: images/n_50_pgm.png
[test]: images/test.png
[n=100]: images/n_100.png
[histogramme_original]: images/histogramme_original.png
[histogramme_original_ppm]: images/histogramme_original_ppm.png
