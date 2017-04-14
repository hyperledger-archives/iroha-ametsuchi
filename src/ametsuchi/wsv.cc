/**
 * Copyright Soramitsu Co., Ltd. 2016 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ametsuchi/wsv.h>
#include <ametsuchi/comparator.h>
#include <ametsuchi/generated/transaction_generated.h>
#include <ametsuchi/currency.h>

namespace ametsuchi {

void WSV::init(MDB_txn *append_tx) {

  append_tx_ = append_tx;
  // [pubkey] => assets (DUP)
  trees_["wsv_pubkey_assets"] = init_btree(append_tx_, "wsv_pubkey_assets", MDB_DUPSORT | MDB_DUPFIXED | MDB_CREATE,
             comparator::cmp_assets);

  // [pubkey] => account (NODUP)
  trees_["wsv_pubkey_account"] = init_btree(append_tx_, "wsv_pubkey_account", MDB_CREATE);

  // [ledger_name+domain_name+asset_name] => creator public key (NODUP)
  trees_["wsv_assetid_asset"] = init_btree(append_tx_, "wsv_assetid_asset", MDB_CREATE);

  // [ip] => peer (NODUP)
  trees_["wsv_ip_peer"] = init_btree(append_tx_, "wsv_ip_peer", MDB_CREATE);

  // we should know created assets, so read entire table in memory
  read_created_assets();

}

void WSV::update(const flatbuffers::Vector<uint8_t> *blob) {
  auto tx = flatbuffers::GetRoot<iroha::Transaction>(blob->data());
  // 4. update WSV
  {
    switch (tx->command_type()) {
      case iroha::Command::AssetCreate: {
        asset_create(tx->command_as_AssetCreate());
        break;
      }
      case iroha::Command::AssetAdd: {
        asset_add(tx->command_as_AssetAdd());
        break;
      }
      case iroha::Command::AssetRemove: {
        asset_remove(tx->command_as_AssetRemove());
        break;
      }
      case iroha::Command::AssetTransfer: {
        asset_transfer(tx->command_as_AssetTransfer());
        break;
      }
      case iroha::Command::AccountAdd: {
        account_add(tx->command_as_AccountAdd());
        break;
      }
      case iroha::Command::AccountRemove: {
        account_remove(tx->command_as_AccountRemove());
        break;
      }
      case iroha::Command::PeerAdd: {
        peer_add(tx->command_as_PeerAdd());
        break;
      }
      case iroha::Command::PeerRemove: {
        peer_remove(tx->command_as_PeerRemove());
        break;
      }
      default: {
        console->critical("Not implemented. Yet.");
        throw exception::InternalError::NOT_IMPLEMENTED;
      }
    }
  }

}
WSV::WSV() {

}
WSV::~WSV() {

}

void WSV::read_created_assets() {
  auto records = read_all_records(trees_["wsv_assetid_asset"].second);
  created_assets_.clear();
  for (auto &&asset : records) {
    std::string assetid{(char *)asset.first.data,
                        (char *)asset.first.data + asset.first.size};

    std::vector<uint8_t> assetfb{(char *)asset.second.data,
                                 (char *)asset.second.data + asset.second.size};

    created_assets_[assetid] = assetfb;
  }

}


void WSV::close_cursors() {
  for (auto &&e : trees_) {
    MDB_cursor *cursor = e.second.second;
    if (cursor)
      mdb_cursor_close(cursor);
  }
}

// WSV commands:
void WSV::asset_create(const iroha::AssetCreate *command) {
  MDB_val c_key, c_val;
  int res;

  auto ln = command->ledger_name();
  auto dn = command->domain_name();
  auto an = command->asset_name();

  // in this order: ledger+domain+asset
  std::string pk;
  pk += ln->data();
  pk += dn->data();
  pk += an->data();

  // create Asset
  flatbuffers::FlatBufferBuilder fbb;
  auto asset =
      iroha::CreateCurrencyDirect(fbb, an->data(), dn->data(), ln->data());
  fbb.Finish(asset);

  auto ptr = fbb.GetBufferPointer();

  c_key.mv_data = (void *)(pk.c_str());
  c_key.mv_size = reinterpret_cast<size_t>(pk.size());
  c_val.mv_data = (void *)ptr;
  c_val.mv_size = fbb.GetSize();

  // Put and sort by assetid
  if ((res = mdb_cursor_put(trees_.at("wsv_assetid_asset").second, &c_key,
                            &c_val, 0))) {
    if (res == MDB_KEYEXIST) {
      throw exception::InvalidTransaction::ASSET_EXISTS;
    }
    AMETSUCHI_CRITICAL(res, MDB_MAP_FULL);
    AMETSUCHI_CRITICAL(res, MDB_TXN_FULL);
    AMETSUCHI_CRITICAL(res, EACCES);
    AMETSUCHI_CRITICAL(res, EINVAL);
  }

  created_assets_[pk] = std::vector<uint8_t>{ptr, ptr + fbb.GetSize()};
}

void WSV::asset_add(const iroha::AssetAdd *command) {
  // Now only Currency is supported
  if (command->asset_nested_root()->asset_type() != iroha::AnyAsset::Currency)
    throw exception::InternalError::NOT_IMPLEMENTED;

  account_add_currency(command->accPubKey(),
                             (iroha::Currency *)command->asset()->data(),
                             command->asset()->size());

}


void WSV::asset_remove(const iroha::AssetRemove *command) {
  // Now only Currency is supported
  if (command->asset_nested_root()->asset_type() != iroha::AnyAsset::Currency)
    throw exception::InternalError::NOT_IMPLEMENTED;

  this->account_remove_currency(command->accPubKey(),
                                (iroha::Currency *)command->asset()->data());
}

void WSV::account_add_currency(const flatbuffers::String *acc_pub_key,
                                     const iroha::Currency *c, size_t c_size) {
  int res;
  MDB_val c_key, c_val;
  auto cursor = trees_.at("wsv_pubkey_assets").second;
  std::vector<uint8_t> copy;

  try {
    // may throw ASSET_NOT_FOUND
    AM_val account_asset =
        accountGetAsset(acc_pub_key, c->ledger_name(), c->domain_name(),
                              c->currency_name());

    assert(c_size == account_asset.size);

    // asset exists, change it:

    copy = {(char *)account_asset.data,
            (char *)account_asset.data + account_asset.size};

    // update the copy
    auto copy_fb = flatbuffers::GetMutableRoot<iroha::Currency>(copy.data());

    Currency current(copy_fb->amount(), copy_fb->precision());
    Currency delta(c->amount(), c->precision());
    Currency new_state = current + delta;

    copy_fb->mutate_amount(new_state.get_amount());
    copy_fb->mutate_precision(new_state.get_precision());

  } catch (exception::InvalidTransaction e) {
    // Create new Asset
    if (e == exception::InvalidTransaction::ASSET_NOT_FOUND) {
      copy = {(char *)c, (char *)c + c_size};
    } else {
      throw;
    }
  }

  // write to tree
  c_key.mv_data = (void *)acc_pub_key->data();
  c_key.mv_size = acc_pub_key->size();
  c_val.mv_data = (void *)copy.data();
  c_val.mv_size = copy.size();

  // cursor is at the correct asset, just replace with a copy of FB and flag
  // MDB_CURRENT
  // TODO: Check if this works for a new Asset (It may replace some random old asset)
  if ((res = mdb_cursor_put(cursor, &c_key, &c_val, MDB_CURRENT))) {
    AMETSUCHI_CRITICAL(res, MDB_KEYEXIST);
    AMETSUCHI_CRITICAL(res, MDB_MAP_FULL);
    AMETSUCHI_CRITICAL(res, MDB_TXN_FULL);
    AMETSUCHI_CRITICAL(res, EACCES);
    AMETSUCHI_CRITICAL(res, EINVAL);
  }
}

void WSV::account_remove_currency(const flatbuffers::String *acc_pub_key,
                                        const iroha::Currency *c) {
  int res;
  MDB_val c_key, c_val;
  auto cursor = trees_.at("wsv_pubkey_assets").second;
  std::vector<uint8_t> copy;

  // may throw ASSET_NOT_FOUND
  AM_val account_asset =
      accountGetAsset(acc_pub_key, c->ledger_name(), c->domain_name(),
                            c->currency_name());

  // asset exists, change it:

  copy = {(char *)account_asset.data,
          (char *)account_asset.data + account_asset.size};

  // update the copy
  auto copy_fb = flatbuffers::GetMutableRoot<iroha::Currency>(copy.data());

  Currency current(copy_fb->amount(), copy_fb->precision());
  Currency delta(c->amount(), c->precision());
  if (current < delta) throw exception::InvalidTransaction::NOT_ENOUGH_ASSETS;
  Currency new_state = current - delta;

  copy_fb->mutate_amount(new_state.get_amount());
  copy_fb->mutate_precision(new_state.get_precision());

  // write to tree
  c_key.mv_data = (void *)acc_pub_key->data();
  c_key.mv_size = acc_pub_key->size();
  c_val.mv_data = (void *)copy.data();
  c_val.mv_size = copy.size();

  // cursor is at the correct asset, just replace with a copy of FB and flag
  // MDB_CURRENT
  if ((res = mdb_cursor_put(cursor, &c_key, &c_val, MDB_CURRENT))) {
    AMETSUCHI_CRITICAL(res, MDB_KEYEXIST);
    AMETSUCHI_CRITICAL(res, MDB_MAP_FULL);
    AMETSUCHI_CRITICAL(res, MDB_TXN_FULL);
    AMETSUCHI_CRITICAL(res, EACCES);
    AMETSUCHI_CRITICAL(res, EINVAL);
  }
}

void WSV::asset_transfer(const iroha::AssetTransfer *command) {
  // Now only currency is supported
  if (command->asset_nested_root()->asset_type() != iroha::AnyAsset::Currency)
    throw exception::InternalError::NOT_IMPLEMENTED;

  auto asset_fb = flatbuffers::GetRoot<iroha::Asset>(command->asset());
  auto currency = asset_fb->asset_as_Currency();

  this->account_remove_currency(command->sender(), currency);
  this->account_add_currency(command->receiver(), currency,
                             command->asset()->size());
}


AM_val WSV::accountGetAsset(const flatbuffers::String *pubKey,
                            const flatbuffers::String *ln,
                            const flatbuffers::String *dn,
                            const flatbuffers::String *cn,
                            bool uncommitted, MDB_env* env) {
  MDB_val c_key, c_val;
  MDB_cursor *cursor;
  MDB_txn *tx;
  int res;

  std::string pk;
  pk += ln->data();
  pk += dn->data();
  pk += cn->data();

  // if given asset exists, then we can get its blob, which consists of {ledger
  // name, domain name and asset name} to speedup read in DUP btree, because we
  // have custom comparator
  auto blob = created_assets_.find(pk);
  if (blob != created_assets_.end()) {
    // asset found, use asset's flatbuffer to find asset in account faster
    c_val.mv_data = (void *)blob->second.data();
    c_val.mv_size = blob->second.size();
  } else {
    throw exception::InvalidTransaction::ASSET_NOT_FOUND;
  }

  // depending on 'uncommitted' we use RO or RW transaction
  if (uncommitted) {
    // reuse existing cursor and "append" transaction
    cursor = trees_.at("wsv_pubkey_assets").second;
    tx = append_tx_;

  } else {
    // create read-only transaction, create new RO cursor
    if ((res = mdb_txn_begin(env, NULL, MDB_RDONLY, &tx))) {
      AMETSUCHI_CRITICAL(res, MDB_PANIC);
      AMETSUCHI_CRITICAL(res, MDB_MAP_RESIZED);
      AMETSUCHI_CRITICAL(res, MDB_READERS_FULL);
      AMETSUCHI_CRITICAL(res, ENOMEM);
    }

    if ((res ==
        mdb_cursor_open(tx, trees_.at("wsv_pubkey_assets").first, &cursor))) {
      AMETSUCHI_CRITICAL(res, EINVAL);
    }
  }

  // query asset by public key
  c_key.mv_data = (void *)pubKey->data();
  c_key.mv_size = pubKey->size();

  // if sender has no such asset, then it is incorrect transaction
  if ((res = mdb_cursor_get(cursor, &c_key, &c_val, MDB_GET_BOTH))) {
    if (res == MDB_NOTFOUND)
      throw exception::InvalidTransaction::ASSET_NOT_FOUND;
    AMETSUCHI_CRITICAL(res, EINVAL);
  }

  return AM_val(c_val);
}


void WSV::account_add(const iroha::AccountAdd *command) {
  MDB_val c_key, c_val;
  int res;

  auto account =
      flatbuffers::GetRoot<iroha::Account>(command->account()->data());
  auto pubkey = account->pubKey();

  c_key.mv_data = (void *)(pubkey->data());
  c_key.mv_size = pubkey->size();
  c_val.mv_data = (void *)command->account()->data();
  c_val.mv_size = command->account()->size();

  if ((res = mdb_cursor_put(trees_.at("wsv_pubkey_account").second, &c_key,
                            &c_val, 0))) {
    // account with this public key exists
    if (res == MDB_KEYEXIST) {
      throw exception::InvalidTransaction::ACCOUNT_EXISTS;
    }
    AMETSUCHI_CRITICAL(res, MDB_MAP_FULL);
    AMETSUCHI_CRITICAL(res, MDB_TXN_FULL);
    AMETSUCHI_CRITICAL(res, EACCES);
    AMETSUCHI_CRITICAL(res, EINVAL);
  }
}


void WSV::account_remove(const iroha::AccountRemove *command) {
  MDB_val c_key, c_val;
  int res;

  auto pubkey = command->pubkey();

  c_key.mv_data = (void *)(pubkey->data());
  c_key.mv_size = pubkey->size();


  // move cursor to account in pubkey_account tree
  auto cursor = trees_.at("wsv_pubkey_account").second;
  if ((res = mdb_cursor_get(cursor, &c_key, &c_val, MDB_SET))) {
    if (res == MDB_NOTFOUND)
      throw exception::InvalidTransaction::ACCOUNT_NOT_FOUND;

    AMETSUCHI_CRITICAL(res, EINVAL);
  }

  // remove it
  if ((res = mdb_cursor_del(cursor, 0))) {
    AMETSUCHI_CRITICAL(res, EACCES);
    AMETSUCHI_CRITICAL(res, EINVAL);
  }


  // move cursor to pubkey in pubkey_assets tree
  cursor = trees_.at("wsv_pubkey_assets").second;
  if ((res = mdb_cursor_get(cursor, &c_key, &c_val, MDB_SET))) {
    AMETSUCHI_CRITICAL(res, EINVAL);
    // do not handle MDB_NOTFOUND! it means, that account has no assets
    if (res == MDB_NOTFOUND) return;
  }

  // remove account from tree with assets
  if ((res = mdb_cursor_del(cursor, MDB_NODUPDATA))) {
    AMETSUCHI_CRITICAL(res, EACCES);
    AMETSUCHI_CRITICAL(res, EINVAL);
  }
}

void WSV::peer_add(const iroha::PeerAdd *command) {
  MDB_cursor *cursor = trees_.at("wsv_ip_peer").second;
  MDB_val c_key, c_val;
  int res;

  auto peer = flatbuffers::GetRoot<iroha::Peer>(command->peer()->data());
  auto ip = peer->ip();

  flatbuffers::GetRoot<iroha::Peer>(peer);

  c_key.mv_data = (void *)(ip->data());
  c_key.mv_size = ip->size();
  c_val.mv_data = (void *)command->peer()->data();
  c_val.mv_size = command->peer()->size();

  if ((res = mdb_cursor_put(cursor, &c_key, &c_val, 0))) {
    // account with this public key exists
    if (res == MDB_KEYEXIST) {
      throw exception::InvalidTransaction::PEER_EXISTS;
    }
    AMETSUCHI_CRITICAL(res, MDB_MAP_FULL);
    AMETSUCHI_CRITICAL(res, MDB_TXN_FULL);
    AMETSUCHI_CRITICAL(res, EACCES);
    AMETSUCHI_CRITICAL(res, EINVAL);
  }
}


void WSV::peer_remove(const iroha::PeerRemove *command) {
  auto cursor = trees_.at("wsv_ip_peer").second;
  MDB_val c_key, c_val;
  int res;

  auto peer = flatbuffers::GetRoot<iroha::Peer>(command->peer()->data());
  auto ip = peer->ip();

  flatbuffers::GetRoot<iroha::Peer>(peer);

  c_key.mv_data = (void *)(ip->data());
  c_key.mv_size = ip->size();
  c_val.mv_data = (void *)command->peer()->data();
  c_val.mv_size = command->peer()->size();

  if ((res = mdb_cursor_get(cursor, &c_key, &c_val, MDB_SET))) {
    if (res == MDB_NOTFOUND) {
      throw exception::InvalidTransaction::PEER_NOT_FOUND;
    }

    AMETSUCHI_CRITICAL(res, EINVAL);
  }

  if ((res = mdb_cursor_del(cursor, 0))) {
    AMETSUCHI_CRITICAL(res, EACCES);
    AMETSUCHI_CRITICAL(res, EINVAL);
  }
}


}
