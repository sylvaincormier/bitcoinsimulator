#include "sugarmine.hpp"


void sugarmine::onTransfer(name from, name to, asset quantity, string memo)
{
   if (from == get_self() || to != get_self() || from == TOKEN_ACC) {
        return;
   }
   if(memo == "SUGAR"){
      _stake(from, quantity);
   }
   else if(memo == "SUGARA"){
      _stake(from, quantity);
   }
}

void sugarmine::_stake(name account, asset quantity)
{
   auto now = current_time_point().sec_since_epoch();
   mining_stake mining_table(get_self(), account.value);
   
   mining_table.emplace(get_self(), [&](auto& row){
        row.id = mining_table.available_primary_key();
        row.account = account;
        row.amount = quantity;
        row.unlock_timestamp = now + STAKE_PERIOD;
      });   
}

void sugarmine::unstake_unlocked(name account, uint64_t timestamp)
{
   asset to_transfer = asset(0, symbol(TOKEN_SYMBOL, SYMBOL_PRE_DIGIT));
   mining_stake mining_table(get_self(), account.value);
   auto itr = mining_table.begin();

   while (itr != mining_table.end()) {
      // if stake unlock timestamp is bigger just skip 
      if (timestamp < itr->unlock_timestamp) {
         itr++;
      // else transfer funds and erase record
      } else {
         to_transfer += itr->amount;
         itr = mining_table.erase(itr);
      }
   }

   string memo = "withdraw";
   action(
      permission_level{get_self(), "active"_n},
      TOKEN_ACC,
      "transfer"_n,
      std::make_tuple(get_self(), account, to_transfer, memo)
   ).send();  
}

void sugarmine::unstake(name owner){
   check(has_auth(owner) || has_auth(get_self()), "Auth failed");
   auto now = current_time_point().sec_since_epoch();
   unstake_unlocked(owner, now);
}

