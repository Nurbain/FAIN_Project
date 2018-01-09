# Projet de FAIN 
## Manipulation/Création de droite de Bresenham 
------------------------------------------------------------------
### Compilation - Version 1.0

Compilation:
>`make`

Lancement: 
>`plot largeur hauteur`       

<br/><br/>

### Fonctionnalités

* **A** : Passe en mode "Append" pour rajouter des sommets par `clic gauche`
  <br/>
* **V** : Passe en mode "Vertex" pour activer les manipulations de sommets :
  <br/><br/>
  **_Touches Autorisées dans ce Mode_**
  - **&**     : Séléctionne le sommet précédent
  - **é**     : Séléctionne le sommet suivant
  - **Suppr** : Supprime le somet séléctionné 
  - *Flèches Directives*
    - **Up** : Monte le sommet séléctionné
    - **Down**: Descends le sommet séléctionné
    - **Left**: Déplace vers la gauche le sommet séléctionné
    - **Right**: Déplace vers la droite le sommet séléctionné
      <br/>
  
  **_Manipulation Souris Autorisées dans ce Mode_**
  
  - **Clic Droit** : Séléctionne le sommet le plus proche du clic 
  <br/>
* **E** : Passe en mode "Edges" pour activer les manipulations d'arêtes :
  <br/><br/>
  **_Touches Autorisées dans ce Mode_**
  - **&**     : Séléctionne l'arête précédente
  - **é**     : Séléctionne l'arête suivante
    <br/>
  
  **_Manipulation Souris Autorisées dans ce Mode_**
  
  - **Clic Droit** : Séléctionne l'arête la plus proche du clic 
  - **Clic Milieu** : Coupe l’arête sélectionnée en deux en insérant un nouveau
sommet entre ses extrémités
  <br/>
* **C** : Ferme la ligne brisé 
* **F** : Remplie le polygone fermé par la méthode *scan-line*
* **W** : Dessine une ligne suivant un tableau donnée en dure
* **Z** : Zoom sur le dernier sommet ajouté 
* **I** : Dézoome


### Exemple de Manipulation 

<img src="https://user-images.githubusercontent.com/11646693/34745675-945a79cc-f591-11e7-9119-63aeaf7792ab.png" width="400">

Lancement dans le répèrtoire courant: 
>`./plot 800 800` 


1. Cliqué sur l'image pour dessiner les points en commencent en haut a droite
<img src="https://user-images.githubusercontent.com/11646693/34745837-2669f234-f592-11e7-8018-1dd03c4bf462.png" width="250">

2. Passer en Mode Vertex **V** et séléctionné l'avant dernier point (l'angle concave) avec la souris ou les touches "&"/"é"
3. Supprimer le avec **Suppr**
<img src="https://user-images.githubusercontent.com/11646693/34745890-60dde204-f592-11e7-88c2-763dfdb1755d.png" width="250">

4. Séléctionné le second sommet (toujours en partant d'en haut a droite 
5. Descender le une quinzaine  de fois
6. Fermé le polygone avec **C**
<img src="https://user-images.githubusercontent.com/11646693/34745915-75ef28ec-f592-11e7-9b29-8e878741ec36.png" width="250">

7. Remplissez le avec **F**
<img src="https://user-images.githubusercontent.com/11646693/34745675-945a79cc-f591-11e7-9119-63aeaf7792ab.png" width="400">

*Seul les étapes 1-6-7 sont obligatoire pour remplir un polygone mais cette manipulation est faite afin de découvrir la plupart des fonctionnalitées* 
<br/>
*Vous pouvez vous amuser par la suite avec les commandes décrites plus haut*

-----------------------------------------------------------------

### Details sur le code 

- Codé en C
- Liste Doublement chainé comme Structure utilisé pour le stockage de sommet 
- La méthode de remplissage utilisé est *scan-line* avec une cohérence horizontale
- Le carré de selection de sommet sauvgarde l'état de l'image avant son arrivé pour pouvoir redessiner les pixels aux bonnes quand on séléctionne un autre sommet (permet de ne pas tous redessiner a chaque fois)
- Lorsque on doit supprimer des arêtes on n'efface seulement les arêtes concernées pour eviter de tout redessiner donc on trace une droite de breshenam mais noir 
- L'arête crée lors de la fermeture n'existe pas dans la structure il a  donc était nécessaire de gérer les cas particuliers dans le code 

###Auteur : Nathan URBAIN
