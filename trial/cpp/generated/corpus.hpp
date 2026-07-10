// GENERATED compile-time corpus oracle by taut/src/taut/gen/cpp.py — do not edit.
// Each static_assert constructs the native value and proves its encoding at COMPILE TIME.
#pragma once
#include "types.hpp"

namespace taut::corpus {

// Board/materialized (Board)
consteval taut::Buf encode_Board_materialized() {
  auto v = taut::Board{std::string_view("world", 5), 8};
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq_hex(encode_Board_materialized(), "a20165776f726c640208"), "Board/materialized encode");
consteval taut::Buf roundtrip_Board_materialized() {
  std::string_view src = std::string_view("\xa2""\x01""eworld\x02""\x08""", 10);
  auto v = taut::Board::from_cbor(taut::parse(src));
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq(roundtrip_Board_materialized(), std::string_view("\xa2""\x01""eworld\x02""\x08""", 10)), "Board/materialized roundtrip");

// ByteOp/replace (ByteOp)
consteval taut::Buf encode_ByteOp_replace() {
  auto v = taut::ByteOp{taut::OpKind::Replace, 3, 2, std::string_view("\x00""xy", 3)};
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq_hex(encode_ByteOp_replace(), "a40102020303020443007879"), "ByteOp/replace encode");
consteval taut::Buf roundtrip_ByteOp_replace() {
  std::string_view src = std::string_view("\xa4""\x01""\x02""\x02""\x03""\x03""\x02""\x04""C\x00""xy", 12);
  auto v = taut::ByteOp::from_cbor(taut::parse(src));
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq(roundtrip_ByteOp_replace(), std::string_view("\xa4""\x01""\x02""\x02""\x03""\x03""\x02""\x04""C\x00""xy", 12)), "ByteOp/replace roundtrip");

// ChatMessage/first (ChatMessage)
consteval taut::Buf encode_ChatMessage_first() {
  auto v = taut::ChatMessage{0, std::string_view("ann", 3), std::string_view("hi", 2)};
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq_hex(encode_ChatMessage_first(), "a301000263616e6e03626869"), "ChatMessage/first encode");
consteval taut::Buf roundtrip_ChatMessage_first() {
  std::string_view src = std::string_view("\xa3""\x01""\x00""\x02""cann\x03""bhi", 12);
  auto v = taut::ChatMessage::from_cbor(taut::parse(src));
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq(roundtrip_ChatMessage_first(), std::string_view("\xa3""\x01""\x00""\x02""cann\x03""bhi", 12)), "ChatMessage/first roundtrip");

// CmdSession/mixed (CmdSession)
consteval taut::Buf encode_CmdSession_mixed() {
  auto v = taut::CmdSession{std::string_view("sess-0001", 9), {std::string_view("git", 3), std::string_view("status", 6)}, {taut::RepoRun{std::string_view("alpha", 5), 0, std::string_view("git status @ alpha", 18), std::nullopt}, taut::RepoRun{std::string_view("beta", 4), 2, std::string_view("", 0), std::string_view("beta: exited 2", 14)}}, {}};
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq_hex(encode_CmdSession_mixed(), "a30169736573732d30303031028263676974667374617475730382a40165616c7068610200037267697420737461747573204020616c70686104f6a401646265746102020360046e626574613a206578697465642032"), "CmdSession/mixed encode");
consteval taut::Buf roundtrip_CmdSession_mixed() {
  std::string_view src = std::string_view("\xa3""\x01""isess-0001\x02""\x82""cgitfstatus\x03""\x82""\xa4""\x01""ealpha\x02""\x00""\x03""rgit status @ alpha\x04""\xf6""\xa4""\x01""dbeta\x02""\x02""\x03""`\x04""nbeta: exited 2", 86);
  auto v = taut::CmdSession::from_cbor(taut::parse(src));
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq(roundtrip_CmdSession_mixed(), std::string_view("\xa3""\x01""isess-0001\x02""\x82""cgitfstatus\x03""\x82""\xa4""\x01""ealpha\x02""\x00""\x03""rgit status @ alpha\x04""\xf6""\xa4""\x01""dbeta\x02""\x02""\x03""`\x04""nbeta: exited 2", 86)), "CmdSession/mixed roundtrip");

// PeerPresence/online (PeerPresence)
consteval taut::Buf encode_PeerPresence_online() {
  auto v = taut::PeerPresence{std::string_view("p1", 2), std::string_view("Ann", 3), taut::PresenceStatus::Online, true, std::string_view("NYC", 3)};
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq_hex(encode_PeerPresence_online(), "a5016270310263416e6e030304f505634e5943"), "PeerPresence/online encode");
consteval taut::Buf roundtrip_PeerPresence_online() {
  std::string_view src = std::string_view("\xa5""\x01""bp1\x02""cAnn\x03""\x03""\x04""\xf5""\x05""cNYC", 19);
  auto v = taut::PeerPresence::from_cbor(taut::parse(src));
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq(roundtrip_PeerPresence_online(), std::string_view("\xa5""\x01""bp1\x02""cAnn\x03""\x03""\x04""\xf5""\x05""cNYC", 19)), "PeerPresence/online roundtrip");

// RepoRun/err (RepoRun)
consteval taut::Buf encode_RepoRun_err() {
  auto v = taut::RepoRun{std::string_view("beta", 4), 2, std::string_view("", 0), std::string_view("beta: exited 2", 14)};
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq_hex(encode_RepoRun_err(), "a401646265746102020360046e626574613a206578697465642032"), "RepoRun/err encode");
consteval taut::Buf roundtrip_RepoRun_err() {
  std::string_view src = std::string_view("\xa4""\x01""dbeta\x02""\x02""\x03""`\x04""nbeta: exited 2", 27);
  auto v = taut::RepoRun::from_cbor(taut::parse(src));
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq(roundtrip_RepoRun_err(), std::string_view("\xa4""\x01""dbeta\x02""\x02""\x03""`\x04""nbeta: exited 2", 27)), "RepoRun/err roundtrip");

// RepoRun/ok (RepoRun)
consteval taut::Buf encode_RepoRun_ok() {
  auto v = taut::RepoRun{std::string_view("alpha", 5), 0, std::string_view("git status @ alpha", 18), std::nullopt};
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq_hex(encode_RepoRun_ok(), "a40165616c7068610200037267697420737461747573204020616c70686104f6"), "RepoRun/ok encode");
consteval taut::Buf roundtrip_RepoRun_ok() {
  std::string_view src = std::string_view("\xa4""\x01""ealpha\x02""\x00""\x03""rgit status @ alpha\x04""\xf6""", 32);
  auto v = taut::RepoRun::from_cbor(taut::parse(src));
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq(roundtrip_RepoRun_ok(), std::string_view("\xa4""\x01""ealpha\x02""\x00""\x03""rgit status @ alpha\x04""\xf6""", 32)), "RepoRun/ok roundtrip");

// TerminalChunk/ls (TerminalChunk)
consteval taut::Buf encode_TerminalChunk_ls() {
  auto v = taut::TerminalChunk{std::string_view("term-1", 6), std::string_view("$ ls\x0a""", 5)};
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq_hex(encode_TerminalChunk_ls(), "a201667465726d2d31024524206c730a"), "TerminalChunk/ls encode");
consteval taut::Buf roundtrip_TerminalChunk_ls() {
  std::string_view src = std::string_view("\xa2""\x01""fterm-1\x02""E$ ls\x0a""", 16);
  auto v = taut::TerminalChunk::from_cbor(taut::parse(src));
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq(roundtrip_TerminalChunk_ls(), std::string_view("\xa2""\x01""fterm-1\x02""E$ ls\x0a""", 16)), "TerminalChunk/ls roundtrip");

// TerminalOpened/alpha (TerminalOpened)
consteval taut::Buf encode_TerminalOpened_alpha() {
  auto v = taut::TerminalOpened{std::string_view("term-1", 6), std::string_view("alpha", 5), 120, 40};
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq_hex(encode_TerminalOpened_alpha(), "a401667465726d2d310265616c706861031878041828"), "TerminalOpened/alpha encode");
consteval taut::Buf roundtrip_TerminalOpened_alpha() {
  std::string_view src = std::string_view("\xa4""\x01""fterm-1\x02""ealpha\x03""\x18""x\x04""\x18""(", 22);
  auto v = taut::TerminalOpened::from_cbor(taut::parse(src));
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq(roundtrip_TerminalOpened_alpha(), std::string_view("\xa4""\x01""fterm-1\x02""ealpha\x03""\x18""x\x04""\x18""(", 22)), "TerminalOpened/alpha roundtrip");

// crdt/op (CrdtOp)
consteval taut::Buf encode_crdt_op() {
  auto v = taut::CrdtOp{std::string_view("board:1", 7), std::string_view("A", 1), 1, 2, std::string_view("\x03""", 1)};
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq_hex(encode_crdt_op(), "a50167626f6172643a3102614103010402054103"), "crdt/op encode");
consteval taut::Buf roundtrip_crdt_op() {
  std::string_view src = std::string_view("\xa5""\x01""gboard:1\x02""aA\x03""\x01""\x04""\x02""\x05""A\x03""", 20);
  auto v = taut::CrdtOp::from_cbor(taut::parse(src));
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq(roundtrip_crdt_op(), std::string_view("\xa5""\x01""gboard:1\x02""aA\x03""\x01""\x04""\x02""\x05""A\x03""", 20)), "crdt/op roundtrip");

// crdt/state (CrdtState)
consteval taut::Buf encode_crdt_state() {
  auto v = taut::CrdtState{std::string_view("board:1", 7), {taut::CrdtOp{std::string_view("board:1", 7), std::string_view("A", 1), 1, 2, std::string_view("\x03""", 1)}, taut::CrdtOp{std::string_view("board:1", 7), std::string_view("B", 1), 1, 1, std::string_view("world", 5)}}, {taut::VersionEntry{std::string_view("A", 1), 1}, taut::VersionEntry{std::string_view("B", 1), 1}}};
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq_hex(encode_crdt_state(), "a30167626f6172643a310282a50167626f6172643a3102614103010402054103a50167626f6172643a31026142030104010545776f726c640382a20161410201a20161420201"), "crdt/state encode");
consteval taut::Buf roundtrip_crdt_state() {
  std::string_view src = std::string_view("\xa3""\x01""gboard:1\x02""\x82""\xa5""\x01""gboard:1\x02""aA\x03""\x01""\x04""\x02""\x05""A\x03""\xa5""\x01""gboard:1\x02""aB\x03""\x01""\x04""\x01""\x05""Eworld\x03""\x82""\xa2""\x01""aA\x02""\x01""\xa2""\x01""aB\x02""\x01""", 70);
  auto v = taut::CrdtState::from_cbor(taut::parse(src));
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq(roundtrip_crdt_state(), std::string_view("\xa3""\x01""gboard:1\x02""\x82""\xa5""\x01""gboard:1\x02""aA\x03""\x01""\x04""\x02""\x05""A\x03""\xa5""\x01""gboard:1\x02""aB\x03""\x01""\x04""\x01""\x05""Eworld\x03""\x82""\xa2""\x01""aA\x02""\x01""\xa2""\x01""aB\x02""\x01""", 70)), "crdt/state roundtrip");

// swmr/delta-1 (FileDelta)
consteval taut::Buf encode_swmr_delta_1() {
  auto v = taut::FileDelta{std::string_view("file:notes.txt", 14), 1, 2, {taut::ByteOp{taut::OpKind::Insert, 11, 0, std::string_view("!", 1)}}, 12};
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq_hex(encode_swmr_delta_1(), "a5016e66696c653a6e6f7465732e747874020103020481a40100020b0300044121050c"), "swmr/delta-1 encode");
consteval taut::Buf roundtrip_swmr_delta_1() {
  std::string_view src = std::string_view("\xa5""\x01""nfile:notes.txt\x02""\x01""\x03""\x02""\x04""\x81""\xa4""\x01""\x00""\x02""\x0b""\x03""\x00""\x04""A!\x05""\x0c""", 35);
  auto v = taut::FileDelta::from_cbor(taut::parse(src));
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq(roundtrip_swmr_delta_1(), std::string_view("\xa5""\x01""nfile:notes.txt\x02""\x01""\x03""\x02""\x04""\x81""\xa4""\x01""\x00""\x02""\x0b""\x03""\x00""\x04""A!\x05""\x0c""", 35)), "swmr/delta-1 roundtrip");

// swmr/delta-2 (FileDelta)
consteval taut::Buf encode_swmr_delta_2() {
  auto v = taut::FileDelta{std::string_view("file:notes.txt", 14), 2, 3, {taut::ByteOp{taut::OpKind::Delete, 0, 5, std::string_view("", 0)}}, 7};
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq_hex(encode_swmr_delta_2(), "a5016e66696c653a6e6f7465732e747874020203030481a401010200030504400507"), "swmr/delta-2 encode");
consteval taut::Buf roundtrip_swmr_delta_2() {
  std::string_view src = std::string_view("\xa5""\x01""nfile:notes.txt\x02""\x02""\x03""\x03""\x04""\x81""\xa4""\x01""\x01""\x02""\x00""\x03""\x05""\x04""@\x05""\x07""", 34);
  auto v = taut::FileDelta::from_cbor(taut::parse(src));
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq(roundtrip_swmr_delta_2(), std::string_view("\xa5""\x01""nfile:notes.txt\x02""\x02""\x03""\x03""\x04""\x81""\xa4""\x01""\x01""\x02""\x00""\x03""\x05""\x04""@\x05""\x07""", 34)), "swmr/delta-2 roundtrip");

// swmr/snapshot (FileSnapshot)
consteval taut::Buf encode_swmr_snapshot() {
  auto v = taut::FileSnapshot{std::string_view("file:notes.txt", 14), 1, std::string_view("hello world", 11), 0, -1, {}};
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq_hex(encode_swmr_snapshot(), "a5016e66696c653a6e6f7465732e7478740201034b68656c6c6f20776f726c6404000520"), "swmr/snapshot encode");
consteval taut::Buf roundtrip_swmr_snapshot() {
  std::string_view src = std::string_view("\xa5""\x01""nfile:notes.txt\x02""\x01""\x03""Khello world\x04""\x00""\x05"" ", 36);
  auto v = taut::FileSnapshot::from_cbor(taut::parse(src));
  taut::Buf b; v.to_cbor(b); return b;
}
static_assert(taut::eq(roundtrip_swmr_snapshot(), std::string_view("\xa5""\x01""nfile:notes.txt\x02""\x01""\x03""Khello world\x04""\x00""\x05"" ", 36)), "swmr/snapshot roundtrip");

inline constexpr int VECTOR_COUNT = 14;

} // namespace taut::corpus
