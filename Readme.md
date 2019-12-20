# Titre du Sujet

Analyse Statique Pour la Classification des Procédures Candidate à la « Taskification »

# Mot Clef

LLVM, Analyse statique, compilation, MPI + X, parallélisation automatique

# Description Générale

Les architecture hybrides convergées à venir posent la question des modèles de programmation. En effet MPI depuis l’avènement des architectures many-core a dû être combiné avec du parallélisme intra-noeud en OpenMP (MPI + X). Le mélange de ces modèles se traduit nécessairement par une complexité accrue de l’expression des codes de calcul. Dans ce travail nous proposons de prendre cette tendance à contre-pied en posant la question de l’expression de tâche de calcul en pur MPI. Les étudiants se verront fournir une implémentation de Remote Procedure Calls (RPC) implémentés en MPI, le but du travail et de détecter quelles fonctions sont éligibles à la sémantique RPC statiquement lors de la phase de compilation (c.a.d. les fonction dites « pures »: indépendantes du tas, des TLS, etc …). Le travail visera le compilateur LLVM dans lequel une passe sera rajoutée pour lister l’ensemble des fonctions éligibles à la sémantique RPC. Pour exemple, une implémentation d’un algorithme de cassage de mot de passe en MPI sera fournie avec pour but sa conversion en RPC producteur/consommateur (github.com/besnardjb/MPI_Brute/) avec l’outil.
