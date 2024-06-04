Rq: qd on fait les librairies dans les répertoires .vf_symbol_cache, on relance un vfs_build qui ne connait pas les répertoires de PowerDiagram 
  Il y a bien un vfs_build_config dans le répertoire, mais on ne peut pas l'écraser pour chaque utilisation possibles

  Un solution pourrait être de passer par des variables d'environnement
  Ou de passer des arguments par le C++ qui fait l'appel
    => il faudrait ajouter les flags utilisés dans les symboles.
    

Pb: quand on fait le destroy, on a  

