// Copyright (c) 2014-2017, The Masari Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

#include "ringct/rctSigs.h"
#include "chaingen.h"
#include "chaingen_tests_list.h"

using namespace epee;
using namespace crypto;
using namespace cryptonote;

//----------------------------------------------------------------------------------------------------------------------
// Tests

bool gen_rct_tx_validation_base::generate_with(std::vector<test_event_entry>& events,
    const int *out_idx, int mixin, uint64_t amount_paid, bool valid,
    const std::function<void(std::vector<tx_source_entry> &sources, std::vector<tx_destination_entry> &destinations)> &pre_tx,
    const std::function<void(transaction &tx)> &post_tx) const
{
  GENERATE_ACCOUNT(miner_account);
  MAKE_GENESIS_BLOCK(events, blk_0, miner_account, 0);

  // create 13 miner accounts, and have them mine the next 13 blocks
  cryptonote::account_base miner_accounts[13];
  const cryptonote::block *prev_block = &blk_0;
  cryptonote::block blocks[13];
  for (size_t i = 0; i < 13; i++) {
    miner_accounts[i].generate();
    CHECK_AND_ASSERT_MES(generator.construct_block_manually(blocks[i], *prev_block, miner_accounts[i]), false, "Failed to generate block");
    events.push_back(blocks[i]);
    prev_block = blocks + i;
    LOG_PRINT_L0("Initial miner tx " << i << ": " << obj_to_json_str(blocks[i].miner_tx));
  }

  REWIND_BLOCKS(events, blk_last, blocks[12], miner_account);

  // create 4 txes from these miners in another block, to generate some rct outputs
  transaction rct_txes[4];
  rct::key rct_tx_masks[16];
  cryptonote::block blk_txes[4];
  for (size_t n = 0; n < 4; ++n)
  {
    std::vector<crypto::hash> starting_rct_tx_hashes;
    std::vector<tx_source_entry> sources;

    sources.resize(1);
    tx_source_entry& src = sources.back();

    src.amount = blocks[n].miner_tx.vout[0].amount;
    src.real_out_tx_key = cryptonote::get_tx_pub_key_from_extra(blocks[n].miner_tx);
    src.real_output = n;
    src.real_output_in_tx_index = 0;
    src.mask = rct::identity();

    for (int m = 0; m < 13; ++m) {
      src.push_output(m + 1, boost::get<txout_to_key>(blocks[m].miner_tx.vout[0].target).key, blocks[m].miner_tx.vout[0].amount);
    }

    //fill outputs entry
    tx_destination_entry td;
    td.addr = miner_accounts[n].get_keys().m_account_address;
    td.amount = src.amount / 4 - src.amount / 8; // arbitrary split over four destinations
    std::vector<tx_destination_entry> destinations;
    destinations.push_back(td);
    destinations.push_back(td);
    destinations.push_back(td);
    destinations.push_back(td);

    crypto::secret_key tx_key;
    bool r = construct_tx_and_get_tx_key(miner_accounts[n].get_keys(), sources, destinations, std::vector<uint8_t>(), rct_txes[n], 0, tx_key);
    CHECK_AND_ASSERT_MES(r, false, "failed to construct transaction");
    events.push_back(rct_txes[n]);
    starting_rct_tx_hashes.push_back(get_transaction_hash(rct_txes[n]));

    for (size_t o = 0; o < 4; ++o)
    {
      crypto::key_derivation derivation;
      bool r = crypto::generate_key_derivation(destinations[o].addr.m_view_public_key, tx_key, derivation);
      CHECK_AND_ASSERT_MES(r, false, "Failed to generate key derivation");
      crypto::secret_key amount_key;
      crypto::derivation_to_scalar(derivation, o, amount_key);
      if (rct_txes[n].rct_signatures.type == rct::RCTTypeSimple)
        rct::decodeRctSimple(rct_txes[n].rct_signatures, rct::sk2rct(amount_key), o, rct_tx_masks[o + n * 4]);
      else
        rct::decodeRct(rct_txes[n].rct_signatures, rct::sk2rct(amount_key), o, rct_tx_masks[o + n * 4]);
    }

    CHECK_AND_ASSERT_MES(generator.construct_block_manually_tx(blk_txes[n], blk_last, miner_account, starting_rct_tx_hashes, (size_t) 1, rct_txes[n].rct_signatures.txnFee), false, "Failed to generate block");
    events.push_back(blk_txes[n]);
    blk_last = blk_txes[n];
  }

  REWIND_BLOCKS(events, pre_tx_blk, blk_last, miner_account);

  //MAKE_TX_MIX(events, tx_0, miner_accounts[0], miner_account, blocks[0].miner_tx.vout[0].amount / 4 - blocks[0].miner_tx.vout[0].amount / 8, 12, blk_last);
  cryptonote::transaction tx;                                                             \
  vector<tx_source_entry> sources;
  vector<tx_destination_entry> destinations;
  fill_tx_sources_and_destinations(events, blk_last, miner_accounts[0], miner_account, blocks[0].miner_tx.vout[0].amount / 4 - blocks[0].miner_tx.vout[0].amount / 8, MK_COINS(1), 12, sources, destinations);

  if (pre_tx) { pre_tx(sources, destinations); }
  bool r = construct_tx(miner_accounts[0].get_keys(), sources, destinations, std::vector<uint8_t>(), tx, 0);
  CHECK_AND_ASSERT_MES(r, false, "failed to construct transaction");
  if (post_tx) { post_tx(tx); }

  if (!valid) { DO_CALLBACK(events, "mark_invalid_tx"); }

  events.push_back(tx);
  return true;
}

bool gen_rct_tx_valid_from_pre_rct::generate(std::vector<test_event_entry>& events) const
{
  const int mixin = 2;
  const int out_idx[] = {0, -1};
  const uint64_t amount_paid = 10000;
  return generate_with(events, out_idx, mixin, amount_paid, true, NULL, NULL);
}

bool gen_rct_tx_valid_from_rct::generate(std::vector<test_event_entry>& events) const
{
  const int mixin = 2;
  const int out_idx[] = {1, -1};
  const uint64_t amount_paid = 10000;
  return generate_with(events, out_idx, mixin, amount_paid, true, NULL, NULL);
}

bool gen_rct_tx_rct_bad_real_dest::generate(std::vector<test_event_entry>& events) const
{
  const int mixin = 2;
  const int out_idx[] = {1, -1};
  const uint64_t amount_paid = 10000;
  // in the case, the tx will fail to create, due to mismatched sk/pk
  return generate_with(events, out_idx, mixin, amount_paid, false,
    [](std::vector<tx_source_entry> &sources, std::vector<tx_destination_entry> &destinations) {
    rct::key sk;
    rct::skpkGen(sk, sources[1].outputs[11].second.dest);
    },
    NULL);
}

bool gen_rct_tx_rct_bad_real_mask::generate(std::vector<test_event_entry>& events) const
{
  const int mixin = 2;
  const int out_idx[] = {1, -1};
  const uint64_t amount_paid = 10000;
  return generate_with(events, out_idx, mixin, amount_paid, false,
    [](std::vector<tx_source_entry> &sources, std::vector<tx_destination_entry> &destinations) {sources[0].outputs[0].second.mask = rct::zeroCommit(99999);},
    NULL);
}

bool gen_rct_tx_rct_bad_fake_dest::generate(std::vector<test_event_entry>& events) const
{
  const int mixin = 2;
  const int out_idx[] = {1, -1};
  const uint64_t amount_paid = 10000;
  return generate_with(events, out_idx, mixin, amount_paid, false,
    [](std::vector<tx_source_entry> &sources, std::vector<tx_destination_entry> &destinations) {rct::key sk; rct::skpkGen(sk, sources[0].outputs[1].second.dest);},
    NULL);
}

bool gen_rct_tx_rct_bad_fake_mask::generate(std::vector<test_event_entry>& events) const
{
  const int mixin = 2;
  const int out_idx[] = {1, -1};
  const uint64_t amount_paid = 10000;
  return generate_with(events, out_idx, mixin, amount_paid, false,
    [](std::vector<tx_source_entry> &sources, std::vector<tx_destination_entry> &destinations) {sources[0].outputs[1].second.mask = rct::zeroCommit(99999);},
    NULL);
}

bool gen_rct_tx_rct_spend_with_zero_commit::generate(std::vector<test_event_entry>& events) const
{
  const int mixin = 2;
  const int out_idx[] = {1, -1};
  const uint64_t amount_paid = 10000;
  return generate_with(events, out_idx, mixin, amount_paid, false,
    [](std::vector<tx_source_entry> &sources, std::vector<tx_destination_entry> &destinations) {sources[0].outputs[0].second.mask = rct::zeroCommit(sources[0].amount); sources[0].mask = rct::identity();},
    [](transaction &tx){boost::get<txin_to_key>(tx.vin[0]).amount = 0;});
}

bool gen_rct_tx_rct_non_zero_vin_amount::generate(std::vector<test_event_entry>& events) const
{
  const int mixin = 2;
  const int out_idx[] = {1, -1};
  const uint64_t amount_paid = 10000;
  return generate_with(events, out_idx, mixin, amount_paid, false,
    NULL, [](transaction &tx) {boost::get<txin_to_key>(tx.vin[0]).amount = 5000000000000;}); // one that we know exists
}

bool gen_rct_tx_non_zero_vout_amount::generate(std::vector<test_event_entry>& events) const
{
  const int mixin = 2;
  const int out_idx[] = {1, -1};
  const uint64_t amount_paid = 10000;
  return generate_with(events, out_idx, mixin, amount_paid, false,
    NULL, [](transaction &tx) {tx.vout[0].amount = 5000000000000;}); // one that we know exists
}

bool gen_rct_tx_rct_duplicate_key_image::generate(std::vector<test_event_entry>& events) const
{
  const int mixin = 2;
  const int out_idx[] = {1, -1};
  const uint64_t amount_paid = 10000;
  return generate_with(events, out_idx, mixin, amount_paid, false,
    NULL, [&events](transaction &tx) {boost::get<txin_to_key>(tx.vin[0]).k_image = boost::get<txin_to_key>(boost::get<transaction>(events[76]).vin[0]).k_image;});
}

bool gen_rct_tx_rct_wrong_key_image::generate(std::vector<test_event_entry>& events) const
{
  const int mixin = 2;
  const int out_idx[] = {1, -1};
  const uint64_t amount_paid = 10000;
  // some random key image from the masari blockchain, so we get something that is a valid key image
  static const uint8_t k_image[33] = "\x49\x3b\x56\x16\x54\x76\xa8\x75\xb7\xf4\xa8\x51\xf5\x55\xd3\x44\xe7\x3e\xea\x73\xee\xc1\x06\x7c\x7d\xb6\x57\x28\x46\x85\xe1\x07";
  return generate_with(events, out_idx, mixin, amount_paid, false,
    NULL, [](transaction &tx) {memcpy(&boost::get<txin_to_key>(tx.vin[0]).k_image, k_image, 32);});
}

bool gen_rct_tx_rct_wrong_fee::generate(std::vector<test_event_entry>& events) const
{
  const int mixin = 2;
  const int out_idx[] = {1, -1};
  const uint64_t amount_paid = 10000;
  return generate_with(events, out_idx, mixin, amount_paid, false,
    NULL, [](transaction &tx) {tx.rct_signatures.txnFee++;});
}

bool gen_rct_tx_rct_remove_vin::generate(std::vector<test_event_entry>& events) const
{
  const int mixin = 2;
  const int out_idx[] = {1, -1};
  const uint64_t amount_paid = 10000;
  return generate_with(events, out_idx, mixin, amount_paid, false,
    NULL, [](transaction &tx) {tx.vin.pop_back();});
}

bool gen_rct_tx_rct_add_vout::generate(std::vector<test_event_entry>& events) const
{
  const int mixin = 2;
  const int out_idx[] = {1, -1};
  const uint64_t amount_paid = 10000;
  return generate_with(events, out_idx, mixin, amount_paid, false,
    NULL, [](transaction &tx) {tx.vout.push_back(tx.vout.back());});
}

bool gen_rct_tx_rct_altered_extra::generate(std::vector<test_event_entry>& events) const
{
  const int mixin = 2;
  const int out_idx[] = {1, -1};
  const uint64_t amount_paid = 10000;
  return generate_with(events, out_idx, mixin, amount_paid, false,
    NULL, [](transaction &tx) {std::string extra_nonce; crypto::hash pid = cryptonote::null_hash; set_payment_id_to_tx_extra_nonce(extra_nonce, pid); add_extra_nonce_to_tx_extra(tx.extra, extra_nonce);});
}

