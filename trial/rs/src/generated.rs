// GENERATED from taut/ir + corpus by taut/src/taut/gen/rust.py — do not edit.
#![allow(dead_code)]
use crate::cbor::Cbor;

#[derive(Clone, Copy, Debug, PartialEq, Default)]
pub enum PresenceStatus {
    #[default] Configured,
    Offline,
    Starting,
    Online,
    Error,
}
impl PresenceStatus {
    pub fn wire(self) -> i64 { match self {
        Self::Configured => 0,
        Self::Offline => 1,
        Self::Starting => 2,
        Self::Online => 3,
        Self::Error => 4,
    } }
    pub fn from_wire(v: i64) -> Self { match v {
        0 => Self::Configured,
        1 => Self::Offline,
        2 => Self::Starting,
        3 => Self::Online,
        4 => Self::Error,
        _ => panic!("bad PresenceStatus wire value {}", v),
    } }
}

#[derive(Clone, Copy, Debug, PartialEq, Default)]
pub enum OpKind {
    #[default] Insert,
    Delete,
    Replace,
}
impl OpKind {
    pub fn wire(self) -> i64 { match self {
        Self::Insert => 0,
        Self::Delete => 1,
        Self::Replace => 2,
    } }
    pub fn from_wire(v: i64) -> Self { match v {
        0 => Self::Insert,
        1 => Self::Delete,
        2 => Self::Replace,
        _ => panic!("bad OpKind wire value {}", v),
    } }
}

#[derive(Clone, Debug, PartialEq, Default)]
pub struct PeerPresence {
    pub id: String,
    pub name: String,
    pub status: PresenceStatus,
    pub online: bool,
    pub location: String,
}
impl PeerPresence {
    pub fn to_cbor(&self) -> Cbor {
        Cbor::Map(vec![
            (1, Cbor::Text(self.id.clone())),
            (2, Cbor::Text(self.name.clone())),
            (3, Cbor::Int(self.status.wire())),
            (4, Cbor::Bool(self.online)),
            (5, Cbor::Text(self.location.clone())),
        ])
    }
    pub fn from_cbor(c: &Cbor) -> Self {
        Self {
            id: c.get(1).text(),
            name: c.get(2).text(),
            status: PresenceStatus::from_wire(c.get(3).int()),
            online: c.get(4).boolean(),
            location: c.get(5).text(),
        }
    }
}

#[derive(Clone, Debug, PartialEq, Default)]
pub struct ByteOp {
    pub op: OpKind,
    pub offset: i64,
    pub length: i64,
    pub data: Vec<u8>,
}
impl ByteOp {
    pub fn to_cbor(&self) -> Cbor {
        Cbor::Map(vec![
            (1, Cbor::Int(self.op.wire())),
            (2, Cbor::Int(self.offset)),
            (3, Cbor::Int(self.length)),
            (4, Cbor::Bytes(self.data.clone())),
        ])
    }
    pub fn from_cbor(c: &Cbor) -> Self {
        Self {
            op: OpKind::from_wire(c.get(1).int()),
            offset: c.get(2).int(),
            length: c.get(3).int(),
            data: c.get(4).bytes(),
        }
    }
}

#[derive(Clone, Debug, PartialEq, Default)]
pub struct FileSnapshot {
    pub resource_id: String,
    pub resume_seq: i64,
    pub content: Vec<u8>,
    pub window_start: i64,
    pub window_end: i64,
    pub preview: String,
}
impl FileSnapshot {
    pub fn to_cbor(&self) -> Cbor {
        Cbor::Map(vec![
            (1, Cbor::Text(self.resource_id.clone())),
            (2, Cbor::Int(self.resume_seq)),
            (3, Cbor::Bytes(self.content.clone())),
            (4, Cbor::Int(self.window_start)),
            (5, Cbor::Int(self.window_end)),
        ])
    }
    pub fn from_cbor(c: &Cbor) -> Self {
        Self {
            resource_id: c.get(1).text(),
            resume_seq: c.get(2).int(),
            content: c.get(3).bytes(),
            window_start: c.get(4).int(),
            window_end: c.get(5).int(),
            preview: Default::default(),
        }
    }
}

#[derive(Clone, Debug, PartialEq, Default)]
pub struct FileDelta {
    pub resource_id: String,
    pub base_seq: i64,
    pub seq: i64,
    pub ops: Vec<ByteOp>,
    pub result_size: i64,
}
impl FileDelta {
    pub fn to_cbor(&self) -> Cbor {
        Cbor::Map(vec![
            (1, Cbor::Text(self.resource_id.clone())),
            (2, Cbor::Int(self.base_seq)),
            (3, Cbor::Int(self.seq)),
            (4, Cbor::Array(self.ops.iter().map(|x| x.to_cbor()).collect())),
            (5, Cbor::Int(self.result_size)),
        ])
    }
    pub fn from_cbor(c: &Cbor) -> Self {
        Self {
            resource_id: c.get(1).text(),
            base_seq: c.get(2).int(),
            seq: c.get(3).int(),
            ops: c.get(4).array().iter().map(|x| ByteOp::from_cbor(x)).collect(),
            result_size: c.get(5).int(),
        }
    }
}

#[derive(Clone, Debug, PartialEq, Default)]
pub struct ChatMessage {
    pub id: i64,
    pub sender_id: String,
    pub text: String,
}
impl ChatMessage {
    pub fn to_cbor(&self) -> Cbor {
        Cbor::Map(vec![
            (1, Cbor::Int(self.id)),
            (2, Cbor::Text(self.sender_id.clone())),
            (3, Cbor::Text(self.text.clone())),
        ])
    }
    pub fn from_cbor(c: &Cbor) -> Self {
        Self {
            id: c.get(1).int(),
            sender_id: c.get(2).text(),
            text: c.get(3).text(),
        }
    }
}

#[derive(Clone, Debug, PartialEq, Default)]
pub struct TerminalChunk {
    pub session_id: String,
    pub data: Vec<u8>,
}
impl TerminalChunk {
    pub fn to_cbor(&self) -> Cbor {
        Cbor::Map(vec![
            (1, Cbor::Text(self.session_id.clone())),
            (2, Cbor::Bytes(self.data.clone())),
        ])
    }
    pub fn from_cbor(c: &Cbor) -> Self {
        Self {
            session_id: c.get(1).text(),
            data: c.get(2).bytes(),
        }
    }
}

#[derive(Clone, Debug, PartialEq, Default)]
pub struct TerminalOpened {
    pub session_id: String,
    pub repo: String,
    pub cols: i64,
    pub rows: i64,
}
impl TerminalOpened {
    pub fn to_cbor(&self) -> Cbor {
        Cbor::Map(vec![
            (1, Cbor::Text(self.session_id.clone())),
            (2, Cbor::Text(self.repo.clone())),
            (3, Cbor::Int(self.cols)),
            (4, Cbor::Int(self.rows)),
        ])
    }
    pub fn from_cbor(c: &Cbor) -> Self {
        Self {
            session_id: c.get(1).text(),
            repo: c.get(2).text(),
            cols: c.get(3).int(),
            rows: c.get(4).int(),
        }
    }
}

#[derive(Clone, Debug, PartialEq, Default)]
pub struct RepoTarget {
    pub repo: String,
    pub fail_with: i64,
}
impl RepoTarget {
    pub fn to_cbor(&self) -> Cbor {
        Cbor::Map(vec![
            (1, Cbor::Text(self.repo.clone())),
            (2, Cbor::Int(self.fail_with)),
        ])
    }
    pub fn from_cbor(c: &Cbor) -> Self {
        Self {
            repo: c.get(1).text(),
            fail_with: c.get(2).int(),
        }
    }
}

#[derive(Clone, Debug, PartialEq, Default)]
pub struct RepoRun {
    pub repo: String,
    pub exit_code: i64,
    pub output: String,
    pub error: Option<String>,
}
impl RepoRun {
    pub fn to_cbor(&self) -> Cbor {
        Cbor::Map(vec![
            (1, Cbor::Text(self.repo.clone())),
            (2, Cbor::Int(self.exit_code)),
            (3, Cbor::Text(self.output.clone())),
            (4, match &self.error { Some(v) => Cbor::Text(v.clone()), None => Cbor::Null }),
        ])
    }
    pub fn from_cbor(c: &Cbor) -> Self {
        Self {
            repo: c.get(1).text(),
            exit_code: c.get(2).int(),
            output: c.get(3).text(),
            error: { let v = c.get(4); if v.is_null() { None } else { Some(v.text()) } },
        }
    }
}

#[derive(Clone, Debug, PartialEq, Default)]
pub struct CmdSession {
    pub session_id: String,
    pub argv: Vec<String>,
    pub targets: Vec<RepoRun>,
    pub started_monotonic: i64,
}
impl CmdSession {
    pub fn to_cbor(&self) -> Cbor {
        Cbor::Map(vec![
            (1, Cbor::Text(self.session_id.clone())),
            (2, Cbor::Array(self.argv.iter().map(|x| Cbor::Text(x.clone())).collect())),
            (3, Cbor::Array(self.targets.iter().map(|x| x.to_cbor()).collect())),
        ])
    }
    pub fn from_cbor(c: &Cbor) -> Self {
        Self {
            session_id: c.get(1).text(),
            argv: c.get(2).array().iter().map(|x| x.text()).collect(),
            targets: c.get(3).array().iter().map(|x| RepoRun::from_cbor(x)).collect(),
            started_monotonic: Default::default(),
        }
    }
}

#[derive(Clone, Debug, PartialEq, Default)]
pub struct CrdtOp {
    pub doc: String,
    pub actor: String,
    pub seq: i64,
    pub field: i64,
    pub value: Vec<u8>,
}
impl CrdtOp {
    pub fn to_cbor(&self) -> Cbor {
        Cbor::Map(vec![
            (1, Cbor::Text(self.doc.clone())),
            (2, Cbor::Text(self.actor.clone())),
            (3, Cbor::Int(self.seq)),
            (4, Cbor::Int(self.field)),
            (5, Cbor::Bytes(self.value.clone())),
        ])
    }
    pub fn from_cbor(c: &Cbor) -> Self {
        Self {
            doc: c.get(1).text(),
            actor: c.get(2).text(),
            seq: c.get(3).int(),
            field: c.get(4).int(),
            value: c.get(5).bytes(),
        }
    }
}

#[derive(Clone, Debug, PartialEq, Default)]
pub struct VersionEntry {
    pub actor: String,
    pub seq: i64,
}
impl VersionEntry {
    pub fn to_cbor(&self) -> Cbor {
        Cbor::Map(vec![
            (1, Cbor::Text(self.actor.clone())),
            (2, Cbor::Int(self.seq)),
        ])
    }
    pub fn from_cbor(c: &Cbor) -> Self {
        Self {
            actor: c.get(1).text(),
            seq: c.get(2).int(),
        }
    }
}

#[derive(Clone, Debug, PartialEq, Default)]
pub struct CrdtState {
    pub doc: String,
    pub ops: Vec<CrdtOp>,
    pub version: Vec<VersionEntry>,
}
impl CrdtState {
    pub fn to_cbor(&self) -> Cbor {
        Cbor::Map(vec![
            (1, Cbor::Text(self.doc.clone())),
            (2, Cbor::Array(self.ops.iter().map(|x| x.to_cbor()).collect())),
            (3, Cbor::Array(self.version.iter().map(|x| x.to_cbor()).collect())),
        ])
    }
    pub fn from_cbor(c: &Cbor) -> Self {
        Self {
            doc: c.get(1).text(),
            ops: c.get(2).array().iter().map(|x| CrdtOp::from_cbor(x)).collect(),
            version: c.get(3).array().iter().map(|x| VersionEntry::from_cbor(x)).collect(),
        }
    }
}

#[derive(Clone, Debug, PartialEq, Default)]
pub struct Board {
    pub title: String,
    pub votes: i64,
}
impl Board {
    pub fn to_cbor(&self) -> Cbor {
        Cbor::Map(vec![
            (1, Cbor::Text(self.title.clone())),
            (2, Cbor::Int(self.votes)),
        ])
    }
    pub fn from_cbor(c: &Cbor) -> Self {
        Self {
            title: c.get(1).text(),
            votes: c.get(2).int(),
        }
    }
}

pub fn roundtrip(message: &str, bytes: &[u8]) -> Vec<u8> {
    let c = crate::cbor::decode(bytes);
    match message {
        "PeerPresence" => crate::cbor::encode(&PeerPresence::from_cbor(&c).to_cbor()),
        "ByteOp" => crate::cbor::encode(&ByteOp::from_cbor(&c).to_cbor()),
        "FileSnapshot" => crate::cbor::encode(&FileSnapshot::from_cbor(&c).to_cbor()),
        "FileDelta" => crate::cbor::encode(&FileDelta::from_cbor(&c).to_cbor()),
        "ChatMessage" => crate::cbor::encode(&ChatMessage::from_cbor(&c).to_cbor()),
        "TerminalChunk" => crate::cbor::encode(&TerminalChunk::from_cbor(&c).to_cbor()),
        "TerminalOpened" => crate::cbor::encode(&TerminalOpened::from_cbor(&c).to_cbor()),
        "RepoTarget" => crate::cbor::encode(&RepoTarget::from_cbor(&c).to_cbor()),
        "RepoRun" => crate::cbor::encode(&RepoRun::from_cbor(&c).to_cbor()),
        "CmdSession" => crate::cbor::encode(&CmdSession::from_cbor(&c).to_cbor()),
        "CrdtOp" => crate::cbor::encode(&CrdtOp::from_cbor(&c).to_cbor()),
        "VersionEntry" => crate::cbor::encode(&VersionEntry::from_cbor(&c).to_cbor()),
        "CrdtState" => crate::cbor::encode(&CrdtState::from_cbor(&c).to_cbor()),
        "Board" => crate::cbor::encode(&Board::from_cbor(&c).to_cbor()),
        _ => panic!("unknown message {}", message),
    }
}

pub static VECTORS: &[(&str, &str, &str)] = &[
    ("Board/materialized", "Board", "a20165776f726c640208"),
    ("ByteOp/replace", "ByteOp", "a40102020303020443007879"),
    ("ChatMessage/first", "ChatMessage", "a301000263616e6e03626869"),
    ("CmdSession/mixed", "CmdSession", "a30169736573732d30303031028263676974667374617475730382a40165616c7068610200037267697420737461747573204020616c70686104f6a401646265746102020360046e626574613a206578697465642032"),
    ("PeerPresence/online", "PeerPresence", "a5016270310263416e6e030304f505634e5943"),
    ("RepoRun/err", "RepoRun", "a401646265746102020360046e626574613a206578697465642032"),
    ("RepoRun/ok", "RepoRun", "a40165616c7068610200037267697420737461747573204020616c70686104f6"),
    ("TerminalChunk/ls", "TerminalChunk", "a201667465726d2d31024524206c730a"),
    ("TerminalOpened/alpha", "TerminalOpened", "a401667465726d2d310265616c706861031878041828"),
    ("crdt/op", "CrdtOp", "a50167626f6172643a3102614103010402054103"),
    ("crdt/state", "CrdtState", "a30167626f6172643a310282a50167626f6172643a3102614103010402054103a50167626f6172643a31026142030104010545776f726c640382a20161410201a20161420201"),
    ("swmr/delta-1", "FileDelta", "a5016e66696c653a6e6f7465732e747874020103020481a40100020b0300044121050c"),
    ("swmr/delta-2", "FileDelta", "a5016e66696c653a6e6f7465732e747874020203030481a401010200030504400507"),
    ("swmr/snapshot", "FileSnapshot", "a5016e66696c653a6e6f7465732e7478740201034b68656c6c6f20776f726c6404000520"),
];
