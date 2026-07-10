// GENERATED native C++ types by taut/src/taut/gen/cpp.py — do not edit.
#pragma once
#include <optional>
#include <string_view>
#include <utility>
#include <vector>
#include "taut/cbor.hpp"

namespace taut {

enum class PresenceStatus : long long {
  Configured = 0,
  Offline = 1,
  Starting = 2,
  Online = 3,
  Error = 4,
};
inline constexpr long long wire(PresenceStatus v) { return static_cast<long long>(v); }
inline constexpr DecodeResult<PresenceStatus> try_PresenceStatus_from_wire(long long v) {
  switch (v) {
    case 0: return DecodeResult<PresenceStatus>::success(PresenceStatus::Configured);
    case 1: return DecodeResult<PresenceStatus>::success(PresenceStatus::Offline);
    case 2: return DecodeResult<PresenceStatus>::success(PresenceStatus::Starting);
    case 3: return DecodeResult<PresenceStatus>::success(PresenceStatus::Online);
    case 4: return DecodeResult<PresenceStatus>::success(PresenceStatus::Error);
    default: return DecodeResult<PresenceStatus>::fail(DecodeError::unknown_enum("PresenceStatus", v));
  }
}

enum class OpKind : long long {
  Insert = 0,
  Delete = 1,
  Replace = 2,
};
inline constexpr long long wire(OpKind v) { return static_cast<long long>(v); }
inline constexpr DecodeResult<OpKind> try_OpKind_from_wire(long long v) {
  switch (v) {
    case 0: return DecodeResult<OpKind>::success(OpKind::Insert);
    case 1: return DecodeResult<OpKind>::success(OpKind::Delete);
    case 2: return DecodeResult<OpKind>::success(OpKind::Replace);
    default: return DecodeResult<OpKind>::fail(DecodeError::unknown_enum("OpKind", v));
  }
}

struct PeerPresence {
  std::string_view id;
  std::string_view name;
  PresenceStatus status;
  bool online;
  std::string_view location;
  constexpr void to_cbor(Buf& b) const {
    b.map(5);
    b.uint(1);
    b.text(id);
    b.uint(2);
    b.text(name);
    b.uint(3);
    b.integer(static_cast<long long>(status));
    b.uint(4);
    b.boolean(online);
    b.uint(5);
    b.text(location);
  }
  static constexpr PeerPresence from_cbor(const Cbor& c) {
    PeerPresence v{};
    v.id = c.get(1).as_text();
    v.name = c.get(2).as_text();
    v.status = static_cast<PresenceStatus>(c.get(3).as_int());
    v.online = c.get(4).as_bool();
    v.location = c.get(5).as_text();
    return v;
  }
  static DecodeResult<PeerPresence> try_from_cbor(const Cbor& c) {
    PeerPresence v{};
    auto __field_1 = c.try_get(1);
    if (!__field_1) return DecodeResult<PeerPresence>::fail(__field_1.error);
    auto __decoded_1 = (*__field_1.value).try_text();
    if (!__decoded_1) return DecodeResult<PeerPresence>::fail(__decoded_1.error);
    v.id = __decoded_1.value;
    auto __field_2 = c.try_get(2);
    if (!__field_2) return DecodeResult<PeerPresence>::fail(__field_2.error);
    auto __decoded_2 = (*__field_2.value).try_text();
    if (!__decoded_2) return DecodeResult<PeerPresence>::fail(__decoded_2.error);
    v.name = __decoded_2.value;
    auto __field_3 = c.try_get(3);
    if (!__field_3) return DecodeResult<PeerPresence>::fail(__field_3.error);
    auto __decoded_3_wire = (*__field_3.value).try_int();
    if (!__decoded_3_wire) return DecodeResult<PeerPresence>::fail(__decoded_3_wire.error);
    auto __decoded_3_enum = try_PresenceStatus_from_wire(__decoded_3_wire.value);
    if (!__decoded_3_enum) return DecodeResult<PeerPresence>::fail(__decoded_3_enum.error);
    v.status = __decoded_3_enum.value;
    auto __field_4 = c.try_get(4);
    if (!__field_4) return DecodeResult<PeerPresence>::fail(__field_4.error);
    auto __decoded_4 = (*__field_4.value).try_bool();
    if (!__decoded_4) return DecodeResult<PeerPresence>::fail(__decoded_4.error);
    v.online = __decoded_4.value;
    auto __field_5 = c.try_get(5);
    if (!__field_5) return DecodeResult<PeerPresence>::fail(__field_5.error);
    auto __decoded_5 = (*__field_5.value).try_text();
    if (!__decoded_5) return DecodeResult<PeerPresence>::fail(__decoded_5.error);
    v.location = __decoded_5.value;
    return DecodeResult<PeerPresence>::success(v);
  }
};

struct ByteOp {
  OpKind op;
  long long offset;
  long long length;
  std::string_view data;
  constexpr void to_cbor(Buf& b) const {
    b.map(4);
    b.uint(1);
    b.integer(static_cast<long long>(op));
    b.uint(2);
    b.integer(offset);
    b.uint(3);
    b.integer(length);
    b.uint(4);
    b.bytes(data);
  }
  static constexpr ByteOp from_cbor(const Cbor& c) {
    ByteOp v{};
    v.op = static_cast<OpKind>(c.get(1).as_int());
    v.offset = c.get(2).as_int();
    v.length = c.get(3).as_int();
    v.data = c.get(4).as_bytes();
    return v;
  }
  static DecodeResult<ByteOp> try_from_cbor(const Cbor& c) {
    ByteOp v{};
    auto __field_1 = c.try_get(1);
    if (!__field_1) return DecodeResult<ByteOp>::fail(__field_1.error);
    auto __decoded_1_wire = (*__field_1.value).try_int();
    if (!__decoded_1_wire) return DecodeResult<ByteOp>::fail(__decoded_1_wire.error);
    auto __decoded_1_enum = try_OpKind_from_wire(__decoded_1_wire.value);
    if (!__decoded_1_enum) return DecodeResult<ByteOp>::fail(__decoded_1_enum.error);
    v.op = __decoded_1_enum.value;
    auto __field_2 = c.try_get(2);
    if (!__field_2) return DecodeResult<ByteOp>::fail(__field_2.error);
    auto __decoded_2 = (*__field_2.value).try_int();
    if (!__decoded_2) return DecodeResult<ByteOp>::fail(__decoded_2.error);
    v.offset = __decoded_2.value;
    auto __field_3 = c.try_get(3);
    if (!__field_3) return DecodeResult<ByteOp>::fail(__field_3.error);
    auto __decoded_3 = (*__field_3.value).try_int();
    if (!__decoded_3) return DecodeResult<ByteOp>::fail(__decoded_3.error);
    v.length = __decoded_3.value;
    auto __field_4 = c.try_get(4);
    if (!__field_4) return DecodeResult<ByteOp>::fail(__field_4.error);
    auto __decoded_4 = (*__field_4.value).try_bytes();
    if (!__decoded_4) return DecodeResult<ByteOp>::fail(__decoded_4.error);
    v.data = __decoded_4.value;
    return DecodeResult<ByteOp>::success(v);
  }
};

struct FileSnapshot {
  std::string_view resource_id;
  long long resume_seq;
  std::string_view content;
  long long window_start;
  long long window_end;
  std::string_view preview;
  constexpr void to_cbor(Buf& b) const {
    b.map(5);
    b.uint(1);
    b.text(resource_id);
    b.uint(2);
    b.integer(resume_seq);
    b.uint(3);
    b.bytes(content);
    b.uint(4);
    b.integer(window_start);
    b.uint(5);
    b.integer(window_end);
  }
  static constexpr FileSnapshot from_cbor(const Cbor& c) {
    FileSnapshot v{};
    v.resource_id = c.get(1).as_text();
    v.resume_seq = c.get(2).as_int();
    v.content = c.get(3).as_bytes();
    v.window_start = c.get(4).as_int();
    v.window_end = c.get(5).as_int();
    return v;
  }
  static DecodeResult<FileSnapshot> try_from_cbor(const Cbor& c) {
    FileSnapshot v{};
    auto __field_1 = c.try_get(1);
    if (!__field_1) return DecodeResult<FileSnapshot>::fail(__field_1.error);
    auto __decoded_1 = (*__field_1.value).try_text();
    if (!__decoded_1) return DecodeResult<FileSnapshot>::fail(__decoded_1.error);
    v.resource_id = __decoded_1.value;
    auto __field_2 = c.try_get(2);
    if (!__field_2) return DecodeResult<FileSnapshot>::fail(__field_2.error);
    auto __decoded_2 = (*__field_2.value).try_int();
    if (!__decoded_2) return DecodeResult<FileSnapshot>::fail(__decoded_2.error);
    v.resume_seq = __decoded_2.value;
    auto __field_3 = c.try_get(3);
    if (!__field_3) return DecodeResult<FileSnapshot>::fail(__field_3.error);
    auto __decoded_3 = (*__field_3.value).try_bytes();
    if (!__decoded_3) return DecodeResult<FileSnapshot>::fail(__decoded_3.error);
    v.content = __decoded_3.value;
    auto __field_4 = c.try_get(4);
    if (!__field_4) return DecodeResult<FileSnapshot>::fail(__field_4.error);
    auto __decoded_4 = (*__field_4.value).try_int();
    if (!__decoded_4) return DecodeResult<FileSnapshot>::fail(__decoded_4.error);
    v.window_start = __decoded_4.value;
    auto __field_5 = c.try_get(5);
    if (!__field_5) return DecodeResult<FileSnapshot>::fail(__field_5.error);
    auto __decoded_5 = (*__field_5.value).try_int();
    if (!__decoded_5) return DecodeResult<FileSnapshot>::fail(__decoded_5.error);
    v.window_end = __decoded_5.value;
    return DecodeResult<FileSnapshot>::success(v);
  }
};

struct FileDelta {
  std::string_view resource_id;
  long long base_seq;
  long long seq;
  std::vector<ByteOp> ops;
  long long result_size;
  constexpr void to_cbor(Buf& b) const {
    b.map(5);
    b.uint(1);
    b.text(resource_id);
    b.uint(2);
    b.integer(base_seq);
    b.uint(3);
    b.integer(seq);
    b.uint(4);
    b.array(ops.size());
    for (const auto& x : ops) { x.to_cbor(b); }
    b.uint(5);
    b.integer(result_size);
  }
  static constexpr FileDelta from_cbor(const Cbor& c) {
    FileDelta v{};
    v.resource_id = c.get(1).as_text();
    v.base_seq = c.get(2).as_int();
    v.seq = c.get(3).as_int();
    for (const auto& x : c.get(4).as_array()) v.ops.push_back(taut::ByteOp::from_cbor(x));
    v.result_size = c.get(5).as_int();
    return v;
  }
  static DecodeResult<FileDelta> try_from_cbor(const Cbor& c) {
    FileDelta v{};
    auto __field_1 = c.try_get(1);
    if (!__field_1) return DecodeResult<FileDelta>::fail(__field_1.error);
    auto __decoded_1 = (*__field_1.value).try_text();
    if (!__decoded_1) return DecodeResult<FileDelta>::fail(__decoded_1.error);
    v.resource_id = __decoded_1.value;
    auto __field_2 = c.try_get(2);
    if (!__field_2) return DecodeResult<FileDelta>::fail(__field_2.error);
    auto __decoded_2 = (*__field_2.value).try_int();
    if (!__decoded_2) return DecodeResult<FileDelta>::fail(__decoded_2.error);
    v.base_seq = __decoded_2.value;
    auto __field_3 = c.try_get(3);
    if (!__field_3) return DecodeResult<FileDelta>::fail(__field_3.error);
    auto __decoded_3 = (*__field_3.value).try_int();
    if (!__decoded_3) return DecodeResult<FileDelta>::fail(__decoded_3.error);
    v.seq = __decoded_3.value;
    auto __field_4 = c.try_get(4);
    if (!__field_4) return DecodeResult<FileDelta>::fail(__field_4.error);
    auto __decoded_4_arr = (*__field_4.value).try_array();
    if (!__decoded_4_arr) return DecodeResult<FileDelta>::fail(__decoded_4_arr.error);
    v.ops.clear();
    for (const auto& x : *__decoded_4_arr.value) {
      ByteOp __decoded_4_item{};
      auto __decoded_4_elem_msg = ByteOp::try_from_cbor(x);
      if (!__decoded_4_elem_msg) return DecodeResult<FileDelta>::fail(__decoded_4_elem_msg.error);
      __decoded_4_item = __decoded_4_elem_msg.value;
      v.ops.push_back(__decoded_4_item);
    }
    auto __field_5 = c.try_get(5);
    if (!__field_5) return DecodeResult<FileDelta>::fail(__field_5.error);
    auto __decoded_5 = (*__field_5.value).try_int();
    if (!__decoded_5) return DecodeResult<FileDelta>::fail(__decoded_5.error);
    v.result_size = __decoded_5.value;
    return DecodeResult<FileDelta>::success(v);
  }
};

struct ChatMessage {
  long long id;
  std::string_view sender_id;
  std::string_view text;
  constexpr void to_cbor(Buf& b) const {
    b.map(3);
    b.uint(1);
    b.integer(id);
    b.uint(2);
    b.text(sender_id);
    b.uint(3);
    b.text(text);
  }
  static constexpr ChatMessage from_cbor(const Cbor& c) {
    ChatMessage v{};
    v.id = c.get(1).as_int();
    v.sender_id = c.get(2).as_text();
    v.text = c.get(3).as_text();
    return v;
  }
  static DecodeResult<ChatMessage> try_from_cbor(const Cbor& c) {
    ChatMessage v{};
    auto __field_1 = c.try_get(1);
    if (!__field_1) return DecodeResult<ChatMessage>::fail(__field_1.error);
    auto __decoded_1 = (*__field_1.value).try_int();
    if (!__decoded_1) return DecodeResult<ChatMessage>::fail(__decoded_1.error);
    v.id = __decoded_1.value;
    auto __field_2 = c.try_get(2);
    if (!__field_2) return DecodeResult<ChatMessage>::fail(__field_2.error);
    auto __decoded_2 = (*__field_2.value).try_text();
    if (!__decoded_2) return DecodeResult<ChatMessage>::fail(__decoded_2.error);
    v.sender_id = __decoded_2.value;
    auto __field_3 = c.try_get(3);
    if (!__field_3) return DecodeResult<ChatMessage>::fail(__field_3.error);
    auto __decoded_3 = (*__field_3.value).try_text();
    if (!__decoded_3) return DecodeResult<ChatMessage>::fail(__decoded_3.error);
    v.text = __decoded_3.value;
    return DecodeResult<ChatMessage>::success(v);
  }
};

struct TerminalChunk {
  std::string_view session_id;
  std::string_view data;
  constexpr void to_cbor(Buf& b) const {
    b.map(2);
    b.uint(1);
    b.text(session_id);
    b.uint(2);
    b.bytes(data);
  }
  static constexpr TerminalChunk from_cbor(const Cbor& c) {
    TerminalChunk v{};
    v.session_id = c.get(1).as_text();
    v.data = c.get(2).as_bytes();
    return v;
  }
  static DecodeResult<TerminalChunk> try_from_cbor(const Cbor& c) {
    TerminalChunk v{};
    auto __field_1 = c.try_get(1);
    if (!__field_1) return DecodeResult<TerminalChunk>::fail(__field_1.error);
    auto __decoded_1 = (*__field_1.value).try_text();
    if (!__decoded_1) return DecodeResult<TerminalChunk>::fail(__decoded_1.error);
    v.session_id = __decoded_1.value;
    auto __field_2 = c.try_get(2);
    if (!__field_2) return DecodeResult<TerminalChunk>::fail(__field_2.error);
    auto __decoded_2 = (*__field_2.value).try_bytes();
    if (!__decoded_2) return DecodeResult<TerminalChunk>::fail(__decoded_2.error);
    v.data = __decoded_2.value;
    return DecodeResult<TerminalChunk>::success(v);
  }
};

struct TerminalOpened {
  std::string_view session_id;
  std::string_view repo;
  long long cols;
  long long rows;
  constexpr void to_cbor(Buf& b) const {
    b.map(4);
    b.uint(1);
    b.text(session_id);
    b.uint(2);
    b.text(repo);
    b.uint(3);
    b.integer(cols);
    b.uint(4);
    b.integer(rows);
  }
  static constexpr TerminalOpened from_cbor(const Cbor& c) {
    TerminalOpened v{};
    v.session_id = c.get(1).as_text();
    v.repo = c.get(2).as_text();
    v.cols = c.get(3).as_int();
    v.rows = c.get(4).as_int();
    return v;
  }
  static DecodeResult<TerminalOpened> try_from_cbor(const Cbor& c) {
    TerminalOpened v{};
    auto __field_1 = c.try_get(1);
    if (!__field_1) return DecodeResult<TerminalOpened>::fail(__field_1.error);
    auto __decoded_1 = (*__field_1.value).try_text();
    if (!__decoded_1) return DecodeResult<TerminalOpened>::fail(__decoded_1.error);
    v.session_id = __decoded_1.value;
    auto __field_2 = c.try_get(2);
    if (!__field_2) return DecodeResult<TerminalOpened>::fail(__field_2.error);
    auto __decoded_2 = (*__field_2.value).try_text();
    if (!__decoded_2) return DecodeResult<TerminalOpened>::fail(__decoded_2.error);
    v.repo = __decoded_2.value;
    auto __field_3 = c.try_get(3);
    if (!__field_3) return DecodeResult<TerminalOpened>::fail(__field_3.error);
    auto __decoded_3 = (*__field_3.value).try_int();
    if (!__decoded_3) return DecodeResult<TerminalOpened>::fail(__decoded_3.error);
    v.cols = __decoded_3.value;
    auto __field_4 = c.try_get(4);
    if (!__field_4) return DecodeResult<TerminalOpened>::fail(__field_4.error);
    auto __decoded_4 = (*__field_4.value).try_int();
    if (!__decoded_4) return DecodeResult<TerminalOpened>::fail(__decoded_4.error);
    v.rows = __decoded_4.value;
    return DecodeResult<TerminalOpened>::success(v);
  }
};

struct RepoTarget {
  std::string_view repo;
  long long fail_with;
  constexpr void to_cbor(Buf& b) const {
    b.map(2);
    b.uint(1);
    b.text(repo);
    b.uint(2);
    b.integer(fail_with);
  }
  static constexpr RepoTarget from_cbor(const Cbor& c) {
    RepoTarget v{};
    v.repo = c.get(1).as_text();
    v.fail_with = c.get(2).as_int();
    return v;
  }
  static DecodeResult<RepoTarget> try_from_cbor(const Cbor& c) {
    RepoTarget v{};
    auto __field_1 = c.try_get(1);
    if (!__field_1) return DecodeResult<RepoTarget>::fail(__field_1.error);
    auto __decoded_1 = (*__field_1.value).try_text();
    if (!__decoded_1) return DecodeResult<RepoTarget>::fail(__decoded_1.error);
    v.repo = __decoded_1.value;
    auto __field_2 = c.try_get(2);
    if (!__field_2) return DecodeResult<RepoTarget>::fail(__field_2.error);
    auto __decoded_2 = (*__field_2.value).try_int();
    if (!__decoded_2) return DecodeResult<RepoTarget>::fail(__decoded_2.error);
    v.fail_with = __decoded_2.value;
    return DecodeResult<RepoTarget>::success(v);
  }
};

struct RepoRun {
  std::string_view repo;
  long long exit_code;
  std::string_view output;
  std::optional<std::string_view> error;
  constexpr void to_cbor(Buf& b) const {
    b.map(4);
    b.uint(1);
    b.text(repo);
    b.uint(2);
    b.integer(exit_code);
    b.uint(3);
    b.text(output);
    b.uint(4);
    if (error.has_value()) { b.text((*error)); } else { b.null_(); }
  }
  static constexpr RepoRun from_cbor(const Cbor& c) {
    RepoRun v{};
    v.repo = c.get(1).as_text();
    v.exit_code = c.get(2).as_int();
    v.output = c.get(3).as_text();
    { const auto& f = c.get(4); if (!f.is_null()) v.error = f.as_text(); }
    return v;
  }
  static DecodeResult<RepoRun> try_from_cbor(const Cbor& c) {
    RepoRun v{};
    auto __field_1 = c.try_get(1);
    if (!__field_1) return DecodeResult<RepoRun>::fail(__field_1.error);
    auto __decoded_1 = (*__field_1.value).try_text();
    if (!__decoded_1) return DecodeResult<RepoRun>::fail(__decoded_1.error);
    v.repo = __decoded_1.value;
    auto __field_2 = c.try_get(2);
    if (!__field_2) return DecodeResult<RepoRun>::fail(__field_2.error);
    auto __decoded_2 = (*__field_2.value).try_int();
    if (!__decoded_2) return DecodeResult<RepoRun>::fail(__decoded_2.error);
    v.exit_code = __decoded_2.value;
    auto __field_3 = c.try_get(3);
    if (!__field_3) return DecodeResult<RepoRun>::fail(__field_3.error);
    auto __decoded_3 = (*__field_3.value).try_text();
    if (!__decoded_3) return DecodeResult<RepoRun>::fail(__decoded_3.error);
    v.output = __decoded_3.value;
    auto __field_4 = c.try_get(4);
    if (!__field_4) return DecodeResult<RepoRun>::fail(__field_4.error);
    if (__field_4.value->is_null()) {
      v.error = std::nullopt;
    } else {
      std::string_view __value_4{};
      auto __decoded_4 = (*__field_4.value).try_text();
      if (!__decoded_4) return DecodeResult<RepoRun>::fail(__decoded_4.error);
      __value_4 = __decoded_4.value;
      v.error = __value_4;
    }
    return DecodeResult<RepoRun>::success(v);
  }
};

struct CmdSession {
  std::string_view session_id;
  std::vector<std::string_view> argv;
  std::vector<RepoRun> targets;
  long long started_monotonic;
  constexpr void to_cbor(Buf& b) const {
    b.map(3);
    b.uint(1);
    b.text(session_id);
    b.uint(2);
    b.array(argv.size());
    for (const auto& x : argv) { b.text(x); }
    b.uint(3);
    b.array(targets.size());
    for (const auto& x : targets) { x.to_cbor(b); }
  }
  static constexpr CmdSession from_cbor(const Cbor& c) {
    CmdSession v{};
    v.session_id = c.get(1).as_text();
    for (const auto& x : c.get(2).as_array()) v.argv.push_back(x.as_text());
    for (const auto& x : c.get(3).as_array()) v.targets.push_back(taut::RepoRun::from_cbor(x));
    return v;
  }
  static DecodeResult<CmdSession> try_from_cbor(const Cbor& c) {
    CmdSession v{};
    auto __field_1 = c.try_get(1);
    if (!__field_1) return DecodeResult<CmdSession>::fail(__field_1.error);
    auto __decoded_1 = (*__field_1.value).try_text();
    if (!__decoded_1) return DecodeResult<CmdSession>::fail(__decoded_1.error);
    v.session_id = __decoded_1.value;
    auto __field_2 = c.try_get(2);
    if (!__field_2) return DecodeResult<CmdSession>::fail(__field_2.error);
    auto __decoded_2_arr = (*__field_2.value).try_array();
    if (!__decoded_2_arr) return DecodeResult<CmdSession>::fail(__decoded_2_arr.error);
    v.argv.clear();
    for (const auto& x : *__decoded_2_arr.value) {
      std::string_view __decoded_2_item{};
      auto __decoded_2_elem = (x).try_text();
      if (!__decoded_2_elem) return DecodeResult<CmdSession>::fail(__decoded_2_elem.error);
      __decoded_2_item = __decoded_2_elem.value;
      v.argv.push_back(__decoded_2_item);
    }
    auto __field_3 = c.try_get(3);
    if (!__field_3) return DecodeResult<CmdSession>::fail(__field_3.error);
    auto __decoded_3_arr = (*__field_3.value).try_array();
    if (!__decoded_3_arr) return DecodeResult<CmdSession>::fail(__decoded_3_arr.error);
    v.targets.clear();
    for (const auto& x : *__decoded_3_arr.value) {
      RepoRun __decoded_3_item{};
      auto __decoded_3_elem_msg = RepoRun::try_from_cbor(x);
      if (!__decoded_3_elem_msg) return DecodeResult<CmdSession>::fail(__decoded_3_elem_msg.error);
      __decoded_3_item = __decoded_3_elem_msg.value;
      v.targets.push_back(__decoded_3_item);
    }
    return DecodeResult<CmdSession>::success(v);
  }
};

struct CrdtOp {
  std::string_view doc;
  std::string_view actor;
  long long seq;
  long long field;
  std::string_view value;
  constexpr void to_cbor(Buf& b) const {
    b.map(5);
    b.uint(1);
    b.text(doc);
    b.uint(2);
    b.text(actor);
    b.uint(3);
    b.integer(seq);
    b.uint(4);
    b.integer(field);
    b.uint(5);
    b.bytes(value);
  }
  static constexpr CrdtOp from_cbor(const Cbor& c) {
    CrdtOp v{};
    v.doc = c.get(1).as_text();
    v.actor = c.get(2).as_text();
    v.seq = c.get(3).as_int();
    v.field = c.get(4).as_int();
    v.value = c.get(5).as_bytes();
    return v;
  }
  static DecodeResult<CrdtOp> try_from_cbor(const Cbor& c) {
    CrdtOp v{};
    auto __field_1 = c.try_get(1);
    if (!__field_1) return DecodeResult<CrdtOp>::fail(__field_1.error);
    auto __decoded_1 = (*__field_1.value).try_text();
    if (!__decoded_1) return DecodeResult<CrdtOp>::fail(__decoded_1.error);
    v.doc = __decoded_1.value;
    auto __field_2 = c.try_get(2);
    if (!__field_2) return DecodeResult<CrdtOp>::fail(__field_2.error);
    auto __decoded_2 = (*__field_2.value).try_text();
    if (!__decoded_2) return DecodeResult<CrdtOp>::fail(__decoded_2.error);
    v.actor = __decoded_2.value;
    auto __field_3 = c.try_get(3);
    if (!__field_3) return DecodeResult<CrdtOp>::fail(__field_3.error);
    auto __decoded_3 = (*__field_3.value).try_int();
    if (!__decoded_3) return DecodeResult<CrdtOp>::fail(__decoded_3.error);
    v.seq = __decoded_3.value;
    auto __field_4 = c.try_get(4);
    if (!__field_4) return DecodeResult<CrdtOp>::fail(__field_4.error);
    auto __decoded_4 = (*__field_4.value).try_int();
    if (!__decoded_4) return DecodeResult<CrdtOp>::fail(__decoded_4.error);
    v.field = __decoded_4.value;
    auto __field_5 = c.try_get(5);
    if (!__field_5) return DecodeResult<CrdtOp>::fail(__field_5.error);
    auto __decoded_5 = (*__field_5.value).try_bytes();
    if (!__decoded_5) return DecodeResult<CrdtOp>::fail(__decoded_5.error);
    v.value = __decoded_5.value;
    return DecodeResult<CrdtOp>::success(v);
  }
};

struct VersionEntry {
  std::string_view actor;
  long long seq;
  constexpr void to_cbor(Buf& b) const {
    b.map(2);
    b.uint(1);
    b.text(actor);
    b.uint(2);
    b.integer(seq);
  }
  static constexpr VersionEntry from_cbor(const Cbor& c) {
    VersionEntry v{};
    v.actor = c.get(1).as_text();
    v.seq = c.get(2).as_int();
    return v;
  }
  static DecodeResult<VersionEntry> try_from_cbor(const Cbor& c) {
    VersionEntry v{};
    auto __field_1 = c.try_get(1);
    if (!__field_1) return DecodeResult<VersionEntry>::fail(__field_1.error);
    auto __decoded_1 = (*__field_1.value).try_text();
    if (!__decoded_1) return DecodeResult<VersionEntry>::fail(__decoded_1.error);
    v.actor = __decoded_1.value;
    auto __field_2 = c.try_get(2);
    if (!__field_2) return DecodeResult<VersionEntry>::fail(__field_2.error);
    auto __decoded_2 = (*__field_2.value).try_int();
    if (!__decoded_2) return DecodeResult<VersionEntry>::fail(__decoded_2.error);
    v.seq = __decoded_2.value;
    return DecodeResult<VersionEntry>::success(v);
  }
};

struct CrdtState {
  std::string_view doc;
  std::vector<CrdtOp> ops;
  std::vector<VersionEntry> version;
  constexpr void to_cbor(Buf& b) const {
    b.map(3);
    b.uint(1);
    b.text(doc);
    b.uint(2);
    b.array(ops.size());
    for (const auto& x : ops) { x.to_cbor(b); }
    b.uint(3);
    b.array(version.size());
    for (const auto& x : version) { x.to_cbor(b); }
  }
  static constexpr CrdtState from_cbor(const Cbor& c) {
    CrdtState v{};
    v.doc = c.get(1).as_text();
    for (const auto& x : c.get(2).as_array()) v.ops.push_back(taut::CrdtOp::from_cbor(x));
    for (const auto& x : c.get(3).as_array()) v.version.push_back(taut::VersionEntry::from_cbor(x));
    return v;
  }
  static DecodeResult<CrdtState> try_from_cbor(const Cbor& c) {
    CrdtState v{};
    auto __field_1 = c.try_get(1);
    if (!__field_1) return DecodeResult<CrdtState>::fail(__field_1.error);
    auto __decoded_1 = (*__field_1.value).try_text();
    if (!__decoded_1) return DecodeResult<CrdtState>::fail(__decoded_1.error);
    v.doc = __decoded_1.value;
    auto __field_2 = c.try_get(2);
    if (!__field_2) return DecodeResult<CrdtState>::fail(__field_2.error);
    auto __decoded_2_arr = (*__field_2.value).try_array();
    if (!__decoded_2_arr) return DecodeResult<CrdtState>::fail(__decoded_2_arr.error);
    v.ops.clear();
    for (const auto& x : *__decoded_2_arr.value) {
      CrdtOp __decoded_2_item{};
      auto __decoded_2_elem_msg = CrdtOp::try_from_cbor(x);
      if (!__decoded_2_elem_msg) return DecodeResult<CrdtState>::fail(__decoded_2_elem_msg.error);
      __decoded_2_item = __decoded_2_elem_msg.value;
      v.ops.push_back(__decoded_2_item);
    }
    auto __field_3 = c.try_get(3);
    if (!__field_3) return DecodeResult<CrdtState>::fail(__field_3.error);
    auto __decoded_3_arr = (*__field_3.value).try_array();
    if (!__decoded_3_arr) return DecodeResult<CrdtState>::fail(__decoded_3_arr.error);
    v.version.clear();
    for (const auto& x : *__decoded_3_arr.value) {
      VersionEntry __decoded_3_item{};
      auto __decoded_3_elem_msg = VersionEntry::try_from_cbor(x);
      if (!__decoded_3_elem_msg) return DecodeResult<CrdtState>::fail(__decoded_3_elem_msg.error);
      __decoded_3_item = __decoded_3_elem_msg.value;
      v.version.push_back(__decoded_3_item);
    }
    return DecodeResult<CrdtState>::success(v);
  }
};

struct Board {
  std::string_view title;
  long long votes;
  constexpr void to_cbor(Buf& b) const {
    b.map(2);
    b.uint(1);
    b.text(title);
    b.uint(2);
    b.integer(votes);
  }
  static constexpr Board from_cbor(const Cbor& c) {
    Board v{};
    v.title = c.get(1).as_text();
    v.votes = c.get(2).as_int();
    return v;
  }
  static DecodeResult<Board> try_from_cbor(const Cbor& c) {
    Board v{};
    auto __field_1 = c.try_get(1);
    if (!__field_1) return DecodeResult<Board>::fail(__field_1.error);
    auto __decoded_1 = (*__field_1.value).try_text();
    if (!__decoded_1) return DecodeResult<Board>::fail(__decoded_1.error);
    v.title = __decoded_1.value;
    auto __field_2 = c.try_get(2);
    if (!__field_2) return DecodeResult<Board>::fail(__field_2.error);
    auto __decoded_2 = (*__field_2.value).try_int();
    if (!__decoded_2) return DecodeResult<Board>::fail(__decoded_2.error);
    v.votes = __decoded_2.value;
    return DecodeResult<Board>::success(v);
  }
};

} // namespace taut
