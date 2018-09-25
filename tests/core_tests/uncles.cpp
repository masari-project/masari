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
  // TODO-TK: test difficulties hitting 0 on integer divisions, need to address impact in rounding up to 1
  REWIND_BLOCKS_VN(events, blk_a, blk_0, first_miner_account, 1, 9);
  REWIND_BLOCKS_VN(events, blk_b, blk_a, first_miner_account, 2, 10);
  REWIND_BLOCKS_VN(events, blk_c, blk_b, first_miner_account, 3, 10);
  REWIND_BLOCKS_VN(events, blk_d, blk_c, first_miner_account, 4, 10);
  REWIND_BLOCKS_VN(events, blk_e, blk_d, first_miner_account, 5, 10);
  REWIND_BLOCKS_VN(events, blk_f, blk_e, first_miner_account, 6, 10);
  REWIND_BLOCKS_VN(events, blk_i, blk_f, first_miner_account, 7, 10);
  REWIND_BLOCKS_VN(events, blk_j, blk_i, first_miner_account, 8, 10);                  // 79

  MAKE_NEXT_BLOCKV(events, blk_1a, blk_j, first_miner_account, 8);                     // 80
  MAKE_NEXT_BLOCKV(events, blk_1b, blk_j, first_miner_account, 8);                     // 80

  // no reorg + nephew mining an uncle
  MAKE_NEXT_BLOCKV_UNCLE(events, blk_2, blk_1a, first_miner_account, 8, blk_1b);       // 81

  MAKE_NEXT_BLOCKV(events, blk_3a, blk_2, first_miner_account, 8);                     // 82
  MAKE_NEXT_BLOCKV(events, blk_3b, blk_2, first_miner_account, 8);                     // 82

  // reorg + nephew mining an uncle
  MAKE_NEXT_BLOCKV_UNCLE(events, blk_4, blk_3b, first_miner_account, 8, blk_3a);       // 83

  MAKE_NEXT_BLOCKV(events, blk_5a, blk_4, first_miner_account, 8);                     // 84
  MAKE_NEXT_BLOCKV(events, blk_5b, blk_4, first_miner_account, 8);                     // 84

  // no reorg + nephew mining an uncle
  MAKE_NEXT_BLOCKV_UNCLE(events, blk_6a, blk_5a, first_miner_account, 8, blk_5b);      // 85
  MAKE_NEXT_BLOCKV_UNCLE(events, blk_6b, blk_5a, first_miner_account, 8, blk_5b);      // 85

  // reorg between two different nephews
  MAKE_NEXT_BLOCKV(events, blk_7, blk_6b, first_miner_account, 8);                     // 86

  // reorg between two different nephews + nephew mined as an uncle
  MAKE_NEXT_BLOCKV(events, blk_8a, blk_7, first_miner_account, 8);                     // 87
  MAKE_NEXT_BLOCKV(events, blk_8b, blk_7, first_miner_account, 8);                     // 87

  MAKE_NEXT_BLOCKV_UNCLE(events, blk_9a, blk_8a, first_miner_account, 8, blk_8b);      // 88
  MAKE_NEXT_BLOCKV_UNCLE(events, blk_9b, blk_8b, first_miner_account, 8, blk_8a);      // 88

  MAKE_NEXT_BLOCKV_UNCLE(events, blk_10, blk_9b, first_miner_account, 8, blk_9a);      // 89

  // no reorg between two different nephews + nephew mined as an uncle
  MAKE_NEXT_BLOCKV(events, blk_11a, blk_10, first_miner_account, 8);                   // 90
  MAKE_NEXT_BLOCKV(events, blk_11b, blk_10, first_miner_account, 8);                   // 90

  MAKE_NEXT_BLOCKV_UNCLE(events, blk_12a, blk_11a, first_miner_account, 8, blk_11b);   // 91
  MAKE_NEXT_BLOCKV_UNCLE(events, blk_12b, blk_11b, first_miner_account, 8, blk_11a);   // 91

  MAKE_NEXT_BLOCKV_UNCLE(events, blk_13, blk_12a, first_miner_account, 8, blk_12b);    // 92

  // TODO-TK: add tests when ready
  return true;
}
