#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>
#include <eosio/eosio.hpp>
#include <eosio/transaction.hpp>
#include <eosio/singleton.hpp>
#include <string>

#define SYMBOL_PRE_DIGIT 8
#define TOKEN_SYMBOL "SGR"
#define TOKEN_ACC name("sugartoken11")
#define MIN_STAKE_AMOUNT asset(100000000000, symbol(TOKEN_SYMBOL, SYMBOL_PRE_DIGIT))
#define MAX_STAKE_AMOUNT asset(1000000000000000, symbol(TOKEN_SYMBOL, SYMBOL_PRE_DIGIT))
#define STAKE_PERIOD  24 * 60 * 60

using namespace eosio;
using std::string;

class[[eosio::contract("sugarmine")]] sugarmine : public contract
{
public:
   using contract::contract;

   sugarmine(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds),
                                                                     _mining(receiver, receiver.value){}
                                                                      
   [[eosio::on_notify("sugartoken11::transfer")]] 
   void onTransfer(name from,
                   name to,
                   asset quantity,
                   string memo);
   
   [[eosio::action]] 
   void unstake(name owner);

private:
   void _stake(name account, asset quantity);
   
   void unstake_unlocked(name account, uint64_t timestamp);
   
   struct [[eosio::table]] miningpool
   {
      uint64_t id;
      name account;
      asset amount;
      uint32_t unlock_timestamp;
      uint64_t primary_key() const { return id; }
   };

   typedef eosio::multi_index<name("miningpool"), miningpool> mining_stake;
   mining_stake _mining;
   
};