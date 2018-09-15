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


//-----------------------------------------------------------------------------------------------------
bool gen_uncles::generate(std::vector<test_event_entry> &events) const
{
  GENERATE_ACCOUNT(first_miner_account);
  MAKE_GENESIS_BLOCK(events, blk_0, first_miner_account, 0);
  MAKE_NEXT_BLOCK(events, blk_1, blk_0, first_miner_account);
  MAKE_NEXT_BLOCKV(events, blk_2, blk_1, first_miner_account, 7);
  MAKE_NEXT_BLOCKV(events, blk_3, blk_2, first_miner_account, 8);
  MAKE_NEXT_BLOCKV(events, blk_4, blk_3, first_miner_account, 8);
  MAKE_NEXT_BLOCKV(events, blk_5, blk_4, first_miner_account, 8);
  MAKE_NEXT_BLOCKV(events, blk_6a, blk_5, first_miner_account, 8);
  MAKE_NEXT_BLOCKV(events, blk_6b, blk_5, first_miner_account, 8);

  // uncle referenced
  // work-in-progress
  cryptonote::block blk_7;
  blk_7.uncle = get_block_hash(blk_6b);
  PUSH_NEXT_BLOCKV(events, blk_7, blk_6a, first_miner_account, 8);

  MAKE_NEXT_BLOCKV(events, blk_8, blk_7, first_miner_account, 8);
  return true;
}
