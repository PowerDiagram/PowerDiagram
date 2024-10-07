
Prop: on ne stocke pas les edges. On va plutôt les chercher à la volée, en partant des points extérieurs.
  Rq: on pourra filtrer la liste des cuts dès le démarrage (sauf si on a besoin des recalculer les positions avec sys linéaire)
  => a priori, on n'aura plus besoin de faire de correction sur les numéros de vertices :)
  Comment trouver tous les edges qui sont attachés à un vertex ?
    Prop: on fait une table edge => numéro de noeud (qui démarre à -1)
    Lorsqu'on fait tous les edges attachés à un noeud, s'il y a déjà un noeud attaché, on a notre edge.

  Comment faire pour diminuer les mouvements du Cut et vertex ?
  
  Si on passe à une liste de vertex actifs, est-ce qu'on aurait intérêt à aggréger les données ?
    Ça pourrait être bon pour le cache...
    Il faudrait à ce compte là mettre aussi les num_cuts