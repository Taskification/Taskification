Pour réussir ce travail :

On doit choisir tout d'abord l'emplacement où nous allons créer notre plugin :
cd $LLVM_SRC/tools/clang/examples (pour le créer dans le repertoires des examples)

mkdir Taskification
cd Taskification (c'est là où notre plugin résidera)

Notre code source contient 3 classes : TaskVisitor, TaskASTConsumer et PluginTaskAction
qui héritent respectivement des classes : RecursiveASTVisitor, ASTConsumer et PluginASTAction 

Pour construire le plugin dans LLVM:
Nous avons juste besoin d'aller à l'emplacement de notre LVM_BUILD et d'exécuter la commande make :
cd $LLVM_BUILD
make -j8 install > /dev/null

la commande de teste de plugin :
clang -Xclang -load -Xclang Taskification.so -Xclang -plugin -Xclang -task-plugin -c tests.c

Resultat :
Fichier de sortie créé - /tmp/tests.c (Output file created - /tmp/tests.c)
