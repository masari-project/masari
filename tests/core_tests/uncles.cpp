// Copyright (c) 2018, The Masari Project
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

#include "uncles.h"

using namespace std;

using namespace epee;
using namespace cryptonote;

// TODO-TK: hacky reuse - look for something better
#define CHAIN_BASE \
  GENERATE_ACCOUNT(first_miner_account); \
  MAKE_GENESIS_BLOCK(events, blk_a, first_miner_account, 0); \
  REWIND_BLOCKS_VN(events, blk_b, blk_a, first_miner_account, 1, 9); \
  REWIND_BLOCKS_VN(events, blk_c, blk_b, first_miner_account, 2, 10); \
  REWIND_BLOCKS_VN(events, blk_d, blk_c, first_miner_account, 3, 10); \
  REWIND_BLOCKS_VN(events, blk_e, blk_d, first_miner_account, 4, 10); \
  REWIND_BLOCKS_VN(events, blk_f, blk_e, first_miner_account, 5, 10); \
  REWIND_BLOCKS_VN(events, blk_g, blk_f, first_miner_account, 6, 10);

//-----------------------------------------------------------------------------------------------------
bool gen_uncles_base::generate_with(std::vector<test_event_entry> &events, const std::function<void(std::vector<test_event_entry> &events, const cryptonote::block &top_bl, const cryptonote::block &alt_bl, const cryptonote::account_base &original_miner, test_generator &generator)> &add_blocks) const
{
  CHAIN_BASE;
  REWIND_BLOCKS_VN(events, blk_h, blk_g, first_miner_account, 7, 10);
  REWIND_BLOCKS_VN(events, blk_i, blk_h, first_miner_account, 8, 1);

  MAKE_NEXT_BLOCKV(events, blk_0a, blk_i, first_miner_account, 8);
  MAKE_NEXT_BLOCKV(events, blk_0b, blk_i, first_miner_account, 8);

  add_blocks(events, blk_0a, blk_0b, first_miner_account, generator);

  return true;
}

// valid
//--------------------------------------------------------------------------

bool gen_uncle::generate(std::vector<test_event_entry>& events) const
{
  auto modifier = [](std::vector<test_event_entry> &events, const cryptonote::block &top_bl, const cryptonote::block &alt_bl, const cryptonote::account_base &original_miner, test_generator &generator) {
    MAKE_NEXT_BLOCKV_UNCLE(events, nephew, top_bl, original_miner, 8, alt_bl);
  };
  return generate_with(events, modifier);
}

bool gen_uncle_reorg::generate(std::vector<test_event_entry>& events) const
{
  auto modifier = [](std::vector<test_event_entry> &events, const cryptonote::block &top_bl, const cryptonote::block &alt_bl, const cryptonote::account_base &original_miner, test_generator &generator) {
    MAKE_NEXT_BLOCKV_UNCLE(events, nephew, alt_bl, original_miner, 8, top_bl);
  };
  return generate_with(events, modifier);
}

bool gen_uncle_alt_nephews::generate(std::vector<test_event_entry>& events) const
{
  auto modifier = [](std::vector<test_event_entry> &events, const cryptonote::block &top_bl, const cryptonote::block &alt_bl, const cryptonote::account_base &original_miner, test_generator &generator) {
    MAKE_NEXT_BLOCKV_UNCLE(events, nephew, alt_bl, original_miner, 8, top_bl);
    // no reorg + nephew mining an uncle
    MAKE_NEXT_BLOCKV_UNCLE(events, nephew0, top_bl, original_miner, 8, alt_bl);
    MAKE_NEXT_BLOCKV_UNCLE(events, nephew1, top_bl, original_miner, 8, alt_bl);

    // no reorg between two different nephews
    MAKE_NEXT_BLOCKV(events, new_top, nephew0, original_miner, 8);
  };
  return generate_with(events, modifier);
}

bool gen_uncle_reorg_alt_nephews::generate(std::vector<test_event_entry>& events) const
{
  auto modifier = [](std::vector<test_event_entry> &events, const cryptonote::block &top_bl, const cryptonote::block &alt_bl, const cryptonote::account_base &original_miner, test_generator &generator) {
    MAKE_NEXT_BLOCKV_UNCLE(events, nephew, alt_bl, original_miner, 8, top_bl);
    // no reorg + nephew mining an uncle
    MAKE_NEXT_BLOCKV_UNCLE(events, nephew0, top_bl, original_miner, 8, alt_bl);
    MAKE_NEXT_BLOCKV_UNCLE(events, nephew1, top_bl, original_miner, 8, alt_bl);

    // reorg between two different nephews
    MAKE_NEXT_BLOCKV(events, new_top, nephew1, original_miner, 8);
  };
  return generate_with(events, modifier);
}

bool gen_uncle_alt_nephews_as_uncle::generate(std::vector<test_event_entry>& events) const
{
  auto modifier = [](std::vector<test_event_entry> &events, const cryptonote::block &top_bl, const cryptonote::block &alt_bl, const cryptonote::account_base &original_miner, test_generator &generator) {
    MAKE_NEXT_BLOCKV_UNCLE(events, nephew, alt_bl, original_miner, 8, top_bl);
    // no reorg + nephew mining an uncle
    MAKE_NEXT_BLOCKV_UNCLE(events, nephew0, top_bl, original_miner, 8, alt_bl);
    MAKE_NEXT_BLOCKV_UNCLE(events, nephew1, top_bl, original_miner, 8, alt_bl);

    // no reorg between two different nephews and mine alt as an uncle
    MAKE_NEXT_BLOCKV_UNCLE(events, new_top, nephew0, original_miner, 8, nephew1);
  };
  return generate_with(events, modifier);
}

bool gen_uncle_reorg_alt_nephews_as_uncle::generate(std::vector<test_event_entry>& events) const
{
  auto modifier = [](std::vector<test_event_entry> &events, const cryptonote::block &top_bl, const cryptonote::block &alt_bl, const cryptonote::account_base &original_miner, test_generator &generator) {
    MAKE_NEXT_BLOCKV_UNCLE(events, nephew, alt_bl, original_miner, 8, top_bl);
    // no reorg + nephew mining an uncle
    MAKE_NEXT_BLOCKV_UNCLE(events, nephew0, top_bl, original_miner, 8, alt_bl);
    MAKE_NEXT_BLOCKV_UNCLE(events, nephew1, top_bl, original_miner, 8, alt_bl);

    // reorg between two different nephews and mine the other as an uncle
    MAKE_NEXT_BLOCKV_UNCLE(events, new_top, nephew1, original_miner, 8, nephew0);
  };
  return generate_with(events, modifier);
}

// invalid
//--------------------------------------------------------------------------

bool gen_uncle_is_parent::generate(std::vector<test_event_entry>& events) const
{
  auto modifier = [](std::vector<test_event_entry> &events, const cryptonote::block &top_bl, const cryptonote::block &alt_bl, const cryptonote::account_base &original_miner, test_generator &generator) {
    DO_CALLBACK(events, "mark_invalid_block");
    MAKE_NEXT_BLOCKV_UNCLE(events, nephew, top_bl, original_miner, 8, top_bl);
  };
  return generate_with(events, modifier);
}

bool gen_uncle_wrong_height::generate(std::vector<test_event_entry>& events) const
{
  auto modifier = [](std::vector<test_event_entry> &events, const cryptonote::block &top_bl, const cryptonote::block &alt_bl, const cryptonote::account_base &original_miner, test_generator &generator) {
    MAKE_NEXT_BLOCKV(events, new_bl, top_bl, original_miner, 8);
    DO_CALLBACK(events, "mark_invalid_block");
    MAKE_NEXT_BLOCKV_UNCLE(events, nephew, new_bl, original_miner, 8, alt_bl);
  };
  return generate_with(events, modifier);
}

// TODO-TK: tx affected but bl.uncle is getting lost somewhere (potentially serialization)
bool gen_uncle_wrong_version::generate(std::vector<test_event_entry>& events) const
{
  CHAIN_BASE;
  REWIND_BLOCKS_VN(events, blk_h, blk_g, first_miner_account, 7, 1);
  MAKE_NEXT_BLOCKV(events, blk_0a, blk_h, first_miner_account, 7);
  MAKE_NEXT_BLOCKV(events, blk_0b, blk_h, first_miner_account, 7);
  DO_CALLBACK(events, "mark_invalid_block");
  MAKE_NEXT_BLOCKV_UNCLE(events, blk_1, blk_0a, first_miner_account, 7, blk_0b);
  return true;
}

bool gen_uncle_bad_ancestry::generate(std::vector<test_event_entry>& events) const
{
  auto modifier = [](std::vector<test_event_entry> &events, const cryptonote::block &top_bl, const cryptonote::block &alt_bl, const cryptonote::account_base &original_miner, test_generator &generator) {
    MAKE_NEXT_BLOCKV(events, bl_i0, top_bl, original_miner, 8);
    MAKE_NEXT_BLOCKV(events, bl_j0, alt_bl, original_miner, 8);
    DO_CALLBACK(events, "mark_invalid_block");
    MAKE_NEXT_BLOCKV_UNCLE(events, bl_i1, bl_i0, original_miner, 8, bl_j0);
  };
  return generate_with(events, modifier);
}
