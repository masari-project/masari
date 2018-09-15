// Copyright (c) 2014-2018, The Monero Project
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

#pragma once
#include "chaingen.h"

struct gen_uncles_base : public test_chain_unit_base
{
  gen_uncles_base() {}
};

template<>
struct get_test_options<gen_uncles_base> {
  const std::pair<uint8_t, uint64_t> hard_forks[9] = {std::make_pair(1, 0), std::make_pair(2, 10), std::make_pair(3, 20), std::make_pair(4, 30),std::make_pair(5, 40), std::make_pair(6, 50), std::make_pair(7, 60), std::make_pair(8, 70), std::make_pair(0, 0)};
  const cryptonote::test_options test_options = {
    hard_forks
  };
};

// valid
//-----------------------------------------------------------------------------------------------------
struct gen_uncles : public gen_uncles_base
{
  bool generate(std::vector<test_event_entry>& events) const;
};
template<> struct get_test_options<gen_uncles>: public get_test_options<gen_uncles_base> {};

// invalid
//-----------------------------------------------------------------------------------------------------
struct gen_uncle_is_parent : public gen_uncles_base
{
  bool generate(std::vector<test_event_entry>& events) const;
};
template<> struct get_test_options<gen_uncle_is_parent>: public get_test_options<gen_uncles_base> {};

struct gen_uncle_hash_too_low : public gen_uncles_base
{
  bool generate(std::vector<test_event_entry>& events) const;
};
template<> struct get_test_options<gen_uncle_hash_too_low>: public get_test_options<gen_uncles_base> {};

struct spend_uncle_original_miner_tx : public gen_uncles_base
{
  bool generate(std::vector<test_event_entry>& events) const;
};
template<> struct get_test_options<spend_uncle_original_miner_tx>: public get_test_options<gen_uncles_base> {};

struct spend_uncle_mined_txs : public gen_uncles_base
{
  bool generate(std::vector<test_event_entry>& events) const;
};
template<> struct get_test_options<spend_uncle_mined_txs>: public get_test_options<gen_uncles_base> {};
