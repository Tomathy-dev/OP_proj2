SO - Projeto1 - Grupo SO-052

Trabalho realizado pelos seguintes alunos:  
fc51051 - Ivo Estrela  
fc55180 - Ricardo Rocha  
fc57528 - Tomás Salgueiro  

Limitações verificadas:
* Ao correr o Valgrind:
  * detetamos um erro relacionado com a nossa implementação do create_request, mais especificamente
    no uso do strdup.
  * detetamos erros relacionados com a biblioteca dos semáforos, mais especificamente na função sem_open
  * detetamos um erro de "Conditional jump or move depends on uninitialised value(s)", no cliente, restaurante e driver,
que não conseguimos resolver

Implementações:
* Foi acrescentado um pequeno sleep para se conseguir verificar a diferença entre os tempos de restaurante/driver/client.
* Foi acrescentado um pequeno sleep a seguir a criar um request para que os prints relacionados com o request fossem
apresentados antes de aparecer o "Introduzir ação".
* Foi colocada a flag SA_RESTART, porque detetamos um erro ao escrever um novo input a seguir ao print do alarme, o que fez
com que as system calls existentes continuassem a executar.
* Foi ignorado o sinal de SIGINT nos processos filhos, porque ao terminar o programa dava-nos um erro ao terminar os processos
filhos.

Como executar:
* cd bin && ./magnaeats config.txt
