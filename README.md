# socket-communication

# Trabalho desenvolvido por:

Ana Vitória Freitas - 11370196

Matheus Luis Oliveira da Silva - 11847429

Thiago H. Cardoso - 11796594

# Funcionamento:

- Para compilar basta ter o gcc instalado e rodar o comando 'make'

- Para rodar existem duas opções: criar um server ou criar um client para algum respectivo server.
	- Para criar um server execute em um terminal 'make run mode=server serverPort=X', sendo X o número da porta desejada
	- Para criar um client execute em um OUTRO terminal 'make run mode=client serverPort=X', sendo X o número da porta do servidor o qual o client será conectado

Assim, fica evidente a regra de que para criar um client, é necessário que um server já tenha sido devidamente alocado e, após realizar as duas alocações, é possível enviar e receber mensagens entre os dois terminais, os quais usarão para conexão o IP local de loopback.
