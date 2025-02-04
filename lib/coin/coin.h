#ifndef __INSPERCOIN_COIN_H__
#define __INSPERCOIN_COIN_H__

void broadcast_transaction(char *date_transaction,
                           unsigned char *address_from,
                           unsigned char *address_to,
                           char *amount,
                           char *reward,
                           unsigned char *signature);
void broadcast_block(
    unsigned char *hash,
    unsigned char *previous_hash,
    unsigned char *miner_address,
    long id_transaction,
    long nonce);
int validate_transaction(char *date_transaction,
                         char *amount,
                         unsigned char *address_from,
                         unsigned char *address_to,
                         char *reward,
                         unsigned char *signature);
int validate_block(char *date_transaction,
                   char *amount,
                   unsigned char *address_from,
                   unsigned char *address_to,
                   char *reward,
                   unsigned char *hash,
                   char *nonce,
                   unsigned char *previous_hash,
                   unsigned char *block_hash);
char *format_amount(double amount);
char *timestring();
void send_money(char *amount, char *wallet, unsigned char *address_to, char *reward);
void mine_transaction(char wallet_name[65], size_t i_transaction);
void mine_n_transactions(char wallet_name[65], char *n, char *processes);
void mine_continuous(char wallet_name[65], char *processes);
void sig_handler(int signo);

#endif