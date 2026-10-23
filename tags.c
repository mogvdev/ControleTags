#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
char cadastroUsuario[50];
char senha[50];
#define MAX_TAGS 500

typedef struct {
    char codigo[20];
    char condominio[50];
    char bloco[10];
    char apartamento[10];
    char morador[60];
    int  mes;
    int  ano;
    int  status;         /* 1 ativa, 2 cancelada, 3 perdida, 4 substituida */
    int  cobrancaFeita;  /* 0 nao feita, 1 feita */
    int  substituicao;     /* 0 nao, 1 sim (nao cobrada) */
} Tag; //Tags e funçoes

Tag tags[MAX_TAGS];
int total = 0;

#define ARQUIVO_TAGS "tags.dat"
#define ARQUIVO_USUARIO "usuario.dat"

void salvarDados(void) {
    FILE *arq = fopen(ARQUIVO_TAGS, "wb");
    if (arq == NULL) {
        printf("\n Erro ao salvar as tags.\n");
        return;
    }
    fwrite(&total, sizeof total, 1, arq);
    fwrite(tags, sizeof(Tag), total, arq);
    fclose(arq);
}//Grava as tags no arquivo

void salvarUsuario(void) {
    FILE *arq = fopen(ARQUIVO_USUARIO, "wb");
    if (arq == NULL) {
        printf("\n Erro ao salvar o usuario.\n");
        return;
    }
    fwrite(cadastroUsuario, sizeof cadastroUsuario, 1, arq);
    fwrite(senha, sizeof senha, 1, arq);
    fclose(arq);
}//Grava o usuario no arquivo

void carregarDados(void) {
    FILE *arq = fopen(ARQUIVO_TAGS, "rb");
    if (arq != NULL) {
        if (fread(&total, sizeof total, 1, arq) != 1 || total < 0 || total > MAX_TAGS) {
            total = 0;
        } else if (fread(tags, sizeof(Tag), total, arq) != (size_t) total) {
            total = 0;
        }
        fclose(arq);
    }

    arq = fopen(ARQUIVO_USUARIO, "rb");
    if (arq != NULL) {
        if (fread(cadastroUsuario, sizeof cadastroUsuario, 1, arq) != 1 ||
            fread(senha, sizeof senha, 1, arq) != 1) {
            cadastroUsuario[0] = '\0';
            senha[0] = '\0';
        }
        fclose(arq);
    }
}//Le os arquivos quando o programa abre

void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
} // Pra nao contar o Enter e os espaços 

void lerTexto(const char *rotulo, char *destino, int tamanho) {
    printf("%s", rotulo);

    if (fgets(destino, tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }

    if (strchr(destino, '\n') == NULL) {
        limparBuffer();
    }

    destino[strcspn(destino, "\n")] = '\0';
}//Pra nao ficar repetindo 

void paraMaiusculas(char *texto) {
    for (int i = 0; texto[i] != '\0'; i++) {
        texto[i] = toupper((unsigned char) texto[i]);
    }
}//Para leitura das letras todas serem maiusculas 

void cadastro_Usuario(void) {
    printf("\n << CADASTRO >> \n");

    lerTexto("\n Digite o nome do usuario: ", cadastroUsuario, sizeof cadastroUsuario);
    lerTexto("\n Digite a senha do usuario: ", senha, sizeof senha);

    printf("\n Usuario cadastrado com sucesso! \n");

}//cadastro

int login_Usuario(void) {
    char loginUsuario[50];
    char senhaDigitada[50];
    int tentativas = 0;

    do {
        printf("\n << LOGIN >> \n");

        lerTexto("\n Digite o nome do usuario: ", loginUsuario, sizeof loginUsuario);
        lerTexto("\n Digite a senha do usuario: ", senhaDigitada, sizeof senhaDigitada);

        if (strcmp(loginUsuario, cadastroUsuario) == 0 &&
            strcmp(senhaDigitada, senha) == 0) {
            printf("\n Login realizado com sucesso! \n");
            return 1;
        }

        tentativas++;
        printf("\n Usuario ou senha incorretos! Tentativas restantes: %d\n", 3 - tentativas);
    } while (tentativas < 3);

    return 0;
}//login

int cadastrarTag(void) {
    Tag nova;
    char resposta;
    if (total >= MAX_TAGS) {
        printf("\n Limite de tags atingido. Nao e possivel cadastrar mais.\n");
        return 0;
    }

    printf("\n=== Cadastrar tag ===\n");
    lerTexto("Codigo da tag: ", nova.codigo, sizeof nova.codigo);
    paraMaiusculas(nova.codigo);
    lerTexto("Condominio: ", nova.condominio, sizeof nova.condominio);
    lerTexto("Bloco: ", nova.bloco, sizeof nova.bloco);
    lerTexto("Apartamento: ", nova.apartamento, sizeof nova.apartamento);
    lerTexto("Morador responsavel: ", nova.morador, sizeof nova.morador);

    do{
        printf("A tag e de substituicao? (s/n): ");
        scanf(" %c", &resposta);
        limparBuffer();
        resposta = tolower((unsigned char) resposta);
        if (resposta != 's' && resposta != 'n') {
            printf("Resposta invalida. Digite s ou n.\n");
        }
    } while (resposta != 's' && resposta != 'n');
    nova.substituicao = (resposta == 's');

    time_t agora = time(NULL);
    struct tm *data = localtime(&agora);
    nova.mes = data->tm_mon + 1;
    nova.ano = data->tm_year + 1900;

    printf("Mes/ano sugerido: %02d/%d\n", nova.mes, nova.ano);
    printf("Deseja alterar o mes/ano? (s/n): ");
    scanf(" %c", &resposta);
    limparBuffer();
    resposta = tolower((unsigned char) resposta);

    if (resposta == 's') {
        do {
            printf("Informe o mes e o ano (mm aaaa): ");
            if (scanf("%d %d", &nova.mes, &nova.ano) != 2) {
                nova.mes = 0;
            }
            limparBuffer();
            if (nova.mes < 1 || nova.mes > 12) {
                printf("Mes invalido.\n");
            }
        } while (nova.mes < 1 || nova.mes > 12);
    }

    nova.status = 1;
    nova.cobrancaFeita = 0;

    tags[total] = nova;
    total++;

    printf("Tag cadastrada.\n");
    return 1;
}//Cadastro de Tags

const char *textoStatus(int status) {
    switch (status) {
        case 1: return "Ativa";
        case 2: return "Cancelada";
        case 3: return "Perdida";
        case 4: return "Substituida";
        default: return "Desconhecido";
    }
}//Status das Tags em texto

const char *textoCobranca(Tag t) {
    if (t.substituicao) {
        return "Isenta";
    }
    return t.cobrancaFeita ? "Feita" : "Nao feita";
}//Texto da cobranca, considerando substituicao

int listarTags(void) {
    int mes, ano;
    int qtd = 0;

    if (total == 0) {
        printf("\n Nenhuma tag cadastrada.\n");
        return 0;
    }

    printf("\n=== Listar tags ===\n");
    printf("Informe o mes e o ano para listar (mm aaaa): ");
    if (scanf("%d %d", &mes, &ano) != 2) {
        mes = 0;
    }
    limparBuffer();

    printf("\n%-10s | %-20s | %-20s | %-6s | %-9s\n",
           "Codigo", "Morador", "Condominio", "Apto", "Cobranca");
    printf("-----------+----------------------+----------------------+--------+----------\n");

    for (int i = 0; i < total; i++) {
        Tag t = tags[i];
        if (t.mes == mes && t.ano == ano) {
            printf("%-10.10s | %-20.20s | %-20.20s | %-6.6s | %-9s\n",
                   t.codigo, t.morador, t.condominio, t.apartamento,
                   textoCobranca(t));
            qtd++;
        }
    }

    if (qtd == 0) {
        printf("\n Nenhuma tag encontrada em %02d/%d.\n", mes, ano);
    } else {
        printf("\n Total de tags em %02d/%d: %d\n", mes, ano, qtd);
    }
    return qtd;
}//Listagem de Tags

int buscarIndice(const char *codigo) {
    for (int i = 0; i < total; i++) {
        if (strcmp(tags[i].codigo, codigo) == 0) {
            return i;
        }
    }
    return -1;
}//Procura a tag pelo codigo 

int buscarTag(void) {
    char codigo[20];
    int indice;

    printf("\n=== Buscar tag ===\n");
    lerTexto("Codigo da tag: ", codigo, sizeof codigo);
    paraMaiusculas(codigo);

    indice = buscarIndice(codigo);
    if (indice == -1) {
        printf("\n Tag nao encontrada.\n");
        return 0;
    }

    Tag t = tags[indice];
    printf("\nCodigo: %s\n", t.codigo);
    printf("Condominio: %s | Bloco: %s | Apartamento: %s\n", t.condominio, t.bloco, t.apartamento);
    printf("Morador: %s\n", t.morador);
    printf("Mes/ano de cadastro: %02d/%d\n", t.mes, t.ano);
    printf("Substituicao: %s\n", t.substituicao ? "Sim" : "Nao");
    printf("Cobranca: %s\n", textoCobranca(t));
    return 1;
}//Busca de tag

int alterarStatusTag(void) {
    char codigo[20];
    int indice;
    int novoStatus;

    printf("\n=== Alterar status da tag ===\n");
    lerTexto("Codigo da tag: ", codigo, sizeof codigo);
    paraMaiusculas(codigo);

    indice = buscarIndice(codigo);
    if (indice == -1) {
        printf("\n Tag nao encontrada.\n");
        return 0;
    }

    printf("Status atual: %s\n", textoStatus(tags[indice].status));
    printf("1 - Ativa\n2 - Cancelada\n3 - Perdida\n4 - Substituida\n");

    do {
        printf("Novo status: ");
        if (scanf("%d", &novoStatus) != 1) {
            novoStatus = 0;
        }
        limparBuffer();
        if (novoStatus < 1 || novoStatus > 4) {
            printf("Opcao invalida. Escolha de 1 a 4.\n");
        }
    } while (novoStatus < 1 || novoStatus > 4);

    tags[indice].status = novoStatus;
    printf("\n Status alterado para %s.\n", textoStatus(novoStatus));
    return 1;
}//Alteracao de status

int registrarCobranca(void) {
    char codigo[20];
    char resposta;
    int indice;

    printf("\n=== Registrar cobranca ===\n");
    lerTexto("Codigo da tag: ", codigo, sizeof codigo);
    paraMaiusculas(codigo);

    indice = buscarIndice(codigo);
    if (indice == -1) {
        printf("\n Tag nao encontrada.\n");
        return 0;
    }

    if (tags[indice].substituicao) {
        printf("\n Esta tag e de substituicao e nao e cobrada.\n");
        return 0;
    }

    printf("Morador: %s | Mes/ano: %02d/%d\n",
           tags[indice].morador, tags[indice].mes, tags[indice].ano);
    printf("Cobranca atual: %s\n", tags[indice].cobrancaFeita ? "Feita" : "Nao feita");

    do {
        printf("A cobranca foi feita no sistema financeiro? (s/n): ");
        scanf(" %c", &resposta);
        limparBuffer();
        resposta = tolower((unsigned char) resposta);
        if (resposta != 's' && resposta != 'n') {
            printf("Resposta invalida. Digite s ou n.\n");
        }
    } while (resposta != 's' && resposta != 'n');

    if (resposta == 's') {
        tags[indice].cobrancaFeita = 1;
    } else {
        tags[indice].cobrancaFeita = 0;
    }

    printf("\n Cobranca registrada como: %s\n",
           tags[indice].cobrancaFeita ? "Feita" : "Nao feita");
    return 1;
}//Registro de cobranca

void menuPrincipal(void) {
    int op;
    do {
        printf("\n << MENU PRINCIPAL >> \n");
        printf("1. Cadastrar tag\n");
        printf("2. Listar tags\n");
        printf("3. Buscar tag\n");
        printf("4. Alterar status da tag\n");
        printf("5. Registrar cobranca\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &op) != 1) {
            op = -1;
        }
        limparBuffer();

        switch (op) {
            case 1:
                cadastrarTag();
                salvarDados();
                break;
            case 2:
                listarTags();
                break;
            case 3:
                buscarTag();
                break;
            case 4:
                alterarStatusTag();
                salvarDados();
                break;
            case 5:
                registrarCobranca();
                salvarDados();
                break;
            case 0:
                printf("\n Encerrando.\n");
                return;
            default:
                printf("\n Opcao invalida!\n");
        }
    } while (1);
}//menu principal

int main(void){
    int op;
    int logado = 0;

    carregarDados();

    do {
        printf("\n << CONTROLE DE TAGS >> \n");
        printf("1. Login\n");
        printf("2. Cadastrar\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &op) != 1) {
            op = -1;
        }
        limparBuffer();

        switch (op) {
            case 1:
                if (cadastroUsuario[0] == '\0') {//caso nao tenha cadastro ainda
                    printf("\n Nenhum usuario cadastrado. Cadastre-se primeiro.\n");
                } else {
                    logado = login_Usuario();//login incorreto
                    if (logado == 0) {
                        printf("\n Numero de tentativas esgotado. Encerrando.\n");
                        return 0;
                    }
                }
                break;
            case 2:
                cadastro_Usuario();
                salvarUsuario();
                logado = login_Usuario();//login
                if (logado == 0) {
                    printf("\n Numero de tentativas esgotado. Encerrando.\n");
                    return 0;
                }//login errado
                break;
            case 0:
                printf("\n Encerrando.\n");
                return 0;
            default:
                printf("\n Opcao invalida!\n");
        }
    } while (logado == 0);

    printf("\n Bem-vindo ao sistema de Controle de Tags!\n");

    menuPrincipal();


    return 0;
}