

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


Rq: on pourrait fonctionner avec par défaut des cellules "non-bornées". 
  Tant qu'elles le restent, on fait des cuts avec M\V, pour ne rater aucun point.
  Comment déterminer si les cellules sont bornées ?
    => 
  Lorsqu'elles sont bornées, on passe au cut par edge.

  
00    { num_cuts: [ 0, 2, 5 ], pos: [ 1.000000, 1.000000, 0.000000 ] }
01    { num_cuts: [ 1, 2, 5 ], pos: [ 0.000000, 1.000000, 0.000000 ] }
02    { num_cuts: [ 1, 2, 4 ], pos: [ 0.000000, 1.000000, 1.000000 ] }
03    { num_cuts: [ 1, 3, 4 ], pos: [ 0.000000, 0.000000, 1.000000 ] }
04    { num_cuts: [ 0, 2, 4 ], pos: [ 1.000000, 1.000000, 1.000000 ] }
05    { num_cuts: [ 0, 3, 4 ], pos: [ 1.000000, 0.000000, 1.000000 ] }
06    { num_cuts: [ 0, 3, 5 ], pos: [ 1.000000, 0.000000, 0.000000 ] }
07    { num_cuts: [ 1, 3, 5 ], pos: [ 0.000000, 0.000000, 0.000000 ] }
08    { num_cuts: [ 1, 5, 6 ], pos: [ 0.000000, 0.500000, 0.000000 ] }
09    { num_cuts: [ 3, 5, 6 ], pos: [ 0.500000, 0.000000, 0.000000 ] }
10    { num_cuts: [ 1, 3, 6 ], pos: [ 0.000000, 0.000000, 0.500000 ] }
