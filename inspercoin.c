// sudo apt-get install libsodium-dev
// sudo apt-get install libjansson-dev
// gcc -g -Og inspercoin.c lib/key/key.c lib/coin/coin.c -o ic -lsodium -lcurl -ljansson -Wno-discarded-qualifiers

#include <sodium.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include <curl/curl.h>
#include <signal.h>
#include "lib/key/key.h"
#include "lib/coin/coin.h"

void update_env()
{
    FILE *file;
    char line[256];
    char variable[256];
    char value[256];
    file = fopen("config.ic", "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: config.ic file not found!\n");
        exit(EXIT_FAILURE);
    }
    while (fgets(line, sizeof(line), file))
    {
        sscanf(line, "%[^=]=%s", variable, value);
        setenv(variable, value, 1);
        //printf("Variable %s is set to %s\n", variable, value);
    }
    fclose(file);
}

void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        struct sigaction handler;
        handler.sa_handler = SIG_DFL;
        sigemptyset(&handler.sa_mask);
        handler.sa_flags = 0;
        sigaction(SIGINT, &handler, NULL);
        printf("Encerrando mineração!\n");
        fflush(stdout);
        raise(SIGINT);
    }
}

int main(int argc, char *argv[])
{
    struct sigaction handler;
    handler.sa_handler = sig_handler;
    sigemptyset(&handler.sa_mask);
    handler.sa_flags = 0;
    sigaction(SIGINT, &handler, NULL);
    init_keyring_env();
    update_env();
    if (argc == 4 &&
        strcmp(argv[1], "criar") == 0 &&
        strcmp(argv[2], "carteira") == 0)
    {
        create_wallet(argv[3]);
        return 0;
    }
    else if (argc == 9 &&
             strcmp(argv[1], "enviar") == 0 &&
             strcmp(argv[3], "para") == 0 &&
             strcmp(argv[4], "endereco") == 0 &&
             strcmp(argv[6], "com") == 0 &&
             strcmp(argv[7], "recompensa") == 0)
    {
        send_money(argv[2], getenv("DEFAULT_WALLET"), (unsigned char *)argv[5], argv[8]);
    }
    else if (argc == 12 &&
             strcmp(argv[1], "enviar") == 0 &&
             strcmp(argv[3], "da") == 0 &&
             strcmp(argv[4], "carteira") == 0 &&
             strcmp(argv[6], "para") == 0 &&
             strcmp(argv[7], "endereco") == 0 &&
             strcmp(argv[9], "com") == 0 &&
             strcmp(argv[10], "recompensa") == 0)
    {
        // ENVIAR GRANA
        // ./inspercoin enviar 0.01 da carteira rico para endereco 4B904AEACACD702908BF822AB1A0FBF0A571C3B2E38C22DD5D67DBC15993D1A7 com recompensa 0.001
        send_money(argv[2], argv[5], (unsigned char *)argv[8], argv[11]);
    }
    else if (argc == 6 &&
             strcmp(argv[1], "minerar") == 0 &&
             strcmp(argv[2], "transacao") == 0 &&
             strcmp(argv[3], "na") == 0 &&
             strcmp(argv[4], "carteira") == 0)
    {
        mine_transaction(argv[5], 0);
    }
    else if (argc == 3 &&
             strcmp(argv[1], "minerar") == 0 &&
             strcmp(argv[2], "transacao") == 0)
    {
        mine_transaction(getenv("DEFAULT_WALLET"), 0);
    }
    else if (argc == 4 &&
             strcmp(argv[1], "minerar") == 0 &&
             strcmp(argv[3], "transacoes") == 0)
    {
        mine_n_transactions(getenv("DEFAULT_WALLET"), argv[2], argv[2]);
    }
    else if (argc == 7 &&
             strcmp(argv[1], "minerar") == 0 &&
             strcmp(argv[3], "transacoes") == 0 &&
             strcmp(argv[4], "em") == 0 &&
             strcmp(argv[6], "processos") == 0)
    {
        mine_n_transactions(getenv("DEFAULT_WALLET"), argv[2], argv[5]);
    }
    else if (argc == 5 &&
             strcmp(argv[1], "minerar") == 0 &&
             strcmp(argv[2], "em") == 0 &&
             strcmp(argv[4], "processos") == 0)
    {
        mine_continuous(getenv("DEFAULT_WALLET"), argv[3]);
    }
    else if (argc == 8 &&
             strcmp(argv[1], "minerar") == 0 &&
             strcmp(argv[2], "em") == 0 &&
             strcmp(argv[4], "processos") == 0 &&
             strcmp(argv[5], "na") == 0 &&
             strcmp(argv[6], "carteira") == 0)
    {
        mine_continuous(argv[7], argv[3]);
    }
    else if (argc == 8 &&
             strcmp(argv[1], "verificar") == 0)
    { // VERIFICAR SE TRANSACAO É VÁLIDA
        // ./inspercoin verificar 20230427003846 E9AE8BF0871DB8F50E58DEF4C2D230B31B72155D97978A486FC6469053BAD23A E9AE8BF0871DB8F50E58DEF4C2D230B31B72155D97978A486FC6469053BAD23A 99999.00000 00000.00000 B28D62649379673982702C10EB1C1735C28FEBF9B871DE9FBB4EBD05CB47A17C5AF521355FB4FCCDE77BE6E289EF8CDBAF483037A86CEB4C8268397BDA07110B
        // ./inspercoin verificar <data> <origem> <destino> <valor> <recompensa> <assinatura>
        return validate_transaction(argv[2],
                                    argv[5],
                                    (unsigned char *)argv[3],
                                    (unsigned char *)argv[4],
                                    argv[6],
                                    (unsigned char *)argv[7]);
    }
    else if (argc == 11 &&
             strcmp(argv[1], "verificar") == 0) // corrigir para validar hash
    {
        // verificar se bloco é válido
        // ./inspercoin verificar 20230427003846 E9AE8BF0871DB8F50E58DEF4C2D230B31B72155D97978A486FC6469053BAD23A E9AE8BF0871DB8F50E58DEF4C2D230B31B72155D97978A486FC6469053BAD23A 99999.00000 00000.00000 B28D62649379673982702C10EB1C1735C28FEBF9B871DE9FBB4EBD05CB47A17C5AF521355FB4FCCDE77BE6E289EF8CDBAF483037A86CEB4C8268397BDA07110B  00000000000000149984 00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 00000E1DA0299B730000000000000000000000000000000000000000000000000000000000000000910000000000000030303030304531444130323939423733
        // ./inspercoin verificar <data> <origem> <destino> <valor> <recompensa> <assinatura_transacao> <nonce> <previous_hash> <hash_block>
        return validate_block(argv[2],
                              argv[5],
                              (unsigned char *)argv[3],
                              (unsigned char *)argv[4],
                              argv[6],
                              (unsigned char *)argv[7],
                              argv[8],
                              (unsigned char *)argv[9],
                              (unsigned char *)argv[10]);
    }
    else
    {
        printf("USAGE:\n");
        printf("./inspercoin criar carteira <name_carteira>\n");
        printf("./inspercoin enviar <valor> para endereco <endereco> (DEFAULT_WALLET)\n");
        printf("./inspercoin enviar <valor> da carteira <carteira> para endereco <endereco>\n");
        printf("./inspercoin minerar transacao na carteira <carteira>\n");
        printf("./inspercoin minear transacao (DEFAULT_WALLET)\n");
        printf("./inspercoin minear <qtde> transacoes\n");
        printf("./inspercoin minear <qtde_t> transacoes em <qtde_t> processos\n");
    }
    return 0;
}