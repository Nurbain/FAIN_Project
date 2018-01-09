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

IMG TODO

Lancement dans le répèrtoire courant: 
>`./plot 800 800` 

Manip à la lettre TODO

-----------------------------------------------------------------

### Details sur le code 

TODO
