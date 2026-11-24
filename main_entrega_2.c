#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

#define ARQ_JOGADORES "jogadores.txt"
#define ARQ_PARTIDAS "campeonato.dat"

char velha[3][3];
int nivelComputador;
int contadorPartidas = 0;
char nomeJogador1[50], nomeJogador2[50];

typedef struct Velha {
    int partida; /*número da partida (Ex: primeira(1), segunda(2), terceira(3)...)*/
    char JogVelha[3][3]; // Tabuleiro do jogo. Armazenará as jogadas
    char resultado; // Resultado da Partida: X, 0 ou V(equivale a Velha - empate)
} Partida;

void inicializa_velha();
int menu();
int le_inteiro();
void escolha_simb(char *jog1, char *jog2);
void mostra_velha();
void joga_partida(int opcao, char jog1, char jog2, int salvarCampeonato);
int jogada_usuario(int lin, int col, char jog);
void jogada_computador(char jog, int nivel);
int encontra_jogada_vencedora(char jog);
int encontra_dupla_vitoria(char jog);
int jogada_basica();
int jogada_intermediaria(char jog);
int jogada_avancada(char jog);
int verifica_ganhador(char jog);
int tabuleiro_cheio();
void joga_campeonato();
void salvar_jogadores(char nome1[], char jog1, char nome2[], char jog2);
int grava_partida(char nomeArquivo[], Partida p);
Partida le_partida(char nomeArquivo[], int numPartida);
void imprime_campeonato(char arqTxt[], char arqBin[]);

int main() {
    int opcao;
    char jog1, jog2;

    srand((unsigned int) time(NULL));
    opcao = menu();

    if (opcao == 3) {
        joga_campeonato();
        return 0;
    }

    if (opcao == 4) {
        printf("Saindo do jogo...\n");
        return 0;
    }

    escolha_simb(&jog1, &jog2);
    joga_partida(opcao, jog1, jog2, 0);
    return 0;
}

int le_inteiro() {
    int valor;
    int resultado;
    int caractere;

    resultado = scanf("%d", &valor);

    // Se a leitura falhar, limpar o buffer de entrada e retornar -1
    if (resultado != 1) {
        do {
            caractere = getchar();
        } while (caractere != '\n' && caractere != EOF);
        return -1;
    }
    return valor;
}

int menu() {
    int opcao = 0;

    while (opcao < 1 || opcao > 4) {
        printf("\n===== JOGO DA VELHA =====\n");
        printf("1. Jogar contra o computador\n");
        printf("2. Jogar contra outro jogador\n");
        printf("3. Jogar campeonato\n");
        printf("4. Sair\n");
        printf("Escolha uma opcao: ");

        opcao = le_inteiro();
        if (opcao < 1 || opcao > 4) printf("Opcao invalida. Tente novamente.\n");
    }

    if (opcao == 1) {
        nivelComputador = 0;

        while (nivelComputador < 1 || nivelComputador > 3) {
            printf("\nNivel do computador:\n");
            printf("1. Basico\n");
            printf("2. Intermediario\n");
            printf("3. Avancado\n");
            printf("Escolha o nivel: ");

            nivelComputador = le_inteiro();
            if (nivelComputador < 1 || nivelComputador > 3) printf("Nivel invalido. Tente novamente.\n");
        }
    }

    return opcao;
}

void escolha_simb(char *jog1, char *jog2) {
    char simbolo;
    int c;

    do {
        printf("\nJogador 1, escolha seu simbolo (X ou O): ");
        if (scanf(" %c", &simbolo) != 1) {
            simbolo = '\0';
        }
        while ((c = getchar()) != '\n' && c != EOF) {
        }

        simbolo = (char) toupper((unsigned char) simbolo);

        if (simbolo != 'X' && simbolo != 'O') printf("Simbolo invalido. Digite X ou O.\n");
    } while (simbolo != 'X' && simbolo != 'O');

    *jog1 = simbolo;

    if (*jog1 == 'X') {
        *jog2 = 'O';
    } else {
        *jog2 = 'X';
    }

    printf("Jogador 1: %c\n", *jog1);
    printf("Jogador 2: %c\n", *jog2);
}

void inicializa_velha() {
    int i, j;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            velha[i][j] = ' ';
        }
    }
}

void mostra_velha() {
    int i, j;
    int posicao;

    printf("\n");

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            posicao = i * 3 + j + 1;

            if (velha[i][j] == ' ') {
                printf(" %d ", posicao);
            } else {
                printf(" %c ", velha[i][j]);
            }

            if (j < 2) printf("|");
        }
        printf("\n");
        if (i < 2) printf("---+---+---\n");
    }
    printf("\n");
}

/* Retorna 0 para jogada valida, 1 para posicao invalida e 2 para casa ocupada. */
int jogada_usuario(int lin, int col, char jog) {
    if (lin < 0 || lin > 2 || col < 0 || col > 2) return 1;
    if (velha[lin][col] != ' ') return 2;
    velha[lin][col] = jog;
    return 0;
}

int verifica_ganhador(char jog) {
    int i;

    /* Linhas e colunas */
    for (i = 0; i < 3; i++) {
        if (velha[i][0] == jog && velha[i][1] == jog && velha[i][2] == jog) return 1;
        if (velha[0][i] == jog && velha[1][i] == jog && velha[2][i] == jog) return 1;
    }

    /* Diagonais */
    if (velha[0][0] == jog && velha[1][1] == jog && velha[2][2] == jog) return 1;
    if (velha[0][2] == jog && velha[1][1] == jog && velha[2][0] == jog) return 1;
    return 0;
}

/* Retorna 1 quando nao ha casas vazias. */
int tabuleiro_cheio() {
    int i, j;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (velha[i][j] == ' ') return 0;
        }
    }
    return 1;
}

/* Retorna a posicao de vitoria ou 0 se nao houver. */
int encontra_jogada_vencedora(char jog) {
    int i, j;
    int posicao;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (velha[i][j] == ' ') {
                velha[i][j] = jog;

                if (verifica_ganhador(jog)) {
                    velha[i][j] = ' ';
                    posicao = i * 3 + j + 1;
                    return posicao;
                }

                velha[i][j] = ' ';
            }
        }
    }
    return 0;
}

void jogada_computador(char jog, int nivel) {
    int posicao;
    int lin, col;

    if (nivel == 1) {
        posicao = jogada_basica();
    } else if (nivel == 2) {
        posicao = jogada_intermediaria(jog);
    } else {
        posicao = jogada_avancada(jog);
    }
    if (posicao == 0) return;

    lin = (posicao - 1) / 3;
    col = (posicao - 1) % 3;
    jogada_usuario(lin, col, jog);
    printf("O computador escolheu a posicao %d.\n", posicao);
}

// Nivel basico: o computador escolhe uma casa vazia aleatoria.
// Nivel intermediario: o computador tenta vencer, bloquear uma vitoria adversaria,
// ocupar o centro e escolher um canto.
// Nivel avancado: a implementacao apresentada ainda e uma estraegia heuristica simples,
// semelhante a intermediaria; nao garante a melhor jogada em todas as situacoes.

int jogada_basica() {
    int casas[9];
    int quantidade = 0;
    int i, j;
    int escolha;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (velha[i][j] == ' ') {
                casas[quantidade] = i * 3 + j + 1;
                quantidade++;
            }
        }
    }
    if (quantidade == 0) return 0;
    escolha = rand() % quantidade;
    return casas[escolha];
}

/* Tenta vencer, bloquear, ocupar o centro ou um canto. */
int jogada_intermediaria(char jog) {
    char adversario;
    int posicao;
    int cantos[4] = {1, 3, 7, 9};
    int quantidade = 4;
    int escolha;
    int lin, col;

    adversario = (jog == 'X') ? 'O' : 'X';

    posicao = encontra_jogada_vencedora(jog);
    if (posicao != 0) return posicao;

    posicao = encontra_jogada_vencedora(adversario);
    if (posicao != 0) return posicao;

    if (velha[1][1] == ' ') return 5;

    while (quantidade > 0) {
        escolha = rand() % quantidade;
        posicao = cantos[escolha];
        lin = (posicao - 1) / 3;
        col = (posicao - 1) % 3;

        if (velha[lin][col] == ' ') return posicao;

        cantos[escolha] = cantos[quantidade - 1];
        quantidade--;
    }

    return jogada_basica();
}

int jogada_avancada(char jog) {
    int posicao;
    int i;
    int laterais[4] = {2, 4, 6, 8};
    int linha, coluna;
    char adversario;

    adversario = (jog == 'X') ? 'O' : 'X';

    posicao = encontra_jogada_vencedora(jog);
    if (posicao != 0) return posicao;

    posicao = encontra_jogada_vencedora(adversario);
    if (posicao != 0) return posicao;

    if (velha[1][1] == ' ') return 5;

    if (velha[1][1] == jog &&
        ((velha[0][0] == adversario && velha[2][2] == adversario) ||
         (velha[0][2] == adversario && velha[2][0] == adversario))) {
        for (i = 0; i < 4; i++) {
            posicao = laterais[i];
            linha = (posicao - 1) / 3;
            coluna = (posicao - 1) % 3;

            if (velha[linha][coluna] == ' ') return posicao;
        }
    }

    posicao = encontra_dupla_vitoria(jog);
    if (posicao != 0) return posicao;

    posicao = encontra_dupla_vitoria(adversario);
    if (posicao != 0) return posicao;

    for (i = 0; i < 4; i++) {
        posicao = laterais[i] - 1;
        if (velha[posicao / 3][posicao % 3] == ' ') return laterais[i];
    }

    return jogada_basica();
}

int encontra_dupla_vitoria(char jog) {
    int i, j;
    int lin, col;
    int possibilidadesVitoria;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (velha[i][j] == ' ') {
                velha[i][j] = jog;
                possibilidadesVitoria = 0;

                for (lin = 0; lin < 3; lin++) {
                    for (col = 0; col < 3; col++) {
                        if (velha[lin][col] == ' ') {
                            velha[lin][col] = jog;

                            if (verifica_ganhador(jog)) possibilidadesVitoria++;

                            velha[lin][col] = ' ';
                        }
                    }
                }
                velha[i][j] = ' ';
                if (possibilidadesVitoria >= 2) return i * 3 + j + 1;
            }
        }
    }

    return 0;
}

void joga_partida(int opcao, char jog1, char jog2, int salvarCampeonato) {
    char turno = 'X';
    int posicao;
    int resultado;
    int lin, col;
    Partida p;
    int i, j;
    int venceu = 0;

    inicializa_velha();

    while (1) {
        mostra_velha();

        if (opcao == 1 && turno == jog2) {
            printf("Vez do computador (%c).\n", turno);
            jogada_computador(turno, nivelComputador);
        } else {
            if (turno == jog1) {
                printf("Vez do jogador 1 (%c).\n", turno);
            } else {
                printf("Vez do jogador 2 (%c).\n", turno);
            }

            do {
                printf("Escolha uma posicao (1-9): ");
                posicao = le_inteiro();

                if (posicao < 1 || posicao > 9) {
                    printf("Posicao invalida. Tente novamente.\n");
                    resultado = 1;
                } else {
                    lin = (posicao - 1) / 3;
                    col = (posicao - 1) % 3;
                    resultado = jogada_usuario(lin, col, turno);
                    if (resultado == 2) printf("Essa casa ja esta preenchida.\n");
                }
            } while (resultado != 0);
        }

        if (verifica_ganhador(turno)) {
            mostra_velha();
            venceu = 1;
            if (opcao == 1 && turno == jog2) {
                printf("O computador venceu!\n");
            } else if (turno == jog1) {
                printf("Jogador 1 venceu!\n");
            } else {
                printf("Jogador 2 venceu!\n");
            }
            break;
        }

        if (tabuleiro_cheio()) {
            mostra_velha();
            printf("Empate! Nao ha mais casas vazias.\n");
            break;
        }

        turno = (turno == 'X') ? 'O' : 'X';
    }

    if (salvarCampeonato) {
        contadorPartidas++;
        p.partida = contadorPartidas;
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                p.JogVelha[i][j] = velha[i][j];
            }
        }
        p.resultado = venceu ? turno : 'V';

        if (!grava_partida(ARQ_PARTIDAS, p)) {
            printf("Erro ao gravar a partida no arquivo binario.\n");
            exit(1);
        }
    }
}

void joga_campeonato() {
    FILE *arqPartidas;
    char jog1, jog2;
    char continuar;

    getchar();

    do {
        printf("\nDigite o nome do Jogador 1: ");
        fgets(nomeJogador1, sizeof(nomeJogador1), stdin);
        nomeJogador1[strcspn(nomeJogador1, "\r\n")] = '\0';
        if (nomeJogador1[0] == '\0') {
            printf("Nome invalido. Digite novamente.\n");
        }
    } while (nomeJogador1[0] == '\0');

    do {
        printf("Digite o nome do Jogador 2: ");
        fgets(nomeJogador2, sizeof(nomeJogador2), stdin);
        nomeJogador2[strcspn(nomeJogador2, "\r\n")] = '\0';
        if (nomeJogador2[0] == '\0') {
            printf("Nome invalido. Digite novamente.\n");
        }
    } while (nomeJogador2[0] == '\0');

    escolha_simb(&jog1, &jog2);
    salvar_jogadores(nomeJogador1, jog1, nomeJogador2, jog2);

    arqPartidas = fopen(ARQ_PARTIDAS, "wb");
    if (arqPartidas == NULL) {
        printf("Erro ao criar o arquivo binario do campeonato.\n");
        return;
    }
    fclose(arqPartidas);

    contadorPartidas = 0;

    do {
        joga_partida(2, jog1, jog2, 1);

        do {
            printf("\nDeseja jogar outra partida no campeonato? (S/N): ");
            scanf(" %c", &continuar);
            continuar = (char) toupper((unsigned char) continuar);

            if (continuar != 'S' && continuar != 'N') {
                printf("Entrada invalida. Digite S ou N.\n");
            }
        } while (continuar != 'S' && continuar != 'N');
    } while (continuar == 'S');

    imprime_campeonato(ARQ_JOGADORES, ARQ_PARTIDAS);
}

/* Grava no arquivo txt oo formato: Nome do Jogador 1; símbolo; Nome do Jogador 2; símbolo */
void salvar_jogadores(char nome1[], char jog1, char nome2[], char jog2) {
    FILE *arquivo;
    int resultado;

    arquivo = fopen(ARQ_JOGADORES, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para salvar os jogadores.\n");
        exit(1);
    }

    resultado = fprintf(arquivo, "%s; %c; %s; %c\n", nome1, jog1, nome2, jog2);
    if (fclose(arquivo) != 0 || resultado < 0) {
        printf("Erro ao gravar os jogadores no arquivo.\n");
        exit(1);
    }
}

/* Grava uma partida no arquivo binario, retorna 1 se sucesso e 0 caso contrario. */
int grava_partida(char nomeArquivo[], Partida p) {
    FILE *arquivo;

    arquivo = fopen(nomeArquivo, "ab");
    if (arquivo == NULL) {
        return 0;
    }

    if (fwrite(&p, sizeof(Partida), 1, arquivo) != 1) {
        fclose(arquivo);
        return 0;
    }

    fclose(arquivo);
    return 1;
}

/* Le do arquivo binario os dados de uma determinada partida. */
Partida le_partida(char nomeArquivo[], int numPartida) {
    FILE *arquivo;
    Partida p;
    int i, j;

    p.partida = 0;
    p.resultado = ' ';
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            p.JogVelha[i][j] = ' ';
        }
    }

    arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de partidas.\n");
        return p;
    }

    if (numPartida >= 1 && fseek(arquivo, (long)(numPartida - 1) * sizeof(Partida), SEEK_SET) == 0) {
        if (fread(&p, sizeof(Partida), 1, arquivo) != 1) {
            p.partida = 0;
        }
    }

    fclose(arquivo);
    return p;
}

/* Le o arquivo txt e o arquivo binario para imprimir todas as partidas e o placar final. */
void imprime_campeonato(char arqTxt[], char arqBin[]) {
    FILE *fTxt;
    FILE *fBin;
    char jog1Nome[50], jog2Nome[50];
    char simb1, simb2;
    char *nome, *simbolo;
    Partida p;
    int pontos1 = 0, pontos2 = 0, velhas = 0;
    int totalPartidas = 0, num;
    int i, j, caractere, posicao;

    fTxt = fopen(arqTxt, "r");
    if (fTxt == NULL) {
        printf("\nArquivo de jogadores nao encontrado.\n");
        return;
    }

    for (i = 0; i < 2; i++) {
        if (i == 0) {
            nome = jog1Nome;
            simbolo = &simb1;
        } else {
            nome = jog2Nome;
            simbolo = &simb2;
        }

        posicao = 0;
        caractere = getc(fTxt);
        while (caractere != ';' && caractere != '\n' && caractere != EOF && posicao < 49) {
            nome[posicao] = (char) caractere;
            posicao++;
            caractere = getc(fTxt);
        }
        nome[posicao] = '\0';

        if (posicao == 0 || caractere != ';' || fscanf(fTxt, " %c", simbolo) != 1 ||
            (i == 0 && (getc(fTxt) != ';' || getc(fTxt) != ' '))) {
            fclose(fTxt);
            printf("\nArquivo de jogadores invalido.\n");
            return;
        }
    }
    fclose(fTxt);

    fBin = fopen(arqBin, "rb");
    if (fBin == NULL) {
        printf("\nO arquivo nao pode ser aberto!\n");
        return;
    }

    fseek(fBin, 0, SEEK_END);
    totalPartidas = (int)(ftell(fBin) / (long) sizeof(Partida));
    fclose(fBin);

    printf("\n===========================================\n");
    printf("        CAMPEONATO DE JOGO DA VELHA        \n");
    printf("===========================================\n");
    printf("%s (%c)  vs  %s (%c)\n", jog1Nome, simb1, jog2Nome, simb2);

    for (num = 1; num <= totalPartidas; num++) {
        p = le_partida(arqBin, num);
        if (p.partida == 0) continue;

        printf("\n--- Partida %d ---\n", p.partida);

        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                printf(" %c ", p.JogVelha[i][j]);
                if (j < 2) printf("|");
            }
            printf("\n");
            if (i < 2) printf("---+---+---\n");
        }

        if (p.resultado == simb1) {
            pontos1++;
            printf("Resultado: Vitoria de %s (%c)!\n", jog1Nome, simb1);
        } else if (p.resultado == simb2) {
            pontos2++;
            printf("Resultado: Vitoria de %s (%c)!\n", jog2Nome, simb2);
        } else {
            velhas++;
            printf("Resultado: Velha (empate)\n");
        }
    }

    printf("\n===========================================\n");
    printf("PLACAR FINAL: %s %d X %d %s\n", jog1Nome, pontos1, pontos2, jog2Nome);
    printf("Empates: %d\n", velhas);

    if (pontos1 > pontos2) {
        printf("%s foi campea(o) do Campeonato de Jogo da Velha!!!\n", jog1Nome);
    } else if (pontos2 > pontos1) {
        printf("%s foi campea(o) do Campeonato de Jogo da Velha!!!\n", jog2Nome);
    } else {
        printf("O Campeonato de Jogo da Velha terminou empatado!\n");
    }
    printf("===========================================\n");
}