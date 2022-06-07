#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <string.h>
#include <pthread.h>
#include <stdbool.h>

/*struct sockaddr_in
{
    short int          sin_family;  // Address family, AF_INET
    unsigned short int sin_port;    // Port number
    struct in_addr     sin_addr;    // Internet address
    unsigned char      sin_zero[8]; // Padding para igualar o tamanho do struct sockaddr
};*/

#define MAX_MESSAGE_LENGHT 4098 // Como vamos usar fgets, 2 caracteres a mais serão aceitos para o \0 e \n

typedef struct
{
    int socket;
    struct sockaddr_in serverAddress;
    int addressLenght;
}User;

//Função que será colocada em thread para que o um usuário possa enviar mensagens, enquanto
//ao mesmo tempo as recebem
void* sendMessage(void *args)
{
    User* user = (User*) args;
    char messageToSend [MAX_MESSAGE_LENGHT];

    //Como sabemos, o mtu da maioria dos ips, exceto loopback, é de 1500 bytes, entretanto, 
    //atualmente o protocolo tcp é capaz de lidar com pacotes que excedam esse limite, 
    //particionando a stream em pacotes menores e enviando um de cada vez. 
    while (1)
    {
        char* stringRead = fgets(messageToSend, MAX_MESSAGE_LENGHT, stdin);
        if (stringRead == NULL) break;
        if (stringRead[0] == '\n') continue;
        
        send(user->socket, messageToSend , strlen(messageToSend), 0);
        memset(messageToSend, 0, MAX_MESSAGE_LENGHT);
    }
}

//Para essa entrega, a mesma função de chat pode ser utilizada tanto para server quanto para cliente
//ela executará uma thread secundária para mandar mensagens e a principal para recebê-las
void chat(User* user)
{
    pthread_t sendThread;
    pthread_create(&sendThread, NULL, &sendMessage, (void *)user);

    char messageReceived [MAX_MESSAGE_LENGHT];
    while (1)
    {
        ssize_t bytesReceived = recv(user->socket, messageReceived, MAX_MESSAGE_LENGHT, 0);

        if (bytesReceived == -1)
        {
            perror("Error on recv");
            break;
        }
        if (bytesReceived == 0) break;
        
        printf("%s", messageReceived);
        memset(messageReceived, 0, MAX_MESSAGE_LENGHT);
    }

    pthread_cancel(sendThread);
    close(user->socket);

    printf("leaving the chat...\n");
}

int clientMode(User* user)
{
    if (connect(user->socket, (const struct sockaddr *) &user->serverAddress, (socklen_t) user->addressLenght) != 0)
    {
        perror("Connection error");
        return false;
    }
    printf("Connection successfully established!\n");
    
    chat(user);
    
    return true;
}

int serverMode(User* user)
{
    if (bind(user->socket, (struct sockaddr *)&user->serverAddress, user->addressLenght))
    {
        perror("Bind error");
        return false;
    }
    printf("Socket bind performed successfully!\n");
    
    if(listen(user->socket, 1) == -1)
    {
        perror("Error, unsuccesfully socket listen.");
        return false;
    }
    printf("Socket listen performed successfully!\n");

    struct sockaddr_in serverClient;
    int serverClientSocket = accept(user->socket, (struct sockaddr *)&serverClient, (socklen_t*)&user->addressLenght);
    if (serverClientSocket == -1)
    {
        perror("Error socket accept");
        return false;
    }
    printf("Socket connection accepted!\n");

    User serverClientUser = *user;
    serverClientUser.socket = serverClientSocket;
    chat(&serverClientUser);
    
    close(user->socket);
    return true;
}

int main(int argc, char** argv)
{
    char* portString;
	if (argc > 2)
    {
        if(argv[1] == NULL || argv[1][0] == '\n')
        {
            printf("Error, incorrect parameters!\n");
            return 0;
        }
        else portString = argv[2];   
    }
    else
    {
        printf("Error, incorrect parameters!\n");
        return 0;
    }

	struct sockaddr_in serverAddress;
    serverAddress.sin_family      = AF_INET;
    serverAddress.sin_port        = htons(atoi(portString));
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    //Para conectar dois clientes via IP externo, basta trocar a linha acima por essa
    //inserindo o ip da rede na qual os dois clientes estão conectados
    //inet_pton(AF_INET, "inserirIPExterno", (void*) &serverAddress.sin_addr);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Erro, falha na criação do socket!");
        return 0;
    }
    else
    {
        //Configurar o socket para que o linux desaloque o socket imediatamente após a conexão fechar
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) perror("setsockopt(SO_REUSEADDR) failed!");

        printf("Socket criado com sucesso\n");
    }

    User user = 
    {
        .socket = sock,
        .serverAddress = serverAddress,
        .addressLenght = sizeof(struct sockaddr_in)
    };

    char* useMode = argv[1];
    
	if(strcmp(useMode, "server") == 0) serverMode(&user);
   	else if(strcmp(useMode, "client") == 0) clientMode(&user);
   	else 
   	{
   		printf("Error, incorrect parameters!");
   		return 0;
	}
}

